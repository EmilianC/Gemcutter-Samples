#pragma once
#include <Jewel3D/Math\Vector.h>

using namespace Jwl;

//- Simulates a flock of boids using the standard forces:
//	Separation, Alignment, and Cohesion
class Flock
{
public:
	//- Randomly distributes the boids inside the valid volume
	void RandomlyPlaceBoids();
	//- Steps the simulation
	//- All entities with Boid Components are updated
	void Update(float deltaTime);

	//- Simulation parameters
	vec2 XBounds{ -150, 150 };
	vec2 YBounds{ -150, 150 };
	vec2 ZBounds{ -150, 150 };
	float PullFactor = 0.33f;
	float InertiaFactor = 0.3f;
	float ProximityFactor = 5.0f;
	float ProximityRange = 900.0f; //Squared value
	float MaxVelocity = 75.0f;
	float EdgeBuffer = 40.0f;
	float EdgeForce = 100.0f;
};
