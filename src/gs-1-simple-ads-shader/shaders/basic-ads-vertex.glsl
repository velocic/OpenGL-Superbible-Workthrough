#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvCoord;
layout(location = 3) in mat4 mvpMatrix;

uniform mat4 projectionMatrix;
uniform vec4 color;

out VS_OUT
{
    vec4 color;
} vs_out;

void main(void)
{
    vec4 preventUnusedOptimizingOut = vec4(normal, 0) + vec4(uvCoord, 0, 0) - vec4(normal, 0) - vec4(uvCoord, 0, 0);

    vs_out.color = color;

    gl_Position = projectionMatrix * mvpMatrix * vec4(position, 1);
    gl_Position += preventUnusedOptimizingOut;
}
