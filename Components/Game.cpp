#include "Game.h"
#include "Boid.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Mesh.h>
#include <Jewel3D/Rendering/Rendering.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load game assets.
	auto model = Load<Model>("Models/Bird");
	auto texture = Load<Texture>("Textures/Boid");
	if (!model || !texture) return false;

	// Create all the boids.
	for (unsigned i = 0; i < NumBirds; i++)
	{
		auto boid = rootNode->CreateChild();
		boid->Add<Mesh>(model);
		boid->Add<Material>(Shader::MakeNewPassThrough(), texture);
		boid->Add<Boid>();
	}

	flock.RandomlyPlaceBoids();

	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 150.0f, 150.0f), vec3(0.0f));

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
	
	flock.Update(Application.GetDeltaTime());

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootNode);
}
