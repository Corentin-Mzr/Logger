cmake_minimum_required(VERSION 3.10)

# Project name
project(LogWriterProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the src directory to the include path
include_directories(src)

# Gather all source files
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(LogWriter ${SOURCES})