#include "Game.h"
#include "Boid.h"

#include <gemcutter/Application/Application.h>
#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Mesh.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Material.h>
#include <gemcutter/Resource/Model.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load game assets.
	auto model = Load<Model>("Models/Bird");
	auto material = Load<Material>("Materials/Boid");
	if (!model || !material) return false;

	// Create all the boids.
	for (unsigned i = 0; i < NumBirds; i++)
	{
		auto boid = rootNode->Get<Hierarchy>().CreateChild();
		boid->Add<Mesh>(model, material);
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
