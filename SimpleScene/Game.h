#pragma once
#include <gemcutter/Application/Application.h>
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

	/* Events */
	Listener<Resize> onResized;

	/* Rendering */
	RenderPass mainRenderPass;
	
	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr object = Entity::MakeNew();

	/* Assets */
	//- The config contains all the properties found in Settings.cfg.
	ConfigTable& config;
};
