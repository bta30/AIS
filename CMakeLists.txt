cmake_minimum_required(VERSION 3.0)
project(ais C)

add_executable(ais
    src/core/main.c
    src/args/args.c
    src/interpolate/interpolate.c
    src/align/noAlign.c
    src/align/starThresholdAlign.c
    src/stack/stack.c
    src/vec/vec.c
    src/imgio/imgio.c
    src/calibrate/calibrate.c
)
target_include_directories(ais PRIVATE include)
target_link_libraries(ais PRIVATE m tiff)