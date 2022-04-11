#pragma once
#include <gemcutter/Application/Application.h>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/RenderPass.h>
#include <gemcutter/Resource/ConfigTable.h>
#include <gemcutter/Resource/Font.h>
#include <vector>

using namespace gem;

class Game
{
public:
	Game(ConfigTable& config);

	bool Init();

	void Update();
	void Draw();

	void UpdateFonts();

	/* Events */
	Listener<Resize> onResized;
	Listener<KeyPressed> onKeyPressed;

	/* Rendering */
	RenderPass mainRenderPass;
	RenderPass UIRenderPass;
	
	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr cameraUI = Entity::MakeNew();
	Entity::Ptr rootEntity = Entity::MakeNewRoot();
	Entity::Ptr rootUiEntity = Entity::MakeNewRoot();

	Entity::Ptr helloWorldText = Entity::MakeNew();
	Entity::Ptr instructionText = Entity::MakeNew();

	/* Assets */
	ConfigTable& config;

	std::vector<Font::Ptr> fonts;
	unsigned currentFont = 0;
	bool direction = false;
	float angle = 0.0f;
};
