#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 mvpMatrix;

void main(void)
{
    gl_Position = vec4(position, 1) * mvpMatrix;
}
