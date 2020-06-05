#pragma once
#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Rendering/RenderTarget.h>
#include <Jewel3D/Resource/ConfigTable.h>
#include <Jewel3D/Resource/Shader.h>
#include <vector>

using namespace Jwl;

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
	RenderPass transparentRenderPass;
	RenderPass UIRenderPass;
	RenderTarget::Ptr frameBuffer;
	RenderTarget::Ptr frameBufferResolve;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr cameraParent = Entity::MakeNewRoot();
	Entity::Ptr cameraUI = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNewRoot();
	Entity::Ptr rootLight = Entity::MakeNewRoot();
	Entity::Ptr ball = Entity::MakeNew();
	Entity::Ptr light = Entity::MakeNew();
	Entity::Ptr text = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;
	Shader::Ptr lightingShader;
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
