#pragma once
#include <Jewel3D/Entity/Entity.h>
#include <Jewel3D/Rendering/RenderPass.h>
#include <Jewel3D/Resource/ConfigTable.h>
#include <Jewel3D/Resource/Font.h>
#include <vector>

using namespace Jwl;

class Game
{
public:
	Game(ConfigTable& config);

	bool Init();

	void Update();
	void Draw();

	void UpdateFonts();

	/* Rendering */
	RenderPass mainRenderPass;
	RenderPass UIRenderPass;
	
	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr cameraUI = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNew();
	Entity::Ptr rootUiEntity = Entity::MakeNew();

	Entity::Ptr helloWorldText = Entity::MakeNew();
	Entity::Ptr instructionText = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;

	std::vector<Font::Ptr> fonts;
	unsigned currentFont = 0;
	bool canChangeFont = true;
	bool direction = false;
	float angle = 0.0f;
};
