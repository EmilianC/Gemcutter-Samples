#pragma once
#include "Flock.h"

#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Resource/ConfigTable.h>

using namespace Jwl;

class Game
{
public:
	Game(ConfigTable& config);

	bool Init();

	void Update();
	void Draw();

	/* Rendering */
	RenderPass mainRenderPass;
	
	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr rootNode = Entity::MakeNew();
	Flock flock;
	static constexpr unsigned NumBirds = 180;

	/* Assets */
	ConfigTable& config;
};
