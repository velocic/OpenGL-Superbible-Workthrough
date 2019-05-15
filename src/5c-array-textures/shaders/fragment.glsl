#version 460 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    flat int droplet;
    vec2 tc;
} fs_in;

layout (binding = 0) uniform sampler2DArray tex_droplets;

void main(void)
{
    color = texture(tex_droplets, vec3(fs_in.tc, float(fs_in.droplet)));
}
