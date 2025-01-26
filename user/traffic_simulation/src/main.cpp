#include <string>
#include <cstdio>
#include "mpi.h"
#include "actors/junction_and_roads.h"
#include "actors/summary.h"
#include "actors/factory.h"
#include "map/load.h"
#include "map/data.h"
#include "util/random.h"
#include "constants/constants.h"
#include "payload/datatype.h"
#include "actor/framework.h"
#include "payload/terminate.h"
#include "payload/vehicle.h"
#include "payload/summary.h"
#include "main.h"

/**
 * PARAMETERS
 *
 * The parameters for the main method, in order, are as follows:
 * (1) Filename of road network configuration
 * (2) Maximum simulation minutes
 * (3) Number of initial vehicles
 * (4) Maximum number of active vehicles
 * (5) Number of actors per MPI process
 * (6) Road length scale down factor
 * (7) Road length minimum
 * (8) Vehicle fuel scale up factor
 *
 * To use the default values of road length and vehicle fuel capacities, set the following parameters as follows:
 * - Road length scale down factor to 1
 * - Road length minimum to 0
 * - Vehicle fuel scale up factor to 1
 */
int main(int argc, char *argv[]) {

    if (argc != 9) {
        fprintf(stderr, "ERROR: expected 8 arguments\n");
        return EXIT_FAILURE;
    }

    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    set_random_seed(RANDOM_SEED);

    // Get command line arguments
    int num_junctions, num_roads;
    std::string road_map_file = argv[1];
    int max_mins = std::stoi(argv[2]);
    int initial_vehicles = std::stoi(argv[3]);
    int max_vehicles = std::stoi(argv[4]);
    int num_actors_per_procs = std::stoi(argv[5]);
    int road_length_scale_down = std::stoi(argv[6]);
    int road_length_minimum = std::stoi(argv[7]);
    int fuel_scale_up = std::stoi(argv[8]);
    auto road_map_info = map::RoadMapInfo(road_map_file, road_length_scale_down, road_length_minimum, fuel_scale_up);
    map::get_num_junctions_and_roads(road_map_info, num_junctions, num_roads);

    // Create actor model framework
    auto ingress_mode = true;
    auto log_debug = LOG_DEBUG ? true : false;
    auto framework = ParallelActorModel(num_actors_per_procs, ingress_mode, log_debug);

    // Setup framework (i.e. add actors and message data type)
    add_junction_actors(framework, num_junctions, initial_vehicles, road_map_info);
    add_factory_actor(framework, num_junctions, initial_vehicles, max_vehicles, road_map_info);
    add_summary_actor(framework, num_junctions, initial_vehicles, max_mins);
    add_message_datatype(framework);
    print_problem_size(framework, num_junctions, num_roads, initial_vehicles);

    // Launch framework execution cycle
    framework.start();

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    print_execution_time(framework, log_debug, start_time, end_time);
    MPI_Finalize();

    return EXIT_SUCCESS;
}

/**
 * Create and add junction actors to the framework.
 */
void add_junction_actors(ParallelActorModel &framework, int num_junctions, int initial_vehicles,
                         map::RoadMapInfo road_map_info) {

    auto factory_id = num_junctions;
    auto summary_id = num_junctions + 1;

    // Assign random number of initial vehicles to each junction
    auto initial_vehicles_by_junction = std::unordered_map<int, int>();

    for (int junction_id = 0; junction_id < num_junctions; junction_id++) {
        initial_vehicles_by_junction[junction_id] = 0;
    }

    for (int i = 0; i < initial_vehicles; i++) {
        auto junction_id = get_random_integer(0, num_junctions);
        initial_vehicles_by_junction[junction_id]++;
    }

    // In the simulation, each vehicle is assigned a unique id.
    // This rule applies to both the initial vehicles and those generated during the simulation.
    // At initialization, each junction actor will create an assigned number of initial vehicles.
    // To ensure vehicle creation without conflicting IDs, each junction actor is assigned a unique starting vehicle ID.

    int initial_vehicle_id = 0;
    for (int actor_id = 0; actor_id < num_junctions; actor_id++) {

        auto initial_vehicle_size = initial_vehicles_by_junction[actor_id];
        auto actor = new actor::JunctionAndRoads(
                actor_id,
                factory_id,
                summary_id,
                initial_vehicle_id,
                initial_vehicle_size,
                road_map_info);

        framework.addActor(actor);
        initial_vehicle_id += initial_vehicle_size;
    }
}

/**
 * Create and add factory actor to the framework.
 * The factory actor periodically adds new vehicles to the simulation.
 */
void add_factory_actor(ParallelActorModel &framework, int num_junctions, int initial_vehicles, int max_vehicles,
                       map::RoadMapInfo road_map_info) {
    auto factory_id = num_junctions;
    auto summary_id = num_junctions + 1;
    auto factory_actor = new actor::Factory(factory_id, summary_id, initial_vehicles, max_vehicles, road_map_info);
    framework.addIsolatedActor(factory_actor);
}

/**
 * Create and add summary actor to the framework.
 * The summary actor periodically prints a summary of the simulation.
 */
void add_summary_actor(ParallelActorModel &framework, int num_junctions, int initial_vehicles, int max_mins) {
    auto factory_id = num_junctions;
    auto summary_id = num_junctions + 1;
    auto summary_actor = new actor::Summary(summary_id, factory_id, num_junctions, initial_vehicles, max_mins);
    framework.addIsolatedActor(summary_actor);
}

/**
 * Create and add the data type of messages sent and received by actors.
 */
void add_message_datatype(ParallelActorModel &framework) {

    // Create the MPI_Datatype used by our actors
    MPI_Create_vehicle_datatype();
    MPI_Create_terminate_datatype();
    MPI_Create_periodic_summary_datatype();
    MPI_Create_junction_summary_datatype();
    MPI_Create_road_summary_datatype();

    // Register datatypes to framework
    framework.addType(mail::Type{sizeof(int), MPI_INT});
    framework.addType(mail::Type{sizeof(payload::Vehicle), MPI_VEHICLE});
    framework.addType(mail::Type{sizeof(payload::Terminate), MPI_TERMINATE});
    framework.addType(mail::Type{sizeof(payload::PeriodicSummary), MPI_PERIODIC_SUMMARY});
    framework.addType(mail::Type{sizeof(payload::JunctionSummary), MPI_JUNCTION_SUMMARY});
    framework.addType(mail::Type{sizeof(payload::RoadSummary), MPI_ROAD_SUMMARY});
}

/**
 * Display the problem size.
 */
void print_problem_size(ParallelActorModel &framework, int num_junctions, int num_roads, int initial_vehicles) {
    if (framework.rank == 0) {
        printf("Starting simulation with %d junctions, %d roads and %d vehicles\n",
               num_junctions, num_roads, initial_vehicles);
        fflush(stdout);
    }
}

/**
 * Prints the execution time of the simulation when the log leve is set to debug.
 */
void print_execution_time(ParallelActorModel &framework, int log_debug, double start_time, double end_time) {
    if (framework.rank == 0 && log_debug) {
        printf("[DEBUG] Execution time is %f\n", end_time - start_time);
        fflush(stdout);
    }
}
