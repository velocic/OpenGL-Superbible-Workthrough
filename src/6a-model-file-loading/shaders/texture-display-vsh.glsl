#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

out VS_OUT
{
    vec2 uvCoord;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1) + vec4(normal, 0) - vec4(normal, 0);
    vs_out.uvCoord = uvCoord;
}
