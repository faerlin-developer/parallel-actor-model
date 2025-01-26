
#include <unordered_map>
#include "constants/constants.h"

std::unordered_map<VehicleType, VehicleAttributes> vehicle_attrs = {
        {CAR,       {CAR_MAX_SPEED,        CAR_PASSENGERS,        CAR_MIN_FUEL,        CAR_MAX_FUEL}},
        {BUS,       {BUS_MAX_SPEED,        BUS_PASSENGERS,        BUS_MIN_FUEL,        BUS_MAX_FUEL}},
        {MINI_BUS,  {MINI_BUS_MAX_SPEED,   MINI_BUS_PASSENGERS,   MINI_BUS_MIN_FUEL,   MINI_BUS_MAX_FUEL}},
        {COACH,     {COACH_MAX_SPEED,      COACH_PASSENGERS,      COACH_MIN_FUEL,      COACH_MAX_FUEL}},
        {MOTORBIKE, {MOTOR_BIKE_MAX_SPEED, MOTOR_BIKE_PASSENGERS, MOTOR_BIKE_MIN_FUEL, MOTOR_BIKE_MAX_FUEL}},
        {BIKE,      {BIKE_MAX_SPEED,       BIKE_PASSENGERS,       BIKE_MIN_FUEL,       BIKE_MAX_FUEL}}
};