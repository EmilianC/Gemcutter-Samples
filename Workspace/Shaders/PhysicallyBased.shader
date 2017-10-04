Attributes
{
	vec4 a_vert		: 0;
	vec2 a_uv		: 1;
	vec3 a_normal	: 2;
	vec4 a_tangent	: 3;
}

Uniforms
{
	Light1 : 0
	{
		vec3 Color;
		vec3 Position;
		vec3 Direction;
		float AttenuationLinear;
		float AttenuationQuadratic;
		float Angle;
		uint Type;
	}
}

Samplers
{
	samplerCube sEnvironment : 0;
	samplerCube sIrradiance  : 1;
	sampler2D sAlbedo		 : 2;
	sampler2D sDetails		 : 3;
	sampler2D sNormals		 : 4;
}

Vertex
{
	out vec2 texcoord;
	out vec3 norm;
	out vec3 pos;
	out mat3 TBN;

	void main()
	{
		texcoord = a_uv;

		norm = mat3(Jwl_NormalToWorld) * a_normal;
		pos = (Jwl_Model * a_vert).xyz;

#ifdef USE_NORMAL_MAP
		TBN = make_TBN(a_normal, a_tangent.xyz, a_tangent.w);
#endif

#ifdef USE_HEIGHT_MAP
		float height = texture(sDetails, texcoord).b;
		pos += norm * height * 0.2;
#endif

		gl_Position = Jwl_ViewProj * vec4(pos, 1.0);
	}
}

Fragment
{
	in vec2 texcoord;
	in vec3 norm;
	in vec3 pos;
	in mat3 TBN;

	out vec3 outColor;

	float DistributionGGX(vec3 N, vec3 H, float roughness)
	{
		float a = roughness * roughness;
		float a2 = a * a;
		float NdotH = max(dot(N, H), 0.0);
		float NdotH2 = NdotH * NdotH;

		float nom = a2;
		float denom = NdotH2 * (a2 - 1.0) + 1.0;
		denom = M_PI * denom * denom;

		return nom / denom;
	}

	float GeometrySchlickGGX(float NdotV, float roughness)
	{
		float r = roughness + 1.0;
		float k = (r * r) / 8.0;

		float nom = NdotV;
		float denom = NdotV * (1.0 - k) + k;

		return nom / denom;
	}

	vec3 Fresnel(float cosTheta, vec3 F0, float roughness)
	{
		return F0 + (max(vec3(1.0 - roughness), F0) - F0) * vec3(pow(1.0 - cosTheta, 5.0));
	}

	void main()
	{
		// Gather material properties.
		vec3 albedo = texture(sAlbedo, texcoord).rgb;
		vec4 details = texture(sDetails, texcoord).rgba;
		float metallic = details.r;
		float roughness = details.g;
		float ao = details.a;

		vec3 N;
#ifdef USE_NORMAL_MAP
		N = normalize(TBN * (texture(sNormals, texcoord).rgb * 2.0 - 1.0));
#else
		N = normalize(norm);
#endif

		vec3 V = normalize(Jwl_CameraPosition - pos);
		float NdotV = max(dot(N, V), 0.0);
		float GeomV = GeometrySchlickGGX(NdotV, roughness);

		// Fresnel reflectance ratio.
		vec3 F0 = mix(vec3(0.08), albedo, metallic);

		// Ambient lighting.
		vec3 ambient;
#ifdef USE_ENVIRONMENT_MAP
		{
			vec3 kS = Fresnel(NdotV, F0, roughness);
			vec3 kD = 1.0 - kS;

			ambient = texture(sIrradiance, N).rgb * albedo * kD * ao;

			float numTextureLevels = 11.0; // Based on the size of the skyboxes.
			ambient += kS * textureLod(sEnvironment, reflect(-V, N), roughness * numTextureLevels).rgb * ao;
		}
#else
		ambient = vec3(0.01) * albedo;
#endif

		// Accumulate lighting.
		vec3 Lo = vec3(0.0);
		vec3 color;
#ifdef USE_ADVANCED_LIGHTING
		{
			float distance = length(Light1.Position - pos);
			float attenuation = 1.0 / (0.75 + Light1.AttenuationLinear * distance + Light1.AttenuationQuadratic * distance * distance);
			vec3 radiance = Light1.Color * attenuation;

			vec3 L = (Light1.Position - pos) / distance;
			vec3 H = normalize(V + L);
			float NdotL = max(dot(N, L), 0.0);

			float NDF = DistributionGGX(N, H, roughness);
			float GeomL = GeometrySchlickGGX(NdotL, roughness);
			vec3 F = Fresnel(max(dot(H, V), 0.0), F0, roughness);

			// Resolve BRDF.
			vec3 specular = 
				(NDF * GeomV * GeomL * F) /
				(4.0 * NdotV * NdotL + 0.001);

			vec3 kS = F;
			vec3 kD = (1.0 - kS) * (1.0 - metallic);

			Lo += (((kD * albedo * ao) / M_PI) + specular) * NdotL * radiance;
		}

		color = Lo + ambient;
#else
		Lo = albedo * compute_light(Light1, N, pos);
		color = Lo + ambient;
#endif

		// HDR -> LDR
		color = color / (color + 1.0);

		outColor = color;
	}
}
