#include "payload/vehicle.h"
#include "constants/constants.h"
#include "util/random.h"
#include "util/timer.h"

namespace payload {

    Vehicle::Vehicle() = default;

    Vehicle::Vehicle(int id, int fuel, int max_speed, int passengers, int source_id, int dest_id) :
            id(id), fuel(fuel), max_speed(max_speed), passengers(passengers), source_id(source_id), dest_id(dest_id) {
        start_time = -1;
        speed = 0;
        last_distance_check_secs = 0;
        on_junction = false;
        current_road = NULL;
        remaining_distance = 0;
    }

    Vehicle::Vehicle(int id, VehicleType type, map::RoadMapInfo &road_map_info) {

        auto attrs = vehicle_attrs[static_cast<VehicleType>(type)];

        this->id = id;
        fuel = get_random_integer(road_map_info.fuel_scale_up * attrs.min_fuel,
                                  road_map_info.fuel_scale_up * attrs.max_fuel + 1);;
        max_speed = attrs.max_speed;
        passengers = get_random_integer(1, attrs.max_passengers + 1);
        start_time = -1;
        speed = 0;
        last_distance_check_secs = 0;
        on_junction = false;
        current_road = NULL;
        remaining_distance = 0;
    }
}