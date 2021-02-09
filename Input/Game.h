#pragma once
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
	Entity::Ptr rootEntity = Entity::MakeNewRoot();

	/* Assets */
	ConfigTable& config;

	bool canFullScreen		 = true;
	bool canChangeBorder	 = true;
	bool canResize			 = true;
	bool canChangeResolution = true;
};
