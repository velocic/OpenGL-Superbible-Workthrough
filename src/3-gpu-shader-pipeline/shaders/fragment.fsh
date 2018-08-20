#version 430 core

//matches the OUT interface block declaration from the fragment shader
//but we can rename the instance variable here as "fs_in", instead of
//reusing a name that was probably more meaningful to the last
//shader stage
in VS_OUT
{
    vec4 color;
} fs_in;

out vec4 color;

void main(void)
{
    color = fs_in.color;
}
