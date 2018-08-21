#version 460 core

in vec4 gs_color;

out vec4 color;

void main(void)
{
    color = vec4(
        sin(gl_FragCoord.x * 0.25) * 0.5 + gs_color.x,
        cos(gl_FragCoord.y * 0.25) * 0.5 + gs_color.y,
        sin(gl_FragCoord.x * 0.15) * cos(gl_FragCoord.y * 0.15),
        1.0
    );
}
