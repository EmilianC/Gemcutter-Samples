#pragma once
#include <gemcutter/Application/Application.h>
#include <gemcutter/Entity/Entity.h>
#include <gemcutter/GUI/Widget.h>
#include <gemcutter/Rendering/RenderPass.h>
#include <gemcutter/Resource/ConfigTable.h>

using namespace gem;

class Game
{
public:
	Game(ConfigTable& config);

	bool Init();
	void CreateLeftPanel();
	void CreateRightPanel();

	void Update();
	void Draw();

	/* Rendering */
	RenderPass mainRenderPass;

	/* Events */
	Listener<Resize> onResized;

	/* Scene */
	Entity::Ptr camera = Entity::MakeNew();
	Entity::Ptr screen = Entity::MakeNewRoot();
	Widget* root = nullptr;

	/* Assets */
	ConfigTable& config;

	bool wiggleX = false;
	bool wiggleY = false;
	float elapsedX = 0.0f;
	float elapsedY = 0.0f;
};
