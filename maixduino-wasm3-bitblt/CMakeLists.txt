# basic config
if (NOT PROJ)
    get_filename_component(PROJ ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)
    get_filename_component(PROJ ${PROJ} NAME)
    string(REPLACE " " "_" PROJ ${PROJ})
    message(STATUS "PROJ not set, use ${PROJ} as PROJ. Also, you can set it manually. e.g. -DPROJ=hello_world")
else()
    message("PROJ = ${PROJ}")
endif ()
cmake_minimum_required(VERSION 3.0)
include(./cmake/common.cmake)
project(${PROJ} C CXX ASM)

# config self use headers
include(./cmake/macros.internal.cmake)
header_directories(${SDK_ROOT}/lib)
header_directories(${PROJ})
header_directories(${PROJ}/m3)

# compile project
add_source_files(
    ${PROJ}/main.c
    ${PROJ}/maixduino.c
    ${PROJ}/lcd/lcd.c
    ${PROJ}/lcd/st7789.c
    ${PROJ}/m3/m3_api_libc.c
    ${PROJ}/m3/m3_api_meta_wasi.c
    ${PROJ}/m3/m3_api_wasi.c
    ${PROJ}/m3/m3_bind.c
    ${PROJ}/m3/m3_code.c
    ${PROJ}/m3/m3_compile.c
    ${PROJ}/m3/m3_core.c
    ${PROJ}/m3/m3_emit.c
    ${PROJ}/m3/m3_env.c
    ${PROJ}/m3/m3_exec.c
    ${PROJ}/m3/m3_info.c
    ${PROJ}/m3/m3_module.c
    ${PROJ}/m3/m3_optimize.c
    ${PROJ}/m3/m3_parse.c
)
add_compile_flags(C
    -Os
    -Dd_m3LogOutput=false
    -Dd_m3VerboseLogs=false
)
include(./cmake/executable.cmake)
