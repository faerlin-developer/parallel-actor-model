
# Actor Model Framework
FRAMEWORK_DIR = lib/framework
FRAMEWORK_H = ${FRAMEWORK_DIR}/include
FRAMEWORK_S = ${FRAMEWORK_DIR}/src
FRAMEWORK_SRC = ${FRAMEWORK_S}/*/*.cpp

# User code
USER_SRC = src/main.cpp

# ---------------------------------------------------------------- #
# Targets for ARCHER2                                              #
# ---------------------------------------------------------------- #

.PHONY: build
build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	CC -O2 -o ${EXE} ${FRAMEWORK_SRC} ${USER_SRC} -I ${FRAMEWORK_H}

run:
	sbatch jobs/hello_world.slurm

# ---------------------------------------------------------------- #
# Targets for Local Machine                                        #
# ---------------------------------------------------------------- #

NUM_PROCS = 1
EXE = build/hello_world

local-build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	mpicxx -o ${EXE} ${FRAMEWORK_SRC} ${USER_SRC} -I ${FRAMEWORK_H}

local-run:
	mpiexec -n ${NUM_PROCS} ./${EXE}