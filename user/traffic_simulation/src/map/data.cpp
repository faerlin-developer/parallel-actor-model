#include <cstdlib>
#include "map/data.h"
#include "payload/summary.h"

data::Road::Road() = default;

data::Junction::Junction() = default;

data::Road::Road(int id, int source_id, int dest_id, int road_length, int max_speed) :
        id(id), road_length(road_length), max_speed(max_speed), source_id(source_id), dest_id(dest_id) {
    current_speed = max_speed;
    current_number_vehicles = 0;
    summary = payload::RoadSummary(source_id, dest_id);
}

data::Junction::Junction(int id, bool has_traffic_lights) : id(id), has_traffic_lights(has_traffic_lights) {
    current_number_vehicles = 0;
    road_enabled_at_traffic_lights = 0;
    summary = payload::JunctionSummary(id);
}
