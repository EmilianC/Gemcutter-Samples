Attributes
{
	vec4 a_vert : 0;
}

Uniforms
{
	template static Material : 0
	{
		vec3 Color = (0.0, 0.0, 0.0);
	}
}

Vertex
{
	void main()
	{
		gl_Position = Jwl_MVP * a_vert;
	}
}

Fragment
{
	out vec3 outColor;

	void main()
	{
		outColor = Material.Color;
	}
}
