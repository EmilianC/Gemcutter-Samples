#include "WaveFunc.h"

WaveFunc::WaveFunc(float frequency, float intensity)
	: Frequency(frequency)
	, Intensity(intensity)
{
}

void WaveFunc::Update(ParticleBuffer& particles, ParticleEmitter &emitter, float deltaTime)
{
	float intensity = Intensity * deltaTime;

	for (unsigned i = 0; i < emitter.GetNumAliveParticles(); i++)
	{
		particles.positions[i].x += std::sin(particles.positions[i].y * Frequency) * intensity;
	}
}
