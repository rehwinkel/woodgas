cmake_minimum_required(VERSION 3.10)
project(woodgas)

add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(zlib EXCLUDE_FROM_ALL)
set(LIBTYPE "STATIC")
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)
add_library(lua STATIC lua/lapi.c lua/lcode.c lua/lctype.c lua/ldebug.c lua/ldo.c lua/ldump.c lua/lfunc.c lua/lgc.c lua/llex.c lua/lmem.c lua/lobject.c lua/lopcodes.c lua/lparser.c lua/lstate.c lua/lstring.c lua/ltable.c lua/ltm.c lua/lundump.c lua/lvm.c lua/lzio.c lua/ltests.c lua/lauxlib.c lua/lbaselib.c lua/ldblib.c lua/liolib.c lua/lmathlib.c lua/loslib.c lua/ltablib.c lua/lstrlib.c lua/lutf8lib.c lua/loadlib.c lua/lcorolib.c lua/linit.c)

include_directories(glfw/include)
include_directories(lua/)
include_directories(stb/)
include_directories(zlib/)
include_directories(AudioFile/)
include_directories(openal-soft/include/)

add_library(woodgas STATIC src/render/glad/glad.c src/render/render.cc src/input/input.cc)
target_link_libraries(woodgas glfw lua zlibstatic OpenAL)

add_executable(woodgas_test src/woodgas_test.cc)
target_link_libraries(woodgas_test woodgas)