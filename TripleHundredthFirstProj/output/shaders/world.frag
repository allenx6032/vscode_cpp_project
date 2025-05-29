#version 330 core

in vec3 pos;
out vec4 color;

uniform sampler2D atlas;
uniform sampler2D textures;

void main()
{
	vec2 coords = vec2(mod(pos.x, 32) / 32, mod(pos.z, 32) / 32);
	//vec2 coords = vec2(fract(pos.x), fract(pos.z));
	//vec2 coords = vec2(0.5, 0.5);
	//color = texture2D(atlas, coords);
	color = vec4(pos,1.0);
	//color = vec4(lol, 0.0f, 0.0f, 1.0f);
}
