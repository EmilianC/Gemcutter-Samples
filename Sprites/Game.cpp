#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Entity/Hierarchy.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Rendering/Sprite.h>

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
	auto nabber_n = Load<Texture>("Textures/nabber_n");
	auto nabber_e = Load<Texture>("Textures/nabber_e");
	auto nabber_s = Load<Texture>("Textures/nabber_s");
	auto nabber_w = Load<Texture>("Textures/nabber_w");
	auto shader = Load<Shader>("Shaders/Default/Sprite");
	if (!nabber_n || !nabber_e || !nabber_s || !nabber_w || !shader)
		return false;

	auto sprite1 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite2 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite3 = rootEntity->Get<Hierarchy>().CreateChild();
	auto sprite4 = rootEntity->Get<Hierarchy>().CreateChild();

	sprite1->Add<Material>(shader, nabber_s, BlendFunc::CutOut);
	sprite1->Add<Sprite>(Alignment::Center);
	sprite1->scale = vec3(16, 20, 1);

	sprite2->Add<Material>(shader, nabber_n, BlendFunc::CutOut);
	sprite2->Add<Sprite>(Alignment::Center);
	sprite2->scale = vec3(16, 20, 1);
	sprite2->RotateY(180.0f);

	sprite3->Tag<BillBoard>();
	sprite3->Add<Material>(shader, nabber_e, BlendFunc::CutOut, DepthFunc::Normal, CullFunc::None);
	sprite3->Add<Sprite>(Alignment::Center, true);
	sprite3->scale = vec3(16, 20, 1);
	sprite3->position.x = 22.0f;

	sprite4->Tag<BillBoard>();
	sprite4->Add<Material>(shader, nabber_w, BlendFunc::CutOut, DepthFunc::Normal, CullFunc::None);
	sprite4->Add<Sprite>(Alignment::Center, true);
	sprite4->scale = vec3(16, 20, 1);
	sprite4->position.x = -22.0f;

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

	mainRenderPass.Render(*rootEntity);
}
