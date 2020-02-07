#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 modelMatrix;

uniform mat4 projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * modelMatrix * vec4(position, 1);
}
