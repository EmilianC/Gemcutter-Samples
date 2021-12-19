#include "Game.h"

#include <gemcutter/Application/Application.h>
#include <gemcutter/Application/Logging.h>
#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Rendering/Sprite.h>
#include <gemcutter/Resource/Material.h>

class BillBoard : public Tag<BillBoard> {};

Game::Game(ConfigTable& _config)
	: config(_config)
{
	onKeyPressed = [](auto& e)
	{
		if (e.key == Key::Space)
		{
			for (auto& entity : With<BillBoard>())
			{
				auto& sprite = entity.Get<Sprite>();
				sprite.SetBillBoarded(!sprite.GetBillBoarded());
			}
		}
	};
}

bool Game::Init()
{
	// Load Assets.
	auto material = Load<Material>("Materials/Logo");

	rootEntity->position.y = 2.0f;
	auto sprite1 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite2 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite3 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite4 = rootEntity->Get<Hierarchy>().CreateChild();

	sprite1->Add<Sprite>(Alignment::Center, false, material);
	sprite1->scale = vec3(16, 16, 1);

	sprite2->Add<Sprite>(Alignment::Center, false, material);
	sprite2->scale = vec3(16, 16, 1);
	sprite2->RotateY(180.0f);

	sprite3->Tag<BillBoard>();
	sprite3->Add<Sprite>(Alignment::Center, true, material);
	sprite3->scale = vec3(16, 16, 1);
	sprite3->position.x = 22.0f;

	sprite4->Tag<BillBoard>();
	sprite4->Add<Sprite>(Alignment::Center, true, material);
	sprite4->scale = vec3(16, 16, 1);
	sprite4->position.x = -22.0f;

	for (auto& sprite : All<Sprite>())
	{
		sprite.variants.Define("GEM_CUTOUT");
	}

	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 15.0f, 50.0f), vec3(0.0f));

	// Setup up renderer.
	mainRenderPass.SetCamera(camera);

	// Setup background color to cornflower blue.
	SetClearColor(0.35f, 0.7f, 0.9f, 0.0f);

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	rootEntity->RotateY(Application.GetDeltaTime() * 35.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Bind();
	mainRenderPass.RenderRoot(*rootEntity);
}
