#version 430 core

//Specifies the number of control points emitted from the tessellation control shader.
//This number can differ from the number of control points received from the vertex shader.
//Default number of control points received per patch from the vertex shader is 3
layout (vertices = 3) out;

void main(void)
{
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 5.0;
        gl_TessLevelOuter[0] = 5.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 5.0;
    }

    //everybody copies their input to their output
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
