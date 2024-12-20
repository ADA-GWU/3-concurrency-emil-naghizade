# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name and supported languages
project(OpenCVProject LANGUAGES CXX)

# Find the OpenCV package
find_package(OpenCV REQUIRED)

# Include OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

# Add source files
# Replace "Source.cpp" with the actual name of your source file
add_executable(my_program src/Source.cpp)

# Link OpenCV libraries
target_link_libraries(my_program ${OpenCV_LIBS})

# Optional: Specify the C++ standard
set_target_properties(my_program PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# Print OpenCV version (Optional for logging)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
