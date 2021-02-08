#pragma once
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Math/Vector.h>

using namespace gem;

class Boid : public Component<Boid>
{
public:
	Boid(Entity &owner);

	vec3 Velocity;
};
