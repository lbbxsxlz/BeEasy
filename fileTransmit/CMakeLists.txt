# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (fileTransmit)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_C_FLAGS "-Wall -g -O2")

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

