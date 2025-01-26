#ifndef MAIN_H
#define MAIN_H

void add_junction_actors(ParallelActorModel &framework, int num_junctions, int initial_vehicles,
                         map::RoadMapInfo road_map_info);

void add_factory_actor(ParallelActorModel &framework, int num_junctions, int initial_vehicles, int max_vehicles,
                       map::RoadMapInfo road_map_info);

void add_summary_actor(ParallelActorModel &framework, int num_junctions, int initial_vehicles, int max_mins);

void add_message_datatype(ParallelActorModel &framework);

void print_problem_size(ParallelActorModel &framework, int num_junctions, int num_roads, int initial_vehicles);

void print_execution_time(ParallelActorModel &framework, int log_debug, double start_time, double end_time);

#endif
