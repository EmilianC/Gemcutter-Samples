#pragma once
#include <Jewel3D/Rendering/ParticleEmitter.h>

using namespace Jwl;

//- Custom particle functor to create unique velocity behaviour
class VelocityFunc : public ParticleFunctor, public Shareable<VelocityFunc>
{
public:
	VelocityFunc() = default;
	VelocityFunc(vec3 direction, float velocityCap, float acceleration);

	virtual void Update(ParticleBuffer &particles, ParticleEmitter &emitter, float deltaTime) override;

	vec3 Direction = vec3(0.0f, 1.0f, 0.0f);
	float VelocityCap = 5.0f;
	float Acceleration = 2.0f;
};

REFLECT_SHAREABLE(VelocityFunc)
REFLECT(VelocityFunc) < ParticleFunctor >,
	MEMBERS <
		REF_MEMBER(Direction)<>,
		REF_MEMBER(VelocityCap)<>,
		REF_MEMBER(Acceleration)<>
	>
REF_END;
