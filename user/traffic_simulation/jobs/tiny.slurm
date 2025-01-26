#!/bin/sh
#SBATCH --job-name=traffic_simulation_program
#SBATCH --time=00:01:10
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --tasks-per-node=4
#SBATCH --cpus-per-task=1
#SBATCH --account=m24ol-s2465760
#SBATCH --partition=standard
#SBATCH --qos=short

export OMP_NUM_THREADS=1

# Configuration = tiny
# Number of MPI processes = 4
# Number of actors per MPI process = 5

exe=build/traffic_simulation_program
data=data/tiny_problem
max_mins=30
initial_vehicles=30
max_vehicles=150
num_actors_per_procs=5
road_length_scale_down=1
road_length_minimum=0
fuel_scale_up=1

srun ./${exe} ${data} ${max_mins} ${initial_vehicles} ${max_vehicles} ${num_actors_per_procs} \
              ${road_length_scale_down} ${road_length_minimum} ${fuel_scale_up}
