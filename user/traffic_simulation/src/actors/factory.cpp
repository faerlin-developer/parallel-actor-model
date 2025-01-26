#include <unordered_map>
#include <vector>
#include "actors/factory.h"
#include "payload/vehicle.h"
#include "map/load.h"
#include "map/search.h"
#include "constants/constants.h"
#include "util/random.h"
#include "util/disjoint_set.h"
#include "mail/message.h"
#include "payload/datatype.h"

actor::Factory::Factory(actor::id id, int summary_id, int initial_number_vehicles,
                        int max_vehicles, map::RoadMapInfo &road_map_info) :
        Actor(id),
        summary_id(summary_id),
        initial_number_vehicles(initial_number_vehicles),
        max_vehicles(max_vehicles),
        road_map_info(road_map_info),
        current_number_vehicles(0),
        total_number_vehicles(0) {}

/**
 * Initialize factory actor.
 */
bool actor::Factory::pre_barrier_init() {

    // Load road map from file
    auto success = map::load(road_map_info, road_map);
    if (!success) {
        fprintf(stderr, "failed to load %s\n", road_map_info.filename.c_str());
        return false;
    }

    // Create disjoint set
    auto num_junctions = static_cast<int>(road_map.size());
    disjoint_set = DisjointSet(num_junctions);
    for (int source_id = 0; source_id < num_junctions; source_id++) {
        for (const auto &road: road_map[source_id].roads) {
            disjoint_set.connect(source_id, road.dest->id);
        }
    }

    // Create connected components
    components = disjoint_set.get_connected_components();

    // Initialize vehicle statistics
    current_number_vehicles = initial_number_vehicles;
    total_number_vehicles = initial_number_vehicles;

    return true;
}

/**
 * Initialize `timer`.
 */
bool actor::Factory::post_barrier_init() {
    timer = Timer(MIN_LENGTH_SECONDS);
    return true;
}

/**
 * Handle incoming messages based on their data type:
 * (1) MPI_INT
 *     - This value signifies the number of vehicles that have exited the simulation.
 * (2) MPI_TERMINATE
 *     - Terminate this actor
 */
actor::next_step actor::Factory::ingress(mail::Message &message) {

    if (message.mpi_datatype == MPI_INT) {

        // Receive an integer from any junction.
        // This integer indicates the number of vehicles that have been removed from simulation.
        // Update current number of vehicles in simulation.
        auto number_vehicles = (int *) message.data;
        current_number_vehicles -= *number_vehicles;
        return actor::CONTINUE;

    } else if (message.mpi_datatype == MPI_TERMINATE) {
        return actor::STOP;
    } else {
        fprintf(stderr, "unexpected mpi datatype at ingress\n");
        return actor::STOP;
    }
}

/**
 * For each simulated minute, create new vehicles with valid source and destination junctions
 * and send them to their designated source junction actors. Ensure the number of new vehicles
 * generated adheres to the maximum limit for active vehicles. Lastly, send the number new
 * vehicles to the summary actor.
 */
actor::next_step actor::Factory::run() {

    if (timer.update_simulation_minutes()) {

        if (current_number_vehicles < max_vehicles) {

            // Determine number of new vehicles
            int num_new_vehicles = get_random_integer(MIN_NEW_VEHICLES, MAX_NEW_VEHICLES + 1);
            num_new_vehicles = std::min(num_new_vehicles, max_vehicles - current_number_vehicles);

            // Generate new vehicles
            auto vehicles_by_junction = std::unordered_map<int, std::vector<payload::Vehicle>>();
            generate_vehicles(vehicles_by_junction, num_new_vehicles);

            // Send vehicles to junction actors and statistics to summary actor.
            send_vehicles(vehicles_by_junction);
            send_statistics_to_summary(num_new_vehicles);

            // Update vehicle statistics
            current_number_vehicles += num_new_vehicles;
            total_number_vehicles += num_new_vehicles;
        }
    }

    return actor::CONTINUE;
}

/**
  * Populate `vehicles_by_junction` by mapping each junction ID to a list of vehicles.
  * The total vehicle count is `number_vehicles`. Note that some junction IDs may map to an empty list.
  */
void actor::Factory::generate_vehicles(std::unordered_map<int, std::vector<payload::Vehicle>> &vehicles_by_junction,
                                       int number_vehicles) {

    auto num_vehicle_types = static_cast<int>(vehicle_attrs.size());
    auto vehicle_id = total_number_vehicles;
    auto max_id = total_number_vehicles + number_vehicles;

    while (vehicle_id < max_id) {
        auto vehicle_type = static_cast<VehicleType>(get_random_integer(0, num_vehicle_types));
        auto vehicle = payload::Vehicle(vehicle_id, vehicle_type, road_map_info);
        assign_source_and_destination(vehicle.source_id, vehicle.dest_id);
        vehicles_by_junction[vehicle.source_id].push_back(vehicle);
        vehicle_id++;
    }
}

/**
 * Generate a candidate destination junction to the given source junction.
 */
int actor::Factory::generate_random_destination(int source) {

    // In the `disjoint_set` data structure, each junction is assigned a root junction.
    // Moreover, each connected component in the road network is assigned a root junction.
    // To generate a candidate destination for the given source junction, take the following steps:
    // (1) Find the root R of the given source junction.
    // (2) Find the size N of the connected component for root R.
    // (3) Generate a random integer j between 0 and N-1.
    // (4) The candidate destination is the j-th junction in the list of connected component for root R.

    auto root = disjoint_set.find(source);
    auto component_size = static_cast<int>(components[root].size());
    auto i = get_random_integer(0, component_size);
    return components[root][i];
}

/**
 * Assign valid source and destination junction IDs to the given parameters.
 * See the comments in the generate_random_destination method for details of the selection algorithm.
 */
void actor::Factory::assign_source_and_destination(int &source, int &dest) {

    auto num_junctions = static_cast<int>(road_map.size());
    while (true) {

        source = get_random_integer(0, num_junctions);
        dest = generate_random_destination(source);
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
}

/**
 * Send new vehicles to their corresponding junctions.
 */
void actor::Factory::send_vehicles(std::unordered_map<int, std::vector<payload::Vehicle>> &vehicles_by_junction) {

    for (const auto &kv: vehicles_by_junction) {

        auto i = kv.first;
        mail::Message message;
        message.count = static_cast<int>(vehicles_by_junction[i].size());
        message.data = vehicles_by_junction[i].data();
        message.mpi_datatype = MPI_VEHICLE;

        if (message.count != 0) {
            mailbox.send(message, i);
        }
    }
}

/**
 * Send the number of new vehicles to the summary actor.
 */
void actor::Factory::send_statistics_to_summary(int number_vehicles) {

    if (number_vehicles != 0) {
        auto summary = payload::PeriodicSummary(0, 0, 0, 0, number_vehicles);
        mail::Message message{};
        message.data = &summary;
        message.count = 1;
        message.mpi_datatype = MPI_PERIODIC_SUMMARY;
        mailbox.send(message, summary_id);
    }
}
