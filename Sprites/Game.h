#pragma once
#include <gemcutter/Application/Application.h>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Input/Input.h>
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
	Listener<KeyPressed> onKeyPressed;

	/* Rendering */
	RenderPass mainRenderPass;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNewRoot();

	/* Assets */
	ConfigTable& config;
};
