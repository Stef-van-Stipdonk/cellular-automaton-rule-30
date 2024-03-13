cmake_minimum_required(VERSION 3.22)
project(Rule_30)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(rule_30 "./src/main.c")

target_link_libraries(rule_30 PRIVATE ${SDL2_LIBRARIES})
