#include <string>
#include <cstring>
#include <cassert>
#include <vector>
#include "map/load.h"
#include "actors/junction_and_roads.h"
#include "mail/message.h"
#include "payload/datatype.h"
#include "payload/summary.h"
#include "map/search.h"
#include "util/random.h"
#include "util/disjoint_set.h"
#include "constants/constants.h"

actor::JunctionAndRoads::JunctionAndRoads(actor::id id,
                                          int factory_id,
                                          int summary_id,
                                          int initial_vehicle_id,
                                          int initial_vehicle_size,
                                          map::RoadMapInfo &road_map_info) :
        Actor(id), factory_id(factory_id), summary_id(summary_id),
        initial_vehicle_id(initial_vehicle_id), initial_vehicle_size(initial_vehicle_size),
        road_map_info(road_map_info) {}

bool actor::JunctionAndRoads::pre_barrier_init() {

    // Load road map from file
    auto success = map::load(road_map_info, road_map);
    if (!success) {
        fprintf(stderr, "failed to load %s\n", road_map_info.filename.c_str());
        return false;
    }

    // Extract junction for a given id from road map
    auto node = road_map[id];
    if (node.id != id) {
        fprintf(stderr, "expected id to match\n");
        return false;
    }

    junction = data::Junction(node.id, node.has_traffic_lights);
    junction.current_number_vehicles = static_cast<int>(vehicles.size());
    junction.summary.total_number_vehicles = static_cast<int>(vehicles.size());

    // Initialize roads for this junction
    for (int edge_id = 0; edge_id < node.roads.size(); edge_id++) {
        auto edge = node.roads[edge_id];
        roads.emplace_back(edge_id, id, edge.dest->id, edge.road_length, edge.max_speed);
    }

    // Initialize vehicles starting from this junction
    if (initial_vehicle_size > 0 && !node.roads.empty()) {

        // Create disjoint set
        auto num_junctions = static_cast<int>(road_map.size());
        auto disjoint_set = DisjointSet(num_junctions);
        for (int source_id = 0; source_id < num_junctions; source_id++) {
            for (const auto &road: road_map[source_id].roads) {
                disjoint_set.connect(source_id, road.dest->id);
            }
        }

        auto components = disjoint_set.get_connected_components();
        auto num_vehicle_types = static_cast<int>(vehicle_attrs.size());
        auto vehicle_id_end = initial_vehicle_id + initial_vehicle_size - 1;
        for (int vehicle_id = initial_vehicle_id; vehicle_id <= vehicle_id_end; vehicle_id++) {

            auto vehicle_type = static_cast<VehicleType>(get_random_integer(0, num_vehicle_types));
            auto vehicle = payload::Vehicle(vehicle_id, vehicle_type, road_map_info);
            vehicle.source_id = junction.id;
            vehicle.dest_id = generate_vehicle_destination(vehicle.source_id, disjoint_set, components);
            vehicle.on_junction = true;
            vehicle.current_road = NULL;
            vehicle.start_time = 0;

            vehicles[vehicle.id] = vehicle;
            junction.current_number_vehicles++;
        }

        junction.summary.total_number_vehicles += initial_vehicle_size;
    }

    periodic_summary = payload::PeriodicSummary();

    return true;
}

bool actor::JunctionAndRoads::post_barrier_init() {

    timer = Timer(MIN_LENGTH_SECONDS);

    auto current_seconds = Timer::get_current_seconds();
    for (const auto &kv: vehicles) {
        auto id = kv.first;
        vehicles[id].start_time = current_seconds;
    }

    return true;
}

actor::next_step actor::JunctionAndRoads::ingress(mail::Message &message) {

    if (message.mpi_datatype == MPI_VEHICLE) {
        process_vehicles(message);
        return actor::CONTINUE;
    } else if (message.mpi_datatype == MPI_TERMINATE) {
        send_final_summaries();
        return actor::STOP;
    } else {
        fprintf(stderr, "received unexpected message type");
        return actor::STOP;
    }
}

actor::next_step actor::JunctionAndRoads::run() {

    // If junction has traffic lights, switch enabled road every simulated minute.
    switch_enabled_road_at_traffic_light();

    // Move vehicles
    std::vector<int> vehicles_to_be_removed;
    for (const auto &kv: vehicles) {
        auto i = kv.first;

        // If vehicle exhausts fuel, remove it from simulation.
        if (Timer::get_current_seconds() - vehicles[i].start_time > vehicles[i].fuel) {
            remove_vehicle_due_to_fuel_exhaustion(i);
            vehicles_to_be_removed.push_back(i);
            continue;
        }

        if (vehicles[i].on_junction) {

            if (vehicles[i].current_road == NULL) {
                assign_road_to_vehicle(i);
            }

            if (junction.has_traffic_lights) {
                auto success = vehicle_exits_junction_with_traffic_lights(i);
                if (!success) {
                    continue;
                }
            } else {
                auto success = vehicle_exits_junction_without_traffic_lights(i);
                if (!success) {
                    vehicles_to_be_removed.push_back(i);
                    continue;
                }
            }

            vehicle_enters_road(i);
        }

        assert(vehicles[i].on_junction == false);
        assert(vehicles[i].current_road != NULL);

        move_vehicle_along_road(i);
        if (vehicles[i].remaining_distance <= 0) {
            vehicle_exits_road(i);
            vehicles_to_be_removed.push_back(i);
            continue;
        }
    }

    for (const auto &i: vehicles_to_be_removed) {
        vehicles.erase(i);
    }

    send_statistics(periodic_summary);
    periodic_summary = payload::PeriodicSummary();

    return actor::CONTINUE;
}

