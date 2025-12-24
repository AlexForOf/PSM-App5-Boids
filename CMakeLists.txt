cmake_minimum_required(VERSION 3.14)

project(Boids)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.x  
)

FetchContent_MakeAvailable(sfml)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Boids ${SOURCES})

target_include_directories(Boids PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/boids
    ${CMAKE_SOURCE_DIR}/src/boids/predator
)

target_link_libraries(Boids PRIVATE sfml-graphics sfml-window sfml-system)
