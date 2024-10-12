cmake_minimum_required(VERSION 3.11)

project(
    waybar-scrolling-mpris
    VERSION 1.0
    LANGUAGES CXX)

    
find_package(json-c CONFIG)

add_executable(${PROJECT_NAME} waybar-scrolling-mpris.cpp config.cpp playerctl_helper.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE json-c::json-c)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)