#include "Boid.h"

Boid::Boid(Entity &owner)
	: Component(owner)
{
}

REFLECT_COMPONENT_SIMPLE(Boid);
