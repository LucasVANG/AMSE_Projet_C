cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(AMSE_ProjetC VERSION 2.0)

# on spécifie où seront les fichiers binaires 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")

# add the executable
add_executable(DCMoteur ${PROJECT_SOURCE_DIR}/src/DCMoteur.c)
add_executable(Kinematics ${PROJECT_SOURCE_DIR}/src/Kinematics.c)
add_executable(Odometry ${PROJECT_SOURCE_DIR}/src/Odometry.c)
add_executable(ResetState ${PROJECT_SOURCE_DIR}/src/ResetState.c)
add_executable(SetTv ${PROJECT_SOURCE_DIR}/src/SetTv.c)
add_executable(SetU ${PROJECT_SOURCE_DIR}/src/SetU.c)
add_executable(cmd_go ${PROJECT_SOURCE_DIR}/src/cmd_go.c)
add_executable(cmd_goTo ${PROJECT_SOURCE_DIR}/src/cmd_goTo.c)
add_executable(cmd_turn ${PROJECT_SOURCE_DIR}/src/cmd_turn.c)
add_executable(regPID ${PROJECT_SOURCE_DIR}/src/regPID.c)

# on spécifie des options pour le compilateur
target_link_libraries(DCMoteur m rt)
target_link_libraries(Kinematics m rt)
target_link_libraries(Odometry m rt)
target_link_libraries(ResetState m rt)
target_link_libraries(SetTv m rt)
target_link_libraries(SetU m rt)
target_link_libraries(cmd_go m rt)
target_link_libraries(cmd_goTo m rt)
target_link_libraries(cmd_turn m rt)
target_link_libraries(regPID m rt)



message(STATUS "CHEMIN DU PROJET = ${PROJECT_SOURCE_DIR}")
message(STATUS "AUTEURS DU PROJETS = Fabien Plouvier, Lucas Vangaeveren, Quentin Delezenne")