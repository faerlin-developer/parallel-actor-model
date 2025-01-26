#!/bin/sh
#SBATCH --job-name=traffic_simulation_program
#SBATCH --time=00:02:00
#SBATCH --exclusive
#SBATCH --nodes=8
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --account=m24ol-s2465760
#SBATCH --partition=standard
#SBATCH --qos=short

export OMP_NUM_THREADS=1

# Configuration = largest
# Number of MPI processes = 1002
# Number of actors per MPI process = 100
# All road lengths are scaled down by a factor of 10
# Minimum and maximum fuel capacity of all vehicle types are scaled up by a factor of 5

exe=build/traffic_simulation_program
data=data/largest_problem
max_mins=30
initial_vehicles=200
max_vehicles=2500
num_actors_per_procs=100
road_length_scale_down=10
road_length_minimum=50
fuel_scale_up=5

srun ./${exe} ${data} ${max_mins} ${initial_vehicles} ${max_vehicles} ${num_actors_per_procs} \
              ${road_length_scale_down} ${road_length_minimum} ${fuel_scale_up}
