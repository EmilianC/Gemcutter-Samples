#pragma once
#include <Jewel3D/Rendering/ParticleEmitter.h>

using namespace Jwl;

//- Custom particle functor to create unique velocity behaviour.
class VelocityFunc : public ParticleFunctor, public Shareable<VelocityFunc>
{
public:
	VelocityFunc() = default;
	VelocityFunc(vec3 direction, float velocityCap, float acceleration);

	void Update(ParticleBuffer &particles, ParticleEmitter &emitter, float deltaTime) override;

	vec3 Direction = vec3(0.0f, 1.0f, 0.0f);
	float VelocityCap = 5.0f;
	float Acceleration = 2.0f;
};
