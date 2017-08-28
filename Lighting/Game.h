#pragma once
#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Rendering/RenderTarget.h>
#include <Jewel3D/Resource/ConfigTable.h>
#include <Jewel3D/Resource/Shader.h>

using namespace Jwl;

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
	Entity::Ptr rootEntity = Entity::MakeNew();
	Entity::Ptr ground = Entity::MakeNew();
	Entity::Ptr monkey = Entity::MakeNew();
	Entity::Ptr rootLight = Entity::MakeNew();
	Entity::Ptr light1 = Entity::MakeNew();
	Entity::Ptr light2;
	Entity::Ptr light3;

	float elapsed = 0.0f;
	
	/* Assets */
	ConfigTable& config;
};
