#pragma once
#include <gemcutter/Application/Application.h>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Rendering/RenderPass.h>
#include <gemcutter/Resource/ConfigTable.h>

using namespace gem;

class Game
{
public:
	Game(ConfigTable &config);

	bool Init();

	void Update();
	void Draw();

	/* Rendering */
	RenderTarget::Ptr shadowMap = RenderTarget::MakeNew();
	RenderPass shadowRenderPass;
	RenderPass mainRenderPass;

	/* Events */
	Listener<Resize> onResized;

	/* Shaders */
	Shader::Ptr lambertShadow = Shader::MakeNew();
	UniformHandle<mat4> worldToShadow;

	/* Scene */
	Entity::Ptr shadowCamera = Entity::MakeNew();
	Entity::Ptr mainCamera = Entity::MakeNew();
	Entity::Ptr rootNode = Entity::MakeNewRoot();
	Entity::Ptr ground = Entity::MakeNew();
	Entity::Ptr shack = Entity::MakeNew();

	/* Assets */
	ConfigTable &config;
};
