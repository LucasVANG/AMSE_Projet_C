cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(AMSE_ProjetC VERSION 1.0)

# on spécifie des options pour le compilateur
string(APPEND CMAKE_C_FLAGS " -lm -lrt")

# on spécifie où seront les fichiers binaires 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")

# add the executable
add_executable(DCMoteur ${PROJECT_SOURCE_DIR}/src/DCMoteur.c)
add_executable(ecrimemparcycle ${PROJECT_SOURCE_DIR}/src/ecrimemparcycle.c)
add_executable(Kinematics ${PROJECT_SOURCE_DIR}/src/Kinematics.c)
add_executable(lecMemParTVL ${PROJECT_SOURCE_DIR}/src/lecMemParTVL.c)
add_executable(lecMemParTVR ${PROJECT_SOURCE_DIR}/src/lecMemParTVR.c)
add_executable(lectempkine ${PROJECT_SOURCE_DIR}/src/lectempkine.c)
add_executable(Odometry ${PROJECT_SOURCE_DIR}/src/Odometry.c)
add_executable(regPID ${PROJECT_SOURCE_DIR}/src/regPID.c)
add_executable(ResetState ${PROJECT_SOURCE_DIR}/src/ResetState.c)


message(STATUS "PROJECT SOURCE DIR = ${PROJECT_SOURCE_DIR}")