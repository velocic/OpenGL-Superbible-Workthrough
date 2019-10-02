#version 460 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec4 color;
} fs_in;

layout (binding = 0) uniform sampler2D textureDiffuse0;
layout (binding = 1) uniform sampler2D textureSpecular0;
layout (binding = 2) uniform sampler2D textureNormal0;

void main(void)
{
    //color = fs_in.color;
    color = texture(textureDiffuse0, vec2(0, 0)) + texture(textureSpecular0, vec2(0, 0)) + texture(textureNormal0, vec2(0, 0)) + fs_in.color;
}
