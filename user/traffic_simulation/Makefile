
include common.mk

.PHONY: build
# ---------------------------------------------------------------- #
# Targets for ARCHER2                                              #
# ---------------------------------------------------------------- #

build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	CC -O2 -o ${EXE} ${SRC} ${INCLUDE} -lm

run-tiny:
	sbatch jobs/tiny.slurm

run-small:
	sbatch jobs/small.slurm

run-medium:
	sbatch jobs/medium.slurm

run-large:
	sbatch jobs/large.slurm

run-largest:
	sbatch jobs/largest.slurm

# ---------------------------------------------------------------- #
# Targets for Local Machine                                        #
# ---------------------------------------------------------------- #

local-build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	mpicxx -o ${EXE} ${SRC} ${INCLUDE} -lm

local-run-tiny:
	mpiexec -n 4 ${EXE} data/tiny_problem 30 30 150 5 1 0 2

local-run-small:
	mpiexec -n 12 ${EXE} data/small_problem 30 100 1000 100 1 0 2

clean:
	rm -rf main