/**
 * Select a valid destination for the given source junction.
 */
int actor::JunctionAndRoads::generate_vehicle_destination(int source, DisjointSet &disjoint_set,
                                                          std::unordered_map<int, std::vector<int>> &components) {

    // In the `disjoint_set` data structure, each junction is assigned a root junction.
    // Moreover, each connected component in the road network is assigned a root junction.
    // To generate a candidate destination for the given source junction, take the following steps:
    // (1) Find the root R of the given source junction.
    // (2) Find the size N of the connected component for root R.
    // (3) Generate a random integer j between 0 and N-1.
    // (4) The candidate destination is the j-th junction in the list of connected component for root R.

    int dest;
    auto root = disjoint_set.find(source);
    auto component_size = static_cast<int>(components[root].size());
    while (true) {

        auto i = get_random_integer(0, component_size);
        dest = components[root][i];
        if (source == dest) {
            // When source and dest refer to the same junction, generate a new pair of junctions.
            continue;
        }

        int next_junction = plan_route(road_map, source, dest);
        if (next_junction == -1) {
            // When there is no path from source to dest, generate a new pair of junctions.
            continue;
        }

        break;
    }

    return dest;
}

/**
 * Extract vehicle objects out of message and add them to the list of vehicles waiting in current junction.
 * If current junction is the vehicle's destination, remove them from simulation and notify factory actor.
 */
void actor::JunctionAndRoads::process_vehicles(mail::Message &message) {

    auto count = message.count;
    auto *data = (payload::Vehicle *) message.data;

    payload::Vehicle vehicle{};
    for (int i = 0; i < count; i++) {

        // Copy vehicle data
        std::memcpy(&vehicle, &data[i], sizeof(payload::Vehicle));

        if (vehicle.dest_id == junction.id) {
            // Current junction is the vehicle's destination
            periodic_summary.delivered_passengers += vehicle.passengers;
            send_statistics_to_factory(1);
        } else {
            // Add vehicle to list of vehicles waiting in current junction
            vehicle.on_junction = true;
            vehicle.current_road = NULL;
            vehicle.start_time = vehicle.start_time <= 0 ? Timer::get_current_seconds() : vehicle.start_time;
            vehicles[vehicle.id] = vehicle;
            junction.current_number_vehicles++;
        }
    }

    // Keep track of total number vehicles that visited the current junction
    junction.summary.total_number_vehicles += count;
}

/**
 * For every simulated minute, switch the active road at the traffic light (if present).
 */
void actor::JunctionAndRoads::switch_enabled_road_at_traffic_light() {

    if (timer.update_simulation_minutes()) {
        if (junction.has_traffic_lights && !roads.empty()) {
            auto current_road = junction.road_enabled_at_traffic_lights;
            auto number_of_roads = static_cast<int>(roads.size());
            junction.road_enabled_at_traffic_lights = (current_road + 1) % number_of_roads;
        }
    }
}

/**
 * Remove vehicle from simulation due to empty fuel tank.
 */
void actor::JunctionAndRoads::remove_vehicle_due_to_fuel_exhaustion(int i) {

    if (vehicles[i].on_junction) {
        junction.current_number_vehicles--;
    } else if (vehicles[i].current_road != NULL) {
        vehicles[i].current_road->current_number_vehicles--;
        vehicles[i].current_road->current_speed = compute_road_speed(vehicles[i].current_road);
    }

    // Update statistics to be sent to summary actor
    periodic_summary.stranded_passengers += vehicles[i].passengers;
    periodic_summary.exhausted_vehicles++;
}

/**
 * Compute the road speed base on the current number of vehicles on road.
 */
int actor::JunctionAndRoads::compute_road_speed(data::Road *road) {
    return std::max(10, road->max_speed - road->current_number_vehicles);
}

/**
 * Finds the road index out of the junction's roads that leads to a specific destination junction.
 **/
int actor::JunctionAndRoads::find_appropriate_road(int dest_junction) {
    for (int i = 0; i < roads.size(); i++) {
        if (roads[i].dest_id == dest_junction) {
            return i;
        }
    }
    return -1;
}

/**
 * Assign vehicle i an outgoing road by planning an optimal route to its destination.
 */
void actor::JunctionAndRoads::assign_road_to_vehicle(int i) {

    // Determine next junction
    int next_junction_id = plan_route(road_map, junction.id, vehicles[i].dest_id);
    assert(next_junction_id != -1);

    // Determine road to junction
    auto road_index = find_appropriate_road(next_junction_id);
    assert(road_index != -1);

    vehicles[i].current_road = &roads[road_index];
}

