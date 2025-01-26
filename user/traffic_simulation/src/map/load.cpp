#include <cstring>
#include <limits>
#include "map/load.h"
#include "map/graph.h"
#include "constants/constants.h"
#include "util/disjoint_set.h"

map::RoadMapInfo::RoadMapInfo(std::string filename, int road_length_scale_down,
                              int road_length_minimum, int fuel_scale_up) :
        filename(filename), road_length_scale_down(road_length_scale_down),
        road_length_minimum(road_length_minimum), fuel_scale_up(fuel_scale_up) {}


/**
 * Load the road network from the file in road_map_info.
 */
bool map::load(RoadMapInfo &road_map_info, graph::RoadMap &road_map) {

    auto filename = road_map_info.filename;
    FILE *f = fopen(filename.c_str(), "r");
    if (f == nullptr) {
        fprintf(stderr, "Error opening road map file %s\n", filename.c_str());
        fflush(stderr);
        return false;
    }

    // Scale the road lengths down by the factor provided in road_map_info
    auto road_length_scale_down = road_map_info.road_length_scale_down;
    auto road_length_minimum = road_map_info.road_length_minimum > 0
                               ? road_map_info.road_length_minimum : std::numeric_limits<int>::max();

    enum ReadMode currentMode = NONE;
    char buffer[MAX_ROAD_LEN];
    while (fgets(buffer, MAX_ROAD_LEN, f)) {
        if (buffer[0] == '%') continue;
        if (buffer[0] == '#') {
            if (strncmp("# Road layout:", buffer, 14) == 0) {
                char *s = strstr(buffer, ":");
                auto num_junctions = atoi(&s[1]);
                road_map.resize(num_junctions);

                for (int i = 0; i < num_junctions; i++) {
                    road_map[i].id = i;
                    road_map[i].has_traffic_lights = false;
                    road_map[i].roads = std::vector<node::Road>();
                }
                currentMode = ROADMAP;
            }
            if (strncmp("# Traffic lights:", buffer, 17) == 0) {
                currentMode = TRAFFICLIGHTS;
            }
        } else {
            if (currentMode == ROADMAP) {
                char *space = strstr(buffer, " ");
                *space = '\0';
                int from_id = atoi(buffer);
                char *nextspace = strstr(&space[1], " ");
                *nextspace = '\0';
                int to_id = atoi(&space[1]);
                char *nextspace2 = strstr(&nextspace[1], " ");
                *nextspace = '\0';
                int roadlength = atoi(&nextspace[1]);
                int speed = atoi(&nextspace2[1]);
                if (road_map[from_id].roads.size() >= MAX_NUM_ROADS_PER_JUNCTION) {
                    fprintf(stderr,
                            "Error: Tried to create road %ld at junction %d, but maximum number of roads is %d, increase 'MAX_NUM_ROADS_PER_JUNCTION'",
                            road_map[from_id].roads.size(), from_id, MAX_NUM_ROADS_PER_JUNCTION);
                    fflush(stderr);
                    return false;
                }

                node::Road road{};
                road.road_length = std::min(road_length_minimum, roadlength / road_length_scale_down);
                road.max_speed = speed;
                road.dest = &road_map[to_id];
                road_map[from_id].roads.push_back(road);

            } else if (currentMode == TRAFFICLIGHTS) {
                int id = atoi(buffer);
                if (road_map[id].roads.size() > 0) {
                    road_map[id].has_traffic_lights = true;
                }
            }
        }
    }
    fclose(f);

    return true;
}

/**
 * Get the number of junctions and roads of the road network specified in road_map_info.
 */
void map::get_num_junctions_and_roads(RoadMapInfo &road_map_info, int &num_junctions, int &num_roads) {

    graph::RoadMap road_map;
    load(road_map_info, road_map);

    num_roads = 0;
    num_junctions = static_cast<int>(road_map.size());
    for (int i = 0; i < num_junctions; i++) {
        num_roads += road_map[i].roads.size();
    }
}