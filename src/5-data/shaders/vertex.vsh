#version 460 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

layout (location = 3) uniform mat4 mvpMatrix;

//example of an interface block
out VS_OUT
{
    vec4 color; //send color to the next stage
} vs_out;

void main(void)
{
    vs_out.color = color;

    gl_Position = position * mvpMatrix;
}