/**
 * Verify if vehicle i can exit the current junction with traffic lights.
 */
bool actor::JunctionAndRoads::vehicle_exits_junction_with_traffic_lights(int i) {

    if (vehicles[i].current_road->id == junction.road_enabled_at_traffic_lights) {
        junction.current_number_vehicles--;
        return true;
    } else {
        return false;
    }
}

/**
 * Verify if vehicle i can exit the current junction without traffic light or crash.
 */
bool actor::JunctionAndRoads::vehicle_exits_junction_without_traffic_lights(int i) {

    // Consider possibility of vehicle crashing while exiting junction
    int collision = get_random_integer(0, 8) * junction.current_number_vehicles;
    if (collision > 40) {
        junction.summary.total_number_crashes++;
        periodic_summary.crashed_vehicles++;
        periodic_summary.stranded_passengers += vehicles[i].passengers;
        return false;
    }

    junction.current_number_vehicles--;
    return true;
}

/**
 * Update vehicle i when entering its designated outgoing road.
 */
void actor::JunctionAndRoads::vehicle_enters_road(int i) {

    // Update vehicle
    vehicles[i].on_junction = false;
    vehicles[i].remaining_distance = vehicles[i].current_road->road_length;
    vehicles[i].speed = std::min(vehicles[i].max_speed, vehicles[i].current_road->current_speed);
    vehicles[i].last_distance_check_secs = Timer::get_current_seconds();

    // Update road
    vehicles[i].current_road->current_number_vehicles++;
    vehicles[i].current_road->summary.total_number_vehicles++;
    vehicles[i].current_road->summary.peak_number_vehicles = std::max(
            vehicles[i].current_road->summary.peak_number_vehicles,
            vehicles[i].current_road->current_number_vehicles);
    vehicles[i].current_road->current_speed = compute_road_speed(vehicles[i].current_road);
}

/**
 * Update the remaining distance vehicle i needs to travel on the road.
 */
void actor::JunctionAndRoads::move_vehicle_along_road(int i) {

    auto current_seconds = Timer::get_current_seconds();
    auto delta_seconds = current_seconds - vehicles[i].last_distance_check_secs;
    auto delta_length = delta_seconds * vehicles[i].speed;

    vehicles[i].remaining_distance -= delta_length;
    vehicles[i].last_distance_check_secs = current_seconds;
}

/**
 * When vehicle i reached the end of the road, forward it to the next junction.
 */
void actor::JunctionAndRoads::vehicle_exits_road(int i) {

    mail::Message message{};
    message.data = &vehicles[i];
    message.count = 1;
    message.mpi_datatype = MPI_VEHICLE;
    this->mailbox.send(message, vehicles[i].current_road->dest_id);

    vehicles[i].current_road->current_number_vehicles--;
    vehicles[i].current_road->current_speed = compute_road_speed(vehicles[i].current_road);
    junction.current_number_vehicles--;
}

/**
 * Send number of vehicles that have exited the simulation to the factory actor.
 */
void actor::JunctionAndRoads::send_statistics_to_factory(int number_vehicles) {

    if (number_vehicles > 0) {
        mail::Message message{};
        message.data = &number_vehicles;
        message.count = 1;
        message.mpi_datatype = MPI_INT;
        this->mailbox.send(message, factory_id);
    }
}

/**
 * Send vehicle statistics to summary actor.
 */
void actor::JunctionAndRoads::send_statistics_to_summary(int delivered, int stranded, int crashed, int exhausted) {

    if (delivered + stranded + crashed + exhausted != 0) {

        auto summary = payload::PeriodicSummary(delivered, stranded, crashed, exhausted, 0);
        mail::Message message{};
        message.data = &summary;
        message.count = 1;
        message.mpi_datatype = MPI_PERIODIC_SUMMARY;
        mailbox.send(message, summary_id);
    }
}

/**
 * Send vehicle statistics to factory and summary actor.
 */
void actor::JunctionAndRoads::send_statistics(payload::PeriodicSummary &summary) {

    auto vehicles_not_reached_destination = summary.crashed_vehicles + summary.exhausted_vehicles;
    send_statistics_to_factory(vehicles_not_reached_destination);
    send_statistics_to_summary(
            summary.delivered_passengers,
            summary.stranded_passengers,
            summary.crashed_vehicles,
            summary.exhausted_vehicles);
}

/**
 * Send detailed summaries of simulation to the summary actor.
 * This method is meant to be called after receiving the termination message.
 */
void actor::JunctionAndRoads::send_final_summaries() {

    // Send junction summary
    mail::Message message;
    message.count = 1;
    message.data = &junction.summary;
    message.mpi_datatype = MPI_JUNCTION_SUMMARY;
    mailbox.send(message, summary_id);

    // Send road summaries
    std::vector<payload::RoadSummary> road_summaries(roads.size());
    for (int i = 0; i < road_summaries.size(); i++) {
        road_summaries[i] = roads[i].summary;
    }

    message.count = road_summaries.size();
    message.data = road_summaries.data();
    message.mpi_datatype = MPI_ROAD_SUMMARY;
    mailbox.send(message, summary_id);
}
