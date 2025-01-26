#ifndef FACTORY_H
#define FACTORY_H

#include <unordered_map>
#include <vector>
#include "actor/actor.h"
#include "util/timer.h"
#include "map/graph.h"
#include "payload/vehicle.h"
#include "util/disjoint_set.h"
#include "map/load.h"

namespace actor {

    /**
     * The factory actor periodically adds new vehicles to the simulation.
     */
    class Factory : public actor::Actor {
    public:

        // Constructor parameters
        int summary_id;                      // ID of summary actor
        int initial_number_vehicles;         // Number of vehicles at the start of simulation
        int max_vehicles;                    // Maximum number of active vehicles
        map::RoadMapInfo road_map_info;      // Metadata on road network configuration

        graph::RoadMap road_map;
        int current_number_vehicles;         // Current number of active vehicles
        int total_number_vehicles;           // Total count of vehicles that have taken part in the simulation.
        DisjointSet disjoint_set;            // Used to efficiently generate valid source and destination junctions
        std::unordered_map<int, std::vector<int>> components;
        Timer timer;                         // Timer for computing simulated minutes

    public:

        Factory(actor::id id, int summary_id, int initial_number_vehicles, int max_vehicles,
                map::RoadMapInfo &road_map_info);

        bool pre_barrier_init() override;

        bool post_barrier_init() override;

        next_step ingress(mail::Message &message) override;

        next_step run() override;

    private:

        void generate_vehicles(std::unordered_map<int, std::vector<payload::Vehicle>> &vehicles_by_junction,
                               int number_vehicles);

        int generate_random_destination(int source);

        void assign_source_and_destination(int &source, int &dest);

        void send_vehicles(std::unordered_map<int, std::vector<payload::Vehicle>> &vehicles_by_junction);

        void send_statistics_to_summary(int number_vehicles);
    };
}

#endif
