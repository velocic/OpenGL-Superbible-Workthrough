#version 460 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec2 uvCoord;
} fs_in;

layout (binding = 0) uniform sampler2D textureDiffuse0;
layout (binding = 1) uniform sampler2D textureDiffuse1;
layout (binding = 2) uniform sampler2D textureSpecular0;
layout (binding = 3) uniform sampler2D textureNormal0;

void main(void)
{
    color = texture(textureDiffuse0, fs_in.uvCoord);
}
