cmake_minimum_required(VERSION 3.10)
project(woodgas)

set(LIBTYPE "STATIC")
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)

include_directories(stb/)
include_directories(openal-soft/include/)
include_directories(/usr/include/python3.8/)

add_library(woodgas STATIC src/render/glad/glad.c src/render/render.cc src/input/input.cc src/util/logging.cc src/asset/asset.cc src/script/python.cc src/core/core.cc)
set_property(TARGET woodgas PROPERTY CXX_STANDARD 17)
target_link_libraries(woodgas glfw python3.8 z OpenAL)

add_executable(render_test src/tests/render.cc)
target_link_libraries(render_test woodgas)

add_executable(logging_test src/tests/logging.cc)
target_link_libraries(logging_test woodgas)

add_executable(asset_test src/tests/assets.cc)
target_link_libraries(asset_test woodgas)

add_executable(python_test src/tests/python.cc)
target_link_libraries(python_test woodgas)

add_executable(core_test src/tests/core.cc)
target_link_libraries(core_test woodgas)
