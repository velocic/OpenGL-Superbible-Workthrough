#version 460 core

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

out VS_OUT
{
    vec4 color;
    vec2 uvCoord;
} vs_out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1);
    vs_out.color = vec4(position, 0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
    vs_out.uvCoord = uvCoord;

    //prevent fields from optimizing out
    vs_out.color = vs_out.color + vec4(normal, 0) + vec4(uvCoord, 0, 0) - vec4(normal, 0) - vec4(uvCoord, 0, 0);
}