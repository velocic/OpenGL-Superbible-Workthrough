#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvCoord;
layout(location = 3) in mat4 mvpMatrix;

out VS_OUT
{
    vec4 color;
} vs_out;


void main(void)
{
    gl_Position = mvpMatrix * vec4(position, 1);

    vs_out.color = vec4(position, 0) * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);

    //prevent fields from optimizing out
    vs_out.color = vs_out.color + vec4(normal, 0) + vec4(uvCoord, 0, 0) - vec4(normal, 0) - vec4(uvCoord, 0, 0);
}
