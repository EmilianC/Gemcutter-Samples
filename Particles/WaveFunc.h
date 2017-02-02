#pragma once
#include <Jewel3D/Entity/Components/ParticleEmitter.h>

using namespace Jwl;

//- Custom particle functor to oscillate particles back and forth as they move upwards
class WaveFunc : public ParticleFunctor, public Shareable<WaveFunc>
{
public:
	WaveFunc() = default;
	WaveFunc(float frequency, float intensity);

	virtual void Update(ParticleBuffer &particles, ParticleEmitter &emitter, float deltaTime) final override;

	float Frequency = 3.0f;
	float Intensity = 1.0f;
};

REFLECT_SHAREABLE(WaveFunc)
REFLECT(WaveFunc) < ParticleFunctor >,
	MEMBERS <
		REF_MEMBER(Frequency)<>,
		REF_MEMBER(Intensity)<>
	>
REF_END;
