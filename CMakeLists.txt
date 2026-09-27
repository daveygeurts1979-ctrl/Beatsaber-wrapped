cmake_minimum_required(VERSION 3.21)
project(beatsaberwrapped)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QPM-restored dependencies land here after `qpm restore`
include_directories(${CMAKE_SOURCE_DIR}/extern/includes)
include_directories(${CMAKE_SOURCE_DIR})
link_directories(${CMAKE_SOURCE_DIR}/extern/libs)

add_compile_options(
    -fno-rtti
    -fexceptions
    -Wall
    -Wextra
    -DVERSION=\"0.1.0\"
    -DMOD_ID=\"BeatSaberWrapped\"
)

# Everything is flat in the repo root now - just grab every .cpp file.
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)

add_library(beatsaberwrapped SHARED ${SOURCES})

if(EXISTS ${CMAKE_SOURCE_DIR}/extern/extern.cmake)
    include(${CMAKE_SOURCE_DIR}/extern/extern.cmake)
endif()

target_link_libraries(beatsaberwrapped
    log
)
