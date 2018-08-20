#version 460 core

layout (location = 0) in vec4 offset;
out vec4 vs_color;

void main(void)
{
    const vec4 vertices[3] = vec4[3](
        vec4(0.25, -0.25, 0.5, 1.0),
        vec4(-0.25, -0.25, 0.5, 1.0),
        vec4(0.25, 0.25, 0.5, 1.0)
    );

    vs_color = offset;

    gl_Position = vertices[gl_VertexID];
    gl_Position = vertices[gl_VertexID] + offset;
}
