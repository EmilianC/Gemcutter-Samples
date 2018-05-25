#include "Flock.h"
#include "Boid.h"

#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Utilities/Random.h>

void Flock::RandomlyPlaceBoids()
{
	for (auto& boid : All<Boid>())
	{
		// Randomize position and velocity.
		boid.owner.position.x = XBounds.Random();
		boid.owner.position.y = YBounds.Random();
		boid.owner.position.z = ZBounds.Random();
		boid.Velocity = RandomDirection() * RandomRange(0.0f, MaxVelocity);
	}
}

void Flock::Update(float deltaTime)
{
	auto& boids = GetComponentIndex<Boid>();
	if (boids.empty())
		return;

	vec3 averagePosition;
	vec3 averageVelocity;

	/* Find average position and velocity */
	for (auto& boid : All<Boid>())
	{
		averagePosition += boid.owner.position;
		averageVelocity += boid.Velocity;
	}

	// Normalize.
	averagePosition /= static_cast<float>(boids.size());
	averageVelocity /= static_cast<float>(boids.size());

	// This keeps boids moving in the same general direction.
	vec3 alignmentForce = averageVelocity * InertiaFactor;

	/* Update flock */
	for (unsigned i = 0; i < boids.size(); i++)
	{
		auto& boid = *static_cast<Boid*>(boids[i]);

		vec3 proximityForce;
		vec3 localCenter;

		/* Calculate local center */
		unsigned localCount = 0;
		for (unsigned j = i + 1; j < boids.size(); j++)
		{
			auto& other = *static_cast<Boid*>(boids[j]);

			// If the other node if within local range, it contributes to the local center.
			if ((boid.owner.position - other.owner.position).LengthSquared() < ProximityRange)
			{
				localCenter += other.owner.position;
				localCount++;
			}
		}

		/* Compute forces */
		// Normalize and avoid divide by zero error.
		if (localCount != 0)
		{
			localCenter /= static_cast<float>(localCount);
			proximityForce = (boid.owner.position - localCenter) * ProximityFactor;
		}

		// Force away from and boundaries.
		vec3 acceleration = alignmentForce + proximityForce + (averagePosition - boid.owner.position) * PullFactor;
		if (boid.owner.position.x < XBounds.min)
		{
			acceleration.x += EdgeForce;
		}
		else if (boid.owner.position.x > XBounds.max)
		{
			acceleration.x -= EdgeForce;
		}

		if (boid.owner.position.y < YBounds.min)
		{
			acceleration.y += EdgeForce;
		}
		else if (boid.owner.position.y > YBounds.max)
		{
			acceleration.y -= EdgeForce;
		}

		if (boid.owner.position.z < ZBounds.min)
		{
			acceleration.z += EdgeForce;
		}
		else if (boid.owner.position.z > ZBounds.max)
		{
			acceleration.z -= EdgeForce;
		}

		/* Step */
		boid.Velocity += acceleration * deltaTime;
		boid.Velocity.ClampLength(MaxVelocity);
		boid.owner.position += boid.Velocity * deltaTime;

		/* Orient boid with velocity */
		vec3 up = vec3(0.0f, 0.0f, 1.0f);
		vec3 forward = boid.Velocity.GetNormalized();
		vec3 right = Cross(forward, up);

		boid.owner.rotation = mat3(right, up, -forward);
	}
}
