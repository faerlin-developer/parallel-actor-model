# Parallel Actor Model

This is my final project for the [Parallel Design Pattern](http://www.drps.ed.ac.uk/24-25/dpt/cxinfr11168.htm) course at the University of Edinburgh. We developed a distributed actor version of the traffic simulation code that supports multiple actors per MPI process. To achieve this, we created the __Parallel Actor Model__ (`PAM`) framework, implemented in C++ with MPI. For performance testing, the traffic simulation code was executed using up to 4002 CPU cores.

Refer to [parallel-actor-model.pdf](https://github.com/faerlin-developer/parallel-actor-model/blob/main/parallel-actor-model.pdf) for a detailed description of the framework's design, implementation, and API.

## Contents

This directory contains two folders:

- `framework`: The source code of the `PAM` framework.
- `user`: User applications that uses the `PAM` framework.

The `user/traffic_simulation` directory contains the following folders:

- `data`: road network configuration files
- `include`: header files
- `jobs`: slurm files
- `lib`: a relative symlink pointing to the framework directory
- `src`: source files

## Problem Sizes

The application supports the following road network configurations:

| Road Network | Junctions | Roads  | Initial Vehicles | Maximum Active Vehicles |
| ------------ | --------- | ------ | ---------------- | ----------------------- |
| Small        | 1000      | 2516   | 100              | 1000                    |
| Medium       | 20000     | 55472  | 100              | 1000                    |
| Large        | 50000     | 142166 | 200              | 2000                    |
| Largest      | 100000    | 277590 | 200              | 2000                    |

## How To Run

This framework and the provided user programs were tested on [ARCHER2](https://www.archer2.ac.uk/), a tier-1 HPC system in [EPCC](https://www.epcc.ed.ac.uk/). 

### Hello World

1. In `user/hello_world/jobs/hello_world.slurm`, update the value of `--account`.
2. Run the following commands
    ```bash
    cd user/hello_world
    make build
    make run
    ```

### Traffic Simulation

1. In `user/traffic_simulation/jobs/largest.slurm`, update the value of `--account`.
2. Run the following commands
   ```bash
   cd user/traffic_simulation
   make build
   make run-largest
   ```

The Makefile targets for the other problem sizes are `run-small`, `run-medium`, and `run-large`.
