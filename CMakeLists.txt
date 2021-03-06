cmake_minimum_required(VERSION 3.10)
project(woodgas)

enable_testing()

set(JSON_BuildTests OFF)
set(PY_VERSION 3.8)
find_package(PythonLibs ${PY_VERSION} REQUIRED)

include_directories(stb/)
include_directories(FastNoise/)
include_directories(json/include/)
include_directories(glfw/include/)
include_directories(zlib/ ${CMAKE_CURRENT_BINARY_DIR}/zlib/)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(json)
add_subdirectory(glfw)
add_subdirectory(zlib)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wconversion -Wno-cast-function-type)
endif()

add_library(woodgas STATIC src/render/glad/glad.c src/render/render.cc src/input/input.cc src/util/timer.cc src/util/logging.cc src/asset/asset.cc src/script/python.cc src/core/core.cc src/util/math.cc FastNoise/FastNoise.cpp)
set_property(TARGET woodgas PROPERTY CXX_STANDARD 17)
target_link_libraries(woodgas glfw zlibstatic ${CMAKE_DL_LIBS} ${PYTHON_LIBRARIES} nlohmann_json)

add_executable(bundler src/bundler.cc)
target_include_directories(bundler PUBLIC src/)
set_property(TARGET bundler PROPERTY CXX_STANDARD 17)
target_link_libraries(bundler woodgas)

add_subdirectory(examples)

add_executable(render_debug test/debug/render.cc)
target_include_directories(render_debug PUBLIC src/)
set_property(TARGET render_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(render_debug woodgas)

add_executable(logging_debug test/debug/logging.cc)
target_include_directories(logging_debug PUBLIC src/)
set_property(TARGET logging_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(logging_debug woodgas)

add_executable(asset_debug test/debug/assets.cc)
target_include_directories(asset_debug PUBLIC src/)
set_property(TARGET asset_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(asset_debug woodgas)

add_executable(python_debug test/debug/python.cc)
target_include_directories(python_debug PUBLIC src/)
set_property(TARGET python_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(python_debug woodgas)

add_executable(core_debug test/debug/core.cc)
target_include_directories(core_debug PUBLIC src/)
set_property(TARGET core_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(core_debug woodgas)

add_executable(time_debug test/debug/time.cc)
target_include_directories(time_debug PUBLIC src/)
set_property(TARGET time_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(time_debug woodgas)

add_executable(full_debug test/debug/full.cc)
target_include_directories(full_debug PUBLIC src/)
set_property(TARGET full_debug PROPERTY CXX_STANDARD 17)
target_link_libraries(full_debug woodgas)

add_executable(python_test test/python.cc)
target_include_directories(python_test PUBLIC src/)
set_property(TARGET python_test PROPERTY CXX_STANDARD 17)
target_link_libraries(python_test woodgas)

add_test(NAME python_test COMMAND python_test)