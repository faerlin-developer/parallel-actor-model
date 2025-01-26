#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include "map/data.h"

namespace node {

    /**
     * A road component of graph::RoadMap.
     */
    struct Road {
        int max_speed;
        int road_length;
        struct Junction *dest;
    };

    /**
     * A junction component of graph::RoadMap.
     */
    struct Junction {
        int id;
        bool has_traffic_lights;
        std::vector<Road> roads;
    };
}

namespace graph {

    /**
     * The road network loaded from a user provided file.
     */
    typedef std::vector<node::Junction> RoadMap;
}

#endif
