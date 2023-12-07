cmake_minimum_required(VERSION 3.20)

project(C2dShooter)

cmake_policy(SET CMP0054 NEW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_C_STANDARD 17)

add_subdirectory(dependencies)
add_subdirectory(src)
