cmake_minimum_required(VERSION 4.0)
project(GravLensing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SFML_DIR "/opt/homebrew/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(GravLensing src/main.cpp)

target_link_libraries(GravLensing
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
