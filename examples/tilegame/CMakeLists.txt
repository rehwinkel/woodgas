project(tilegame)

add_custom_command(OUTPUT assets.c COMMAND bundler ${PROJECT_SOURCE_DIR}/res/)
add_executable(tilegame src/main.cc ${CMAKE_CURRENT_BINARY_DIR}/assets.c)
target_include_directories(tilegame PUBLIC ../../src/)
set_property(TARGET tilegame PROPERTY CXX_STANDARD 17)
target_link_libraries(tilegame woodgas)