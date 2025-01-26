#ifndef SUMMARY_H
#define SUMMARY_H

#include "actor/actor.h"
#include "payload/summary.h"
#include "util/timer.h"

namespace actor {

    /**
     * The summary actor periodically prints a summary of the simulation to stdout.
     */
    class Summary : public actor::Actor {
    public:

        // Constructor parameters
        int factory_id;                     // ID of factory actor
        int num_junctions;                  // Number of junctions in road network
        int initial_vehicles;               // Number of initial vehicles
        int max_mins;                       // The maximum simulated minutes

        int total_vehicles;                 // Total number of vehicles modelled
        int delivered_passengers;           // Number of passengers who reached their destinations
        int stranded_passengers;            // Number of stranded passengers
        int crashed_vehicles;               // Number of vehicles that crashed
        int exhausted_vehicles;             // Number of vehicles that ran out of fuel
        int remaining_detailed_summaries;   // Current number detailed summaries to be received
        bool termination_sent;              // True when termination message has been sent to all actors
        std::vector<payload::JunctionSummary> junction_summaries;       // Summaries for all junction
        std::vector<std::vector<payload::RoadSummary>> road_summaries;  // Summaries for all road
        Timer timer;                        // Timer for simulated minutes

    public:
        Summary(actor::id id, int factory_id, int num_junctions, int initial_vehicles, int max_mins);

        bool pre_barrier_init() override;

        bool post_barrier_init() override;

        next_step ingress(mail::Message &message) override;

        next_step run() override;

    private:

        void send_terminate();

        void write_detailed_info();
    };
}

#endif
