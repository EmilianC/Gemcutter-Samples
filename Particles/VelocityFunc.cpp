#include "VelocityFunc.h"

VelocityFunc::VelocityFunc(vec3 direction, float velocityCap, float acceleration)
	: Direction(direction)
	, VelocityCap(velocityCap)
	, Acceleration(acceleration)
{
}

void VelocityFunc::Update(ParticleBuffer& particles, ParticleEmitter &emitter, float deltaTime)
{
	for (unsigned i = 0; i < emitter.GetNumAliveParticles(); i++)
	{
		if (LengthSquared(particles.velocities[i]) > VelocityCap)
		{
			particles.velocities[i] = vec3::Zero;
		}
		else
		{
			particles.velocities[i] += Direction * Acceleration * deltaTime;
		}
	}
}
