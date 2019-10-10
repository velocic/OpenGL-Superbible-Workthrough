#version 460 core

out vec4 color;

in VS_OUT
{
    vec4 color;
    vec2 uvCoord;
} fs_in;

layout (binding = 0) uniform sampler2D textureDiffuse0;
layout (binding = 1) uniform sampler2D textureDiffuse1;
layout (binding = 2) uniform sampler2D textureSpecular0;
layout (binding = 3) uniform sampler2D textureNormal0;

void main(void)
{
    //color = fs_in.color - fs_in.color + texture(textureDiffuse0, fs_in.uvCoord);
    //color = texture(textureDiffuse0, fs_in.uvCoord);
    //Prevent params optimizing out on Nvidia drivers for testing
    color = (fs_in.color + vec4(1, 0, 0, 1)) + texture(textureDiffuse0, fs_in.uvCoord) - fs_in.color - vec4(1, 0, 0, 1);
}
