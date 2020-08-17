cmake_minimum_required(VERSION 3.10)
project(woodgas)

add_subdirectory(glfw)

include_directories(glfw/include)
add_executable(woodgas_test src/woodgas_test.cc src/glad/glad.c)
target_link_libraries(woodgas_test glfw)