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
	gl_Position = position * mvMatrix;
	vs_out.color = position * 2.0f + vec4(0.5, 0.5, 0.2, 1.0);
}