#version 150

layout(points) in;

out vec4 vertex_color;

void main(void)
{
	int max = gl_in.length();

	for (int i = 0; i < max; ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		vertex_color = vec4(0.0, (float(i) / float(max) * 0.9 + 0.1), 0.0, 1.0);
		EmitVertex();
	}
	EndPrimitive();
}
