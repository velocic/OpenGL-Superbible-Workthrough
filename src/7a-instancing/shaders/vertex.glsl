#version 460 core

layout(location = 0) in vec3 position;

void main(void)
{
    gl_Position = position;
}
