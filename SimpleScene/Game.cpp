#include "Game.h"

#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Mesh.h>
#include <gemcutter/Resource/Material.h>
#include <gemcutter/Resource/Model.h>

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
	// Load all the assets we need.
	auto model = Load<Model>("Models/Helicopter");
	auto material = Load<Material>("Materials/Helicopter");

	// Add a Mesh component with our loaded model and material.
	object->Add<Mesh>(model, material);

	// Create a perspective camera and point it downwards.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 20.0f, 50.0f), vec3(0.0f));

	// Set up the renderer.
	// When mainRenderPass.Render(*object) is called, it will render to the screen from the camera's perspective.
	mainRenderPass.SetCamera(camera);

	// Set the background color to cornflower blue.
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

	// The time in seconds that has passed since the last call to Update().
	float deltaTime = Application.GetDeltaTime();

	// Make the object rotate in place.
	object->RotateY(deltaTime * 35.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Bind();
	mainRenderPass.Render(*object);
}
