#pragma once
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Resource/ConfigTable.h>

using namespace Jwl;

class Game
{
public:
	Game(ConfigTable &config);

	bool Init();

	void Update();
	void Draw();

	/* Rendering */
	RenderTarget::Ptr shadowMap;
	RenderPass shadowRenderPass;
	RenderPass mainRenderPass;

	/* Shaders */
	Shader::Ptr lambertShadow = Shader::MakeNew();
	UniformHandle<mat4> worldToShadow;
	
	/* Scene */
	Entity::Ptr shadowCamera = Entity::MakeNew();
	Entity::Ptr mainCamera = Entity::MakeNew();
	Entity::Ptr rootNode = Entity::MakeNew();
	Entity::Ptr ground = Entity::MakeNew();
	Entity::Ptr shack = Entity::MakeNew();

	/* Assets */
	ConfigTable &config;

private:
};
