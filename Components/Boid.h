#pragma once
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Math/Vector.h>

using namespace Jwl;

class Boid : public Component<Boid>
{
public:
	Boid(Entity &owner);

	vec3 Velocity;
};

REFLECT(Boid) < Component >,
	MEMBERS <
		REF_MEMBER(Velocity)<>
	>
REF_END;
