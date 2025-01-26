
# Actor Model Framework
FRAMEWORK_DIR = lib/framework
FRAMEWORK_H = ${FRAMEWORK_DIR}/include
FRAMEWORK_SRC = ${FRAMEWORK_DIR}/src/*/*.cpp

# User code
TRAFFIC_H = include
TRAFFIC_SRC = src/main.cpp src/*/*.cpp

SRC = ${TRAFFIC_SRC} ${FRAMEWORK_SRC}
INCLUDE = -I ${FRAMEWORK_H} -I ${TRAFFIC_H}
EXE = build/traffic_simulation_program
