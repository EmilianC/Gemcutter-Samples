#pragma once
#include "Flock.h"

#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Rendering/RenderPass.h>
#include <gemcutter/Resource/ConfigTable.h>

using namespace gem;

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
	Entity::Ptr rootNode = Entity::MakeNewRoot();
	Flock flock;
	static constexpr unsigned NumBirds = 180;

	/* Assets */
	ConfigTable& config;
};
