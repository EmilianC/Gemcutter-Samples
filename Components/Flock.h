#pragma once
#include <Jewel3D/Utilities/Random.h>

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
	Range XBounds{ -110, 110 };
	Range YBounds{ -110, 110 };
	Range ZBounds{ -110, 110 };
	float PullFactor = 0.33f;
	float InertiaFactor = 0.3f;
	float ProximityFactor = 5.0f;
	float ProximityRange = 900.0f; //Squared value
	float MaxVelocity = 75.0f;
	float EdgeForce = 100.0f;
};
