Uniforms
{
	static Properties : 0
	{
		vec2 LightPositionOnScreen;
		float Exposure = 0.0039f;
		float Decay = 1.0f;
		float Weight = 7.0f;
	}
}

Vertex
{
	#include "./Assets/Shaders/GodRays/FullScreenQuad.vert"
}

Samplers
{
	sampler2D sTex : 0;
}

Fragment
{
	#define NUM_SAMPLES 256

	in vec2 texcoord;

	out vec3 outColor;

	void main()
	{
		vec2 texCoord = texcoord;
		vec2 deltaTextCoord = Properties.LightPositionOnScreen - texcoord;
		deltaTextCoord /= float(NUM_SAMPLES);

		outColor = vec3(0.0f, 0.0f, 0.0f);

		float illuminationDecay = 1.0;
		for (int i = 0; i < NUM_SAMPLES; i++)
		{
			texCoord += deltaTextCoord;
			outColor += texture(sTex, texCoord).rgb * illuminationDecay * Properties.Weight;

			illuminationDecay *= Properties.Decay;
		}

		outColor *= Properties.Exposure;
	}
}
