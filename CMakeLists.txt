cmake_minimum_required(VERSION 3.14)

project(Boids)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.x  
)

FetchContent_MakeAvailable(sfml)

add_executable(Boids main.cpp)

target_link_libraries(Boids PRIVATE sfml-graphics sfml-window sfml-system)