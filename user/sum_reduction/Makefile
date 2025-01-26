
# Actor model framework
FRAMEWORK_DIR = lib/framework
FRAMEWORK_H = ${FRAMEWORK_DIR}/include
FRAMEWORK_S = ${FRAMEWORK_DIR}/src
FRAMEWORK_SRC = ${FRAMEWORK_S}/*/*.cpp

# User code
USER_H = include
USER_SRC = src/*.cpp

# ---------------------------------------------------------------- #
# Targets for ARCHER2                                              #
# ---------------------------------------------------------------- #

.PHONY: build
build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	CC -O2 -o ${EXE} ${FRAMEWORK_SRC} ${USER_SRC} -I ${FRAMEWORK_H} -I ${USER_H}

run:
	sbatch jobs/sum_reduction.slurm

# ---------------------------------------------------------------- #
# Targets for Local Machine                                        #
# ---------------------------------------------------------------- #

NUM_PROCS = 2
EXE = build/sum_reduction

local-build:
	rm -rf lib && mkdir lib && cd lib && ln -s ../../../framework/ framework
	rm -rf build && mkdir build
	mpicxx -o ${EXE} ${FRAMEWORK_SRC} ${USER_SRC} -I ${FRAMEWORK_H} -I ${USER_H}

local-run:
	mpiexec -n ${NUM_PROCS} ./${EXE}
