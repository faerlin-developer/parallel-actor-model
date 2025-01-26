#ifndef LOAD_H
#define LOAD_H

#include <vector>
#include <string>
#include "map/graph.h"

namespace map {

    /**
     * Metadata of road network configuration.
     */
    struct RoadMapInfo {

        std::string filename;           // Filename for road network configuration
        int road_length_scale_down;     // Scale down factor for all road lengths in the provided road network
        int road_length_minimum;        // Minimum road length when scaling down.
        int fuel_scale_up;              // Scale up factor for the min and max fuel capacities of all vehicle types

        // NOTE: To use the default road lengths and fuel capacities, use the following settings:
        //
        // road_length_scale_down = 1
        // road_length_minimum = 0
        // fuel_scale_up = 1

        RoadMapInfo(std::string filename, int road_length_scale_down, int road_length_minimum, int fuel_scale_up);
    };

    bool load(RoadMapInfo &road_map_info, graph::RoadMap &road_map);

    void get_num_junctions_and_roads(RoadMapInfo &road_map_info, int &num_junctions, int &num_roads);
}

#endif
