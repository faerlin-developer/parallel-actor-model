#ifndef VEHICLE_H
#define VEHICLE_H

#include <unordered_map>
#include "constants/constants.h"
#include "map/load.h"

namespace payload {

    /**
     * Vehicle object sent between junction actors (corresponds to MPI_VEHICLE).
     */
    struct Vehicle {

        // Fixed Properties
        int id;
        int fuel;                            // initial fuel
        int max_speed;                       // maximum speed of vehicle
        int passengers;                      // number of passengers on vehicle
        int source_id;                       // ID of source junction
        int dest_id;                         // ID of destination junction
        int start_time;                      // activation time (number of seconds since Unix epoch)

        // Variable Properties
        int speed;                           // current speed
        int last_distance_check_secs;        // time at last distance check (number of seconds since Unix epoch)
        bool on_junction;                    // True when vehicle is on a junction
        data::Road *current_road;           // current road vehicle is on
        double remaining_distance;           // Remaining distance to travel

        Vehicle();

        Vehicle(int id, int fuel, int max_speed, int passengers, int source_id, int dest_id);

        Vehicle(int id, VehicleType type, map::RoadMapInfo &road_map_info);
    };

    typedef std::unordered_map<int, Vehicle> Vehicles;
}

#endif
