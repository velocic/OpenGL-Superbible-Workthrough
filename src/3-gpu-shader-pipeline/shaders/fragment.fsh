#version 460 core

in vec4 tes_color;

out vec4 color;

void main(void)
{
    color = tes_color;
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
