#pragma once
#include <gemcutter/Application/Event.h>
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
	Listener<KeyPressed> onKeyPressed;

	/* Rendering */
	RenderPass mainRenderPass;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr smoke = Entity::MakeNew();
	Entity::Ptr fire = Entity::MakeNew();
	Entity::Ptr sparkle = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNewRoot();

	/* Assets */
	ConfigTable& config;
};
