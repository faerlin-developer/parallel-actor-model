
#include "payload/summary.h"


payload::JunctionSummary::JunctionSummary() = default;

payload::RoadSummary::RoadSummary() = default;

payload::JunctionSummary::JunctionSummary(int id) : id(id), total_number_vehicles(0), total_number_crashes(0) {}

payload::RoadSummary::RoadSummary(int source_id, int dest_id) :
        source_id(source_id), dest_id(dest_id), total_number_vehicles(0), peak_number_vehicles(0) {}

payload::PeriodicSummary::PeriodicSummary() :
        delivered_passengers(0),
        stranded_passengers(0),
        crashed_vehicles(0),
        exhausted_vehicles(0),
        total_vehicles(0) {}

payload::PeriodicSummary::PeriodicSummary(int delivered, int stranded, int crashed, int exhausted, int total_vehicles) :
        delivered_passengers(delivered),
        stranded_passengers(stranded),
        crashed_vehicles(crashed),
        exhausted_vehicles(exhausted),
        total_vehicles(total_vehicles) {}
