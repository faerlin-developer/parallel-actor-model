#ifndef DATA_H
#define DATA_H

#include <vector>
#include "payload/summary.h"

namespace data {

    /**
     * A road in the network.
     * One of the internal state variables of a junction actor.
     */
    struct Road {

        // Fixed properties
        int id;
        int road_length;              // Length of road
        int max_speed;                // Maximum speed supported by road
        int source_id;                // source junction
        int dest_id;                  // destination junction

        // Variable properties
        int current_speed;            // Current speed on the road (function of number of vehicles on road)
        int current_number_vehicles;  // Current number of vehicles on the road
        payload::RoadSummary summary; // Statistics sent to summary actor

        Road();

        Road(int id, int source_id, int dest_id, int road_length, int max_speed);
    };

    /**
     * A junction in the network.
     * One of the internal state variables of a junction actor.
     */
    struct Junction {

        // Fixed properties
        int id;
        bool has_traffic_lights;             // Does junction have traffic lights?

        // Variable properties
        int current_number_vehicles;         // Current number of vehicles waiting on junction
        int road_enabled_at_traffic_lights;  // ID of road currently enabled at traffic lights
        payload::JunctionSummary summary;    // Statistics sent to summary actor

        Junction();

        Junction(int id, bool has_traffic_lights);
    };

    typedef std::vector<data::Road> Roads;
}

#endif
