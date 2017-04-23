#include "Flock.h"
#include "Boid.h"

#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Utilities/Random.h>

#define MIN_X_BOUND XBounds.x
#define MAX_X_BOUND XBounds.y
#define MIN_Y_BOUND YBounds.x
#define MAX_Y_BOUND YBounds.y
#define MIN_Z_BOUND ZBounds.x
#define MAX_Z_BOUND ZBounds.y

void Flock::RandomlyPlaceBoids()
{
	ASSERT(MIN_X_BOUND < MAX_X_BOUND, "Invalid bounds for Boid flocking volume.");
	ASSERT(MIN_Y_BOUND < MAX_Y_BOUND, "Invalid bounds for Boid flocking volume.");
	ASSERT(MIN_Z_BOUND < MAX_Z_BOUND, "Invalid bounds for Boid flocking volume.");

	for (auto& boid : All<Boid>())
	{
		// Randomize velocity.
		boid.Velocity.x = RandomRangef(-1.0f, 1.0f);
		boid.Velocity.y = RandomRangef(-1.0f, 1.0f);
		boid.Velocity.z = RandomRangef(-1.0f, 1.0f);
		boid.Velocity.Normalize();
		boid.Velocity *= RandomRangef(0.0f, MaxVelocity);

		// Randomize position.
		boid.owner.position.x = RandomRangef(MIN_X_BOUND, MAX_X_BOUND);
		boid.owner.position.y = RandomRangef(MIN_Y_BOUND, MAX_Y_BOUND);
		boid.owner.position.z = RandomRangef(MIN_Z_BOUND, MAX_Z_BOUND);
	}
}

void Flock::Update(float deltaTime)
{
	vec3 averagePosition;
	vec3 averageVelocity;
	vec3 alignmentForce;

	/* Find average position and velocity */
	unsigned numBoids = 0;
	for (auto& boid : All<Boid>())
	{
		averagePosition += boid.owner.position;
		averageVelocity += boid.Velocity;
		numBoids++;
	}

	if (numBoids == 0)
	{
		return;
	}

	// Normalize.
	averagePosition /= static_cast<float>(numBoids);
	averageVelocity /= static_cast<float>(numBoids);

	// This keeps boids moving in the same general direction.
	alignmentForce = averageVelocity * InertiaFactor;

	/* Update flock */
	for (auto& boid : All<Boid>())
	{
		vec3 acceleration;
		vec3 pullForce;
		vec3 proximityForce;
		vec3 localCenter;

		/* Calculate local center */
		unsigned localCount = 0;
		for (auto& other : All<Boid>())
		{
			// Skip comparison of boid with itself.
			if (&boid == &other)
			{
				continue;
			}

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

		pullForce = (averagePosition - boid.owner.position) * PullFactor;

		acceleration = alignmentForce + pullForce + proximityForce;
		// Clamp position to bounds.
		if (boid.owner.position.x < MIN_X_BOUND + EdgeBuffer)
		{
			acceleration.x += EdgeForce;
		}
		else if (boid.owner.position.x > MAX_X_BOUND - EdgeBuffer)
		{
			acceleration.x -= EdgeForce;
		}

		if (boid.owner.position.y < MIN_Y_BOUND + EdgeBuffer)
		{
			acceleration.y += EdgeForce;
		}
		else if (boid.owner.position.y > MAX_Y_BOUND - EdgeBuffer)
		{
			acceleration.y -= EdgeForce;
		}

		if (boid.owner.position.z < MIN_Z_BOUND + EdgeBuffer)
		{
			acceleration.z += EdgeForce;
		}
		else if (boid.owner.position.z > MAX_Z_BOUND - EdgeBuffer)
		{
			acceleration.z -= EdgeForce;
		}

		/* Step */
		boid.Velocity += acceleration * deltaTime;
		// Clamp velocity.
		if (boid.Velocity.Length() > MaxVelocity)
		{
			boid.Velocity.Normalize();
			boid.Velocity *= MaxVelocity;
		}

		boid.owner.position += boid.Velocity * deltaTime;

		/* Orient boid with velocity */
		vec3 right, up, forward;

		forward = boid.Velocity;
		forward.Normalize();

		up = vec3(0.0f, 0.0f, 1.0f);

		right = Cross(forward, up);
		right.Normalize();

		boid.owner.rotation = mat3(right, up, -forward);
	}
}
