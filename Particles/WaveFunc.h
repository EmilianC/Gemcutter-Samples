#pragma once
#include <Jewel3D/Rendering/ParticleEmitter.h>

using namespace Jwl;

//- Custom particle functor to oscillate particles back and forth as they move upwards.
class WaveFunc : public ParticleFunctor, public Shareable<WaveFunc>
{
public:
	WaveFunc() = default;
	WaveFunc(float frequency, float intensity);

	void Update(ParticleBuffer &particles, ParticleEmitter &emitter, float deltaTime) override;

	float Frequency = 3.0f;
	float Intensity = 1.0f;
};
