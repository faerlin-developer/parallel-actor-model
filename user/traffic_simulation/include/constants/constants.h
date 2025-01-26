#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <unordered_map>

#define BUFFER_SIZE 1024*1024*10

#define VERBOSE_ROUTE_PLANNER 0
#define LARGE_NUM 99999999.0

#define MAX_ROAD_LEN 100
#define MIN_LENGTH_SECONDS 2
#define MAX_NUM_ROADS_PER_JUNCTION 50
#define SUMMARY_FREQUENCY 2

#define BUS_PASSENGERS 80
#define BUS_MAX_SPEED 50
#define BUS_MIN_FUEL 10
#define BUS_MAX_FUEL 100
#define CAR_PASSENGERS 4
#define CAR_MAX_SPEED 100
#define CAR_MIN_FUEL 1
#define CAR_MAX_FUEL 40
#define MINI_BUS_MAX_SPEED 80
#define MINI_BUS_PASSENGERS 15
#define MINI_BUS_MIN_FUEL 2
#define MINI_BUS_MAX_FUEL 75
#define COACH_MAX_SPEED 60
#define COACH_PASSENGERS 40
#define COACH_MIN_FUEL 20
#define COACH_MAX_FUEL 200
#define MOTOR_BIKE_MAX_SPEED 120
#define MOTOR_BIKE_PASSENGERS 2
#define MOTOR_BIKE_MIN_FUEL 1
#define MOTOR_BIKE_MAX_FUEL 20
#define BIKE_MAX_SPEED 10
#define BIKE_PASSENGERS 1
#define BIKE_MIN_FUEL 2
#define BIKE_MAX_FUEL 10

#define RANDOM_SEED 42
#define MIN_NEW_VEHICLES 100
#define MAX_NEW_VEHICLES 200

#define LOG_DEBUG 0

enum ReadMode {
    NONE = 0,
    ROADMAP,
    TRAFFICLIGHTS
};

enum VehicleType {
    CAR = 0,
    BUS,
    MINI_BUS,
    COACH,
    MOTORBIKE,
    BIKE
};

struct VehicleAttributes {
    int max_speed;
    int max_passengers;
    int min_fuel;
    int max_fuel;
};

extern std::unordered_map<VehicleType, VehicleAttributes> vehicle_attrs;

#endif
