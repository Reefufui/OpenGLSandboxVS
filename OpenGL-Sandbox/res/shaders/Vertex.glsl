#version 450 core

in vec4 position;

out VS_OUT
{
	vec4 color;
} vs_out;

uniform mat4 mvpMatrix;

void main(void)
{
	gl_Position = position * mvpMatrix;
	vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 1.0);
}