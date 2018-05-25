#pragma once
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Input/Input.h>
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

	/* Events */
	Listener<KeyPressed> onKeyPressed;

	/* Rendering */
	RenderPass mainRenderPass;
	
	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr smoke = Entity::MakeNew();
	Entity::Ptr fire = Entity::MakeNew();
	Entity::Ptr sparkle = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;
};
