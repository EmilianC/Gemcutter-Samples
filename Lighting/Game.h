#pragma once
#include <gemcutter/Application/Application.h>
#include <gemcutter/Application/Event.h>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/RenderPass.h>
#include <gemcutter/Rendering/RenderTarget.h>
#include <gemcutter/Resource/ConfigTable.h>
#include <gemcutter/Resource/Shader.h>
#include <vector>

using namespace gem;

class Game
{
public:
	Game(ConfigTable& config);

	void CreateGround();
	bool Init();

	void UpdateSkybox();
	void UpdateSettings();

	void Update();
	void Draw();

	/* Events */
	Listener<Resize> onResized;
	Listener<KeyPressed> onKeyPressed;

	/* Rendering */
	RenderPass mainRenderPass;
	RenderPass UIRenderPass;
	RenderTarget::Ptr frameBuffer;
	RenderTarget::Ptr frameBufferResolve;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr cameraParent = Entity::MakeNewRoot();
	Entity::Ptr cameraUI = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNewRoot();
	Entity::Ptr rootLight = Entity::MakeNewRoot();
	Entity::Ptr skybox = Entity::MakeNew();
	Entity::Ptr ball = Entity::MakeNew();
	Entity::Ptr light = Entity::MakeNew();
	Entity::Ptr text = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;
	std::vector<Texture::Ptr> environmentMaps;
	std::vector<Texture::Ptr> irradianceMaps;

	unsigned MSAA_Level = 1;
	float angleY = -90.0f;
	float angleX = 35.0f;

	bool useNormalMap = true;
	bool useHeightMap = true;
	bool useEnvironmentMap = true;
	bool useAdvancedLighting = true;
	unsigned currentSkybox = 0;
};
