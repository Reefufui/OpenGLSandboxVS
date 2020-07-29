#version 450 core

uniform Sampler2D s;

out vec4 color;

void main(void)
{
	color = texelFetch(s, ivec2(gl_FragCoord.xy), 0);
}