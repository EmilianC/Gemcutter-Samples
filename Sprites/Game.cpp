#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/Material.h>
#include <Jewel3D/Entity/Components/Sprite.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load Assets.
	auto dragonTexture = Load<Texture>("Textures/Dragon.png", TextureFilterMode::Point);
	auto dogeTexture = Load<Texture>("Textures/Doge.png");
	auto faceTexture = Load<Texture>("Textures/Face.png");
	auto shader = Load<Shader>("Shaders/Default/Sprite.shader");
	if (!dragonTexture || !dogeTexture || !faceTexture || !shader) return false;

	auto dragon = rootEntity->CreateChild();
	auto doge = rootEntity->CreateChild();
	auto face = rootEntity->CreateChild();

	dragon->Add<Material>(shader, dragonTexture).SetBlendMode(BlendFunc::CutOut);
	dragon->Add<Sprite>().SetCenteredX(true);
	dragon->Get<Sprite>().SetCenteredY(true);
	dragon->scale = vec3(20, 20, 1);

	doge->Add<Material>(shader, dogeTexture).SetBlendMode(BlendFunc::CutOut);
	doge->Add<Sprite>();
	doge->scale = vec3(10, 10, 1);
	doge->position.x = 22.0f;

	face->Add<Material>(shader, faceTexture).SetBlendMode(BlendFunc::CutOut);
	face->Add<Sprite>().SetCenteredX(true);
	face->Get<Sprite>().SetBillBoarded(true);
	face->scale = vec3(10, 10, 1);
	face->position.x = -25.0f;

	rootEntity->AddChild(dragon);
	rootEntity->AddChild(doge);
	rootEntity->AddChild(face);

	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 20.0f, 50.0f), vec3(0.0f));

	// Setup up renderer.
	mainRenderPass.SetCamera(camera);

	// Setup background color to cornflower blue.
	SetClearColor(0.35f, 0.7f, 0.9f, 0.0f);

	// Keep sprites rendering even when facing away from the camera.
	for (auto& mat : All<Material>())
	{
		mat.SetCullMode(CullFunc::None);
	}

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}
	
	rootEntity->RotateY(Application.GetDeltaTime() * 25.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
