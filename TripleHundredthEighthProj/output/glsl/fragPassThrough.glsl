#version 150

in vec4 vertex_color;

void main(void)
{
	gl_FragColor = vertex_color;
}
