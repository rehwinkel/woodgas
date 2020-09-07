cmake_minimum_required(VERSION 3.10)
project(woodgas)

set(PY_VERSION 3.8)
find_package(PythonLibs ${PY_VERSION} REQUIRED)

include_directories(stb/)
include_directories(libsoundio/)
include_directories(rapidjson/include/)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(libsoundio)

add_library(woodgas STATIC src/render/glad/glad.c src/render/render.cc src/input/input.cc src/util/timer.cc src/util/logging.cc src/asset/asset.cc src/script/python.cc src/core/core.cc)
set_property(TARGET woodgas PROPERTY CXX_STANDARD 17)
target_link_libraries(woodgas glfw z dl libsoundio_static ${PYTHON_LIBRARIES})

add_executable(render_test src/tests/render.cc)
set_property(TARGET render_test PROPERTY CXX_STANDARD 17)
target_link_libraries(render_test woodgas)

add_executable(logging_test src/tests/logging.cc)
set_property(TARGET logging_test PROPERTY CXX_STANDARD 17)
target_link_libraries(logging_test woodgas)

add_executable(asset_test src/tests/assets.cc)
set_property(TARGET asset_test PROPERTY CXX_STANDARD 17)
target_link_libraries(asset_test woodgas)

add_executable(python_test src/tests/python.cc)
set_property(TARGET python_test PROPERTY CXX_STANDARD 17)
target_link_libraries(python_test woodgas)

add_executable(core_test src/tests/core.cc)
set_property(TARGET core_test PROPERTY CXX_STANDARD 17)
target_link_libraries(core_test woodgas)

add_executable(time_test src/tests/time.cc)
set_property(TARGET time_test PROPERTY CXX_STANDARD 17)
target_link_libraries(time_test woodgas)

add_executable(full_test src/tests/full.cc)
set_property(TARGET full_test PROPERTY CXX_STANDARD 17)
target_link_libraries(full_test woodgas)
