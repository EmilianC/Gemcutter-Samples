#include "Game.h"

#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Math/Math.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Mesh.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Material.h>
#include <gemcutter/Resource/Model.h>
#include <gemcutter/Resource/Shader.h>
#include <gemcutter/Resource/Sound.h>
#include <gemcutter/Resource/Texture.h>
#include <gemcutter/Sound/SoundListener.h>
#include <gemcutter/Sound/SoundSource.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	onResized = [this](auto& e)
	{
		camera->Get<Camera>().SetAspectRatio(e.GetAspectRatio());
	};
}

bool Game::Init()
{
	// Load Orb.
	auto model = Load<Model>("Models/Orb");
	auto material = Load<Material>("Materials/Sphere");
	auto sound = Load<Sound>("Sounds/Happy_Rock");
	if (!model || !material || !sound)
		return false;

	orb->Add<Mesh>(model, material);
	orb->Add<SoundSource>(sound);

	// Load Monkey.
	model = Load<Model>("Models/Monkey");
	material = Load<Material>("Materials/Monkey");
	monkey->Add<Mesh>(model, material);

	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 12.0f, 12.0f), vec3(0.0f));

	// Setup up renderer.
	mainRenderPass.SetCamera(camera);
	mainRenderPass.SetShader(Shader::MakeNewPassThrough());

	// Setup background color to cornflower blue.
	SetClearColor(0.35f, 0.7f, 0.9f, 0.0f);

	// Setup Scene.
	rootEntity->Get<Hierarchy>().AddChild(orbParent);
	rootEntity->Get<Hierarchy>().AddChild(monkey);
	orbParent->Get<Hierarchy>().AddChild(orb);
	orb->position.x = 5.0f;
	orb->scale *= 5.0f;
	monkey->LookAt(*camera);

	// Start music.
	rootEntity->Add<SoundListener>();
	orb->Get<SoundSource>().Play();

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	float deltaTime = Application.GetDeltaTime();

	orbParent->RotateY(deltaTime * 100.0f);
	orb->RotateY(deltaTime * -100.0f);

	if (Input.IsDown(Key::Up))
	{
		orb->position.x += deltaTime * 5.0f;
	}
	else if (Input.IsDown(Key::Down))
	{
		orb->position.x -= deltaTime * 5.0f;
	}

	orb->position.x = Clamp(orb->position.x, 2.33f, 16.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Bind();
	mainRenderPass.RenderRoot(*rootEntity);
}
