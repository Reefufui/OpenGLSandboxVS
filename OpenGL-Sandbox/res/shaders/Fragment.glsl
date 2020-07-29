#version 450 core

in VS_OUT
{
	vec4 color;
} fs_in;

out vec4 color;

void main(void)
{
	color = vec4(fs_in.color.r, fs_in.color.g, fs_in.color.b, 1.0);
}