#pragma once
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Resource/Shader.h>
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
	Entity::Ptr rootEntity = Entity::MakeNew();
	Entity::Ptr monkey = Entity::MakeNew();
	Entity::Ptr orbParent = Entity::MakeNew();
	Entity::Ptr orb = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;

	Shader::Ptr passThroughProgram;
};
