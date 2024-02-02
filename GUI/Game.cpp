#include "Game.h"

#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/GUI/Button.h>
#include <gemcutter/GUI/Image.h>
#include <gemcutter/GUI/Label.h>
#include <gemcutter/GUI/Screen.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Math/Math.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Renderable.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Font.h>

namespace
{
	Button& CreateButton(Widget& Parent, std::string_view label)
	{
		auto& button = Parent.CreateChild<Button>(
			Load<Font>("Fonts/editundo"), label,
			Load<Texture>("Textures/GUI/ButtonIdle"),
			Load<Texture>("Textures/GUI/ButtonHover"),
			Load<Texture>("Textures/GUI/ButtonPressed")
		);

		// Set a margin on the text.
		button.GetLabel().left.offset = 15.0f;
		button.GetLabel().right.offset = 15.0f;

		button.pressedSound = Load<Sound>("Sounds/Snap");
		return button;
	}
}

Game::Game(ConfigTable& _config)
	: config(_config)
{
	onResized = [this](const Resize& event) {
		camera->Get<Camera>().SetOrthographic(Application.GetScreenViewport(), -1.0f, 1.0f);
	};
}

bool Game::Init()
{
	// Setup Camera.
	camera->Add<Camera>().SetOrthographic(Application.GetScreenViewport(), -1.0f, 1.0f);

	// Setup up renderer.
	mainRenderPass.SetCamera(camera);

	// Setup widgets.
	screen->Add<Screen>();

	root = &screen->Get<Widget>().CreateChild<Widget>();
	root->CreateChild<Image>(Load<Material>("Materials/GUI/PanelDark"));

	CreateLeftPanel();
	CreateRightPanel();

	return true;
}

void Game::CreateLeftPanel()
{
	/// Background ///
	auto& panelLeft = root->CreateChild<Image>(Load<Material>("Materials/GUI/Panel"));
	panelLeft.top.anchor    = 0.10f;
	panelLeft.right.anchor  = 0.45f;
	panelLeft.left.anchor   = 0.10f;
	panelLeft.bottom.anchor = 0.10f;

	/// Title ///
	auto& label = panelLeft.CreateChild<Label>(
		Load<Font>("Fonts/editundo"), "Window Controls",
		Load<Material>("Engine/Materials/GUI/Label")
	);
	label.left.anchor   = 0.50f;
	label.right.anchor  = 0.50f;
	label.bottom.anchor = 0.90f;
	label.left.offset   = -180.0f;
	label.right.offset  = -180.0f;
	label.owner.Get<Renderable>().buffers[0].SetUniform<vec3>("Color", vec3(0));

	/// Buttons ///
	{
		auto& button = CreateButton(panelLeft, "800 x 600");
		button.top.anchor    = 0.15f;
		button.right.anchor  = 0.55f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.75f;

		button.onClick.AddOwned([this](Button&) { Application.SetResolution(800, 600); });
	}

	{
		auto& button = CreateButton(panelLeft, "1280 x 720");
		button.top.anchor    = 0.15f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.55f;
		button.bottom.anchor = 0.75f;

		button.onClick.AddOwned([this](Button&) { Application.SetResolution(1280, 720); });
	}

	{
		auto& button = CreateButton(panelLeft, "1600 x 900");
		button.top.anchor    = 0.30f;
		button.right.anchor  = 0.55f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.60f;

		button.onClick.AddOwned([this](Button&) { Application.SetResolution(1600, 900); });
	}

	{
		auto& button = CreateButton(panelLeft, "1920 x 1080");
		button.top.anchor    = 0.30f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.55f;
		button.bottom.anchor = 0.60f;

		button.onClick.AddOwned([this](Button&) { Application.SetResolution(1920, 1080); });
	}

	{
		auto& button = CreateButton(panelLeft, "Bordered");
		button.top.anchor    = 0.45f;
		button.right.anchor  = 0.55f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.30f;

		button.onClick.AddOwned([this](Button&) { Application.SetBordered(!Application.IsBordered()); });
	}

	{
		auto& button = CreateButton(panelLeft, "Resizable");
		button.top.anchor    = 0.45f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.55f;
		button.bottom.anchor = 0.30f;

		button.onClick.AddOwned([this](Button&) { Application.SetResizable(!Application.IsResizable()); });
	}

	{
		auto& button = CreateButton(panelLeft, "Fullscreen");
		button.top.anchor    = 0.75f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.10f;

		button.onClick.AddOwned([this](Button&) { Application.SetFullscreen(!Application.IsFullscreen()); });
	}
}

void Game::CreateRightPanel()
{
	/// Background ///
	auto& panelRight = root->CreateChild<Image>(Load<Material>("Materials/GUI/Panel"));
	panelRight.top.anchor    = 0.10f;
	panelRight.right.anchor  = 0.10f;
	panelRight.left.anchor   = 0.60f;
	panelRight.bottom.anchor = 0.10f;

	/// Title ///
	auto& label = panelRight.CreateChild<Label>(
		Load<Font>("Fonts/editundo"), "Wiggle Controls",
		Load<Material>("Engine/Materials/GUI/Label")
	);
	label.left.anchor   = 0.50f;
	label.right.anchor  = 0.50f;
	label.bottom.anchor = 0.90f;
	label.left.offset   = -180.0f;
	label.right.offset  = -180.0f;
	label.owner.Get<Renderable>().buffers[0].SetUniform<vec3>("Color", vec3(0));

	/// Buttons ///
	{
		auto& button = CreateButton(panelRight, "Horizontal");
		button.top.anchor    = 0.15f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.60f;

		button.onClick.AddOwned([this](Button&) { wiggleX = !wiggleX; });
	}

	{
		auto& button = CreateButton(panelRight, "Vertical");
		button.top.anchor    = 0.45f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.30f;

		button.onClick.AddOwned([this](Button&) { wiggleY = !wiggleY; });
	}

	{
		auto& button = CreateButton(panelRight, "Reset");
		button.top.anchor    = 0.75f;
		button.right.anchor  = 0.10f;
		button.left.anchor   = 0.10f;
		button.bottom.anchor = 0.10f;

		button.onClick.AddOwned([this](Button&) {
			wiggleX = false;
			wiggleY = false;
			elapsedX = 0.0f;
			elapsedY = 0.0f;
		});
	}
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	float deltaTime = Application.GetDeltaTime();
	if (wiggleX) elapsedX += deltaTime;
	if (wiggleY) elapsedY += deltaTime;

	root->top.anchor    = sin(elapsedY) / 10.0f;
	root->bottom.anchor = sin(elapsedY) / 10.0f;
	root->left.anchor   = sin(elapsedX + ToRadian(180.0f)) / 10.0f;
	root->right.anchor  = sin(elapsedX + ToRadian(180.0f)) / 10.0f;

	// Engine systems and components, including widget layouts, are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Bind();
	mainRenderPass.RenderRoot(*screen);
}
