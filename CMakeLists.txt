cmake_minimum_required(VERSION 3.5)

project(OpenGL-Superbible-Workthrough)

set(SAMPLE_RUNNER_NAME sample-browser)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -ggdb3")
set(PROJECT_SOURCE_DIR "${PROJECT_BINARY_DIR}/src")

add_executable(
    ${SAMPLE_RUNNER_NAME}
    src/first-program-tutorial/draw-a-triangle.cpp
    src/3-gpu-shader-pipeline/shader-pipeline.cpp
    src/5-data/data.cpp
    src/5b-spinning-cubes/spinning-cubes.cpp
    src/flare/gl/buffer.cpp
    src/flare/gl/uniformblock.cpp
    src/flare/gl/vertexarray.cpp
    src/flare/gl/shaderprogram.cpp
    src/flare/gl/texture.cpp
    src/flare/gl/texture2d.cpp
    src/flare/gl/arraytexture2d.cpp
    src/flare/gl/sampler.cpp
    src/flare/ui/renderwindow.cpp
    src/flare/utility/file.cpp
    src/flare/utility/math.cpp
    lib/gl3w/gl3w.c
    lib/lodepng/lodepng.cpp
    src/main.cpp
)

target_link_libraries(
    ${SAMPLE_RUNNER_NAME}
    SDL2
    GL
    dl
)

target_include_directories(
    ${SAMPLE_RUNNER_NAME}
    PUBLIC
    "include"
    "lib"
    "lib/gl3w"
)
