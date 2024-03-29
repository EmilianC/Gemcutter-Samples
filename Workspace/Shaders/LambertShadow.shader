Uniforms
{
	Light : 0
	{
		vec3 Color;
		vec3 Position;
		vec3 Direction;
		float AttenuationLinear;
		float AttenuationQuadratic;
		float Angle;
		uint Type;
	}

	static Ambient : 1
	{
		vec3 Color = (0.1, 0.1, 0.1);
	}

	static ShadowParams : 2
	{
		mat4 WorldToShadow;
		float Bias = 0.0005;
	}
}

Attributes
{
	vec4 a_vert   : 0;
	vec2 a_uv     : 1;
	vec3 a_normal : 2;
}

Vertex
{
	out vec2 texcoord;
	out vec3 norm;
	out vec3 pos;
	out vec4 shadowCoord;

	void main()
	{
		texcoord = a_uv;
		norm = Gem_NormalToWorld * a_normal;

		pos = (Gem_Model * a_vert).xyz;

		shadowCoord = ShadowParams.WorldToShadow * vec4(pos, 1.0f);
		shadowCoord.z -= ShadowParams.Bias;

		gl_Position = Gem_ViewProj * vec4(pos, 1.0f);
	}
}

Samplers
{
	sampler2D sTex : 0;
	sampler2DShadow sShadowMap : 1;
}

Fragment
{
	in vec2 texcoord;
	in vec3 norm;
	in vec3 pos;
	in vec4 shadowCoord;

	out vec3 outColor;

	void main()
	{
		vec3 normal = normalize(norm);
		vec3 lighting = Ambient.Color;

		float visibilityFactor = 0.0f;
		float NdotL = dot(normal, -Light.Direction);
		if (NdotL > 0.0f)
		{
			// If the normal is facing the light, we need to check for shadows.
			visibilityFactor = texture(sShadowMap, shadowCoord.xyz);
			lighting += compute_light(Light, normal, pos) * visibilityFactor;
		}

		outColor = texture(sTex, texcoord).rgb * lighting;

		// Since we render directly into the backbuffer,
		// there will not be an implicit linear->sRGB conversion.
		outColor = linear_to_sRGB(outColor);
	}
}
