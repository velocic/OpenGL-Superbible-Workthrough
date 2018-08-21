#version 460 core

layout (location = 0) in vec4 offset;
layout (location = 1) in vec4 color;

//example of an interface block
out VS_OUT
{
    vec4 color; //send color to the next stage
} vs_out;

void main(void)
{
    const vec4 vertices[3] = vec4[3](
        vec4(0.25, -0.25, 0.5, 1.0),
        vec4(-0.25, -0.25, 0.5, 1.0),
        vec4(0.25, 0.25, 0.5, 1.0)
    );

    vs_out.color = color;

    gl_Position = vertices[gl_VertexID] + offset;
}
