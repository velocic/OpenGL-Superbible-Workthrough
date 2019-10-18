cmake_minimum_required(VERSION 3.5)

project(OpenGL-Superbible-Workthrough)

set(SAMPLE_RUNNER_NAME sample-browser)

set(FLARE_RENDERSYSTEM_TYPE RENDERSYSTEM_GL4 CACHE EXTERNAL STRING)

add_compile_options(-std=c++17 -Wall -Wextra -ggdb3 -D${FLARE_RENDERSYSTEM_TYPE} -DGLM_ENABLE_EXPERIMENTAL)
set(PROJECT_SOURCE_DIR "${PROJECT_BINARY_DIR}/src")

add_executable(
    ${SAMPLE_RUNNER_NAME}
    src/first-program-tutorial/draw-a-triangle.cpp
    src/3-gpu-shader-pipeline/shader-pipeline.cpp
    src/5-data/data.cpp
    src/5b-spinning-cubes/spinning-cubes.cpp
    src/5c-array-textures/array-textures.cpp
    src/6a-model-file-loading/model-file-loading.cpp
    src/7a-instancing/instancing.cpp
    src/7b-packed-mesh-rendering/packed-mesh-rendering.cpp
    src/7c-scenegraph/scenegraph.cpp
    src/flare/gl/buffer.cpp
    src/flare/gl/vertexarray.cpp
    src/flare/gl/shaderprogram.cpp
    src/flare/gl/texturemanager.cpp
    src/flare/gl/texture.cpp
    src/flare/gl/texture2d.cpp
    src/flare/gl/arraytexture2d.cpp
    src/flare/gl/sampler.cpp
    src/flare/rendersystem/resizablebuffer.cpp
    src/flare/rendersystem/factory.cpp
    src/flare/rendersystem/samplermanager.cpp
    src/flare/rendersystem/shadermanager.cpp
    src/flare/rendersystem/wrappers.cpp
    src/flare/scenegraph/modelmanager.cpp
    src/flare/scenegraph/basicmesh.cpp
    src/flare/scenegraph/model.cpp
    src/flare/scenegraph/packedmesh.cpp
    src/flare/scenegraph/scenegraph.cpp
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
    assimp
)

target_include_directories(
    ${SAMPLE_RUNNER_NAME}
    PUBLIC
    "include"
    "lib"
    "lib/gl3w"
)
