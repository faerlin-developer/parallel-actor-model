#ifndef PAYLOAD_SUMMARY_H
#define PAYLOAD_SUMMARY_H

namespace payload {

    /**
     * Vehicle statistics periodically sent to summary actor (corresponds to MPI_PERIODIC_SUMMARY).
     */
    struct PeriodicSummary {

        int delivered_passengers;
        int stranded_passengers;
        int crashed_vehicles;
        int exhausted_vehicles;
        int total_vehicles;

        PeriodicSummary();

        PeriodicSummary(int delivered, int stranded, int crashed, int exhausted, int total_vehicles);
    };

    /**
     * Detailed junction summary sent to summary actor at the end of simulation (corresponds to MPI_JUNCTION_SUMMARY).
     */
    struct JunctionSummary {
        int id;
        int total_number_vehicles;           // Total number of vehicles that visited this junction
        int total_number_crashes;            // Total number of crashes that happened

        JunctionSummary();

        explicit JunctionSummary(int id);
    };

    /**
  */

    /**
     * Detailed road summary sent to summary actor at the end of simulation (corresponds to MPI_ROAD_SUMMARY).
     */
    struct RoadSummary {
        int source_id;
        int dest_id;
        int total_number_vehicles;    // Total number of vehicles visited this road
        int peak_number_vehicles;     // Peak number of concurrent vehicles travelled on this road

        RoadSummary();

        RoadSummary(int source_id, int dest_id);
    };
}

#endif
