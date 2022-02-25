# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (fileTransmit)

SET(CMAKE_C_FLAGS "-Wall -g -O2")

SET(PLAT ${PLAT} CACHE STRING "Choose the plat of build: x86 hi3559av100 hi3516dv300 hi3519av100" FORCE)

if(NOT PLAT)
    SET(CMAKE_C_COMPILER gcc)
endif()

if(PLAT STREQUAL "x86_64")
MESSAGE("Plat = x86_64")
SET(CMAKE_C_COMPILER gcc)
endif()

if(PLAT STREQUAL "hi3559av100")
MESSAGE("Plat = hi3559av100")
SET(CMAKE_C_COMPILER aarch64-himix100v630-linux-gcc)
endif()

if(PLAT STREQUAL "hi3516dv300")
MESSAGE("Plat = hi3516dv300")
SET(CMAKE_C_COMPILER arm-himix200v002-linux-gcc)
endif()

if(PLAT STREQUAL "hi3519av100")
MESSAGE("Plat = hi3519av100")
SET(CMAKE_C_COMPILER aarch64-himix200-linux-gcc)
endif()

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    ethernet.c
    fileTransmit.c
)

# Add an executable with the above sources
add_executable(fileTransmit ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running gcc these will be included as -I/directory/path/
target_include_directories(fileTransmit
    PRIVATE 
        ethernet.h
)

