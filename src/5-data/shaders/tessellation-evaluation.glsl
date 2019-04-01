#version 460 core

//Sets the tessellation mode to be triangles, equally spaced,
//with clockwise vertex winding
layout (triangles, equal_spacing, cw) in;

patch in vec4 tcs_color;
out vec4 tes_color;

void main(void)
{
    tes_color = tcs_color;

    gl_Position = (
        gl_TessCoord.x * gl_in[0].gl_Position +
        gl_TessCoord.y * gl_in[1].gl_Position +
        gl_TessCoord.z * gl_in[2].gl_Position
    );
}
