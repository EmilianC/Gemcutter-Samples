layout(location = 0) in vec4 a_vert;
layout(location = 1) in vec2 a_uv;

out vec2 texcoord;

void main()
{
	texcoord = a_uv;
	gl_Position = a_vert;
}
