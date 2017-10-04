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

	template static Ambient : 1
	{
		vec3 Color = (0.1, 0.1, 0.1);
	}
	
	template static ShadowParams : 2
	{
		mat4 WorldToShadow;
		float Intensity = 0.35;
		float Bias = 0.0005;
	}
}

Attributes
{
	vec4 a_vert		: 0;
	vec2 a_uv		: 1;
	vec3 a_normal	: 2;
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
		norm = mat3(Jwl_NormalToWorld) * a_normal;

		pos = (Jwl_Model * a_vert).xyz;
		
		shadowCoord = ShadowParams.WorldToShadow * vec4(pos, 1.0f);
		gl_Position = Jwl_ViewProj * vec4(pos, 1.0f);
	}
}

Samplers
{
	sampler2D sTex 		 : 0;
	sampler2D sShadowMap : 1;
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
		lighting += compute_light(Light, normal, pos);
		
		// If the normal is facing the light, we need to check for shadows.
		float NdotL = dot(normal, -Light.Direction);
		if (NdotL > 0.0 &&
			shadowCoord.z < texture(sShadowMap, shadowCoord.xy).r + ShadowParams.Bias)
		{
			// Calculate the diffuse contribution.
			lighting += Light.Color * NdotL;
		}
		else
		{
			lighting *= ShadowParams.Intensity;
		}
		
		outColor = texture(sTex, texcoord).rgb * lighting;
	}
}
