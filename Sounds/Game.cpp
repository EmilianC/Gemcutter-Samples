#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Entity/Hierarchy.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Math/Math.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Mesh.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/Model.h>
#include <Jewel3D/Resource/Shader.h>
#include <Jewel3D/Resource/Sound.h>
#include <Jewel3D/Resource/Texture.h>
#include <Jewel3D/Sound/SoundListener.h>
#include <Jewel3D/Sound/SoundSource.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load Orb.
	auto model = Load<Model>("Models/Orb");
	auto texture = Load<Texture>("Textures/Orb");
	auto sound = Load<Sound>("Sounds/Happy_Rock");
	if (!model || !texture || !sound)
		return false;

	orb->Add<Mesh>(model);
	orb->Add<Material>(texture);
	orb->Add<SoundSource>(sound);

	// Load Monkey.
	model = Load<Model>("Models/Monkey");
	texture = Load<Texture>("Textures/Monkey");
	monkey->Add<Mesh>(model);
	monkey->Add<Material>(texture);

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
	monkey->RotateX(90.0f);

	// Prepare sounds.
	monkey->Add<SoundListener>();
	orb->Get<SoundSource>().SetLooping(true);
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

	mainRenderPass.Render(*rootEntity);
}
