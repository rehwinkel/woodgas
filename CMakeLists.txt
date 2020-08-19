cmake_minimum_required(VERSION 3.10)
project(woodgas)
find_package(PkgConfig REQUIRED) 

set(LIBTYPE "STATIC")
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)

include_directories(stb/)
include_directories(openal-soft/include/)

add_library(woodgas STATIC src/render/glad/glad.c src/render/render.cc src/input/input.cc)
target_link_libraries(woodgas glfw python3.8 z OpenAL)

add_executable(render_test src/tests/render.cc)
target_link_libraries(render_test woodgas)