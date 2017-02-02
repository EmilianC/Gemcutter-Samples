Vertex
{
	#include "./Assets/Shaders/GodRays/FullScreenQuad.vert"
}

Samplers
{
	sampler2D sScene : 0;
	sampler2D sRays1 : 1;
	sampler2D sRays2 : 2;
}

Fragment
{
	in vec2 texcoord;

	out vec3 outColor;

	void main()
	{
		vec3 colorA = texture(sScene, texcoord).rgb;
		vec3 colorB = texture(sRays1, texcoord).rgb;
		vec3 colorC = texture(sRays2, texcoord).rgb;

		vec3 raysAvg = clamp(colorB + colorC, 0.0, 1.0);
	
		//Screen blend function
		outColor = 1.0 - (1.0 - colorA) * (1.0 - raysAvg);
	}
}
