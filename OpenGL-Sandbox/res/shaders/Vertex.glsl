#version 450 core

in vec4 position;

out VS_OUT
{
	vec4 color;
} vs_out;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;

void main(void)
{
	gl_Position = position * mvMatrix * projMatrix;
	vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 1.0);
}