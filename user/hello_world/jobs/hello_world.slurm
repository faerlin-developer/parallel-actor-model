#!/bin/sh
#SBATCH --job-name=hello_world
#SBATCH --time=00:00:10
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --account=m24ol-s2465760
#SBATCH --partition=standard
#SBATCH --qos=short

export OMP_NUM_THREADS=1

srun build/hello_world
