#ifndef JUNCTION_H
#define JUNCTION_H

#include <string>
#include "actor/actor.h"
#include "actor/types.h"
#include "actors/junction_and_roads.h"
#include "mail/mailbox.h"
#include "map/data.h"
#include "map/graph.h"
#include "payload/vehicle.h"
#include "payload/summary.h"
#include "util/timer.h"
#include "util/disjoint_set.h"
#include "map/load.h"

namespace actor {

    /**
     *
     */
    class JunctionAndRoads : public actor::Actor {
    public:

        // Constructor parameters
        int factory_id;                   // ID of factory actor
        int summary_id;                   // ID of summary actor
        int initial_vehicle_id;           // Starting ID of vehicles to be created at initialization
        int initial_vehicle_size;         // Number of vehicles to be created at initialization
        map::RoadMapInfo road_map_info;   // Metadata on road network configuration

        graph::RoadMap road_map;                    // Road network
        data::Junction junction;                    // Current junction
        data::Roads roads;                          // Outgoing roads of current junction
        payload::Vehicles vehicles;                 // Vehicles waiting on this junction or on one of its roads
        payload::PeriodicSummary periodic_summary;  // Data to be sent to summary actor periodically
        Timer timer;                                // timer for computing simulated minutes

    public:

        JunctionAndRoads(actor::id id, int factory_id, int summary_id, int initial_vehicle_id, int initial_vehicle_size,
                         map::RoadMapInfo &road_map_info);

        bool pre_barrier_init() override;

        bool post_barrier_init() override;

        next_step ingress(mail::Message &message) override;

        next_step run() override;

    private:

        int generate_vehicle_destination(int source, DisjointSet &disjoint_set,
                                         std::unordered_map<int, std::vector<int>> &components);

        void process_vehicles(mail::Message &message);

        void switch_enabled_road_at_traffic_light();

        void remove_vehicle_due_to_fuel_exhaustion(int i);

        int compute_road_speed(data::Road *road);

        int find_appropriate_road(int dest_junction);

        void assign_road_to_vehicle(int i);

        bool vehicle_exits_junction_with_traffic_lights(int i);

        bool vehicle_exits_junction_without_traffic_lights(int i);

        void vehicle_enters_road(int i);

        void move_vehicle_along_road(int i);

        void vehicle_exits_road(int i);

        void send_statistics_to_factory(int number_vehicles);

        void send_statistics_to_summary(int delivered, int stranded, int crashed, int exhausted);

        void send_statistics(payload::PeriodicSummary &summary);

        void send_final_summaries();
    };

}

#endif
