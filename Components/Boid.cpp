#include "Boid.h"

Boid::Boid(Entity &owner)
	: Component(owner)
{
}

REFLECT_COMPONENT(Boid, gem::ComponentBase) REF_END;
