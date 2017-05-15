#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Mesh.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load all the assets we need.
	auto model = Load<Model>("Models/Helicopter.model");
	auto shader = Load<Shader>("Shaders/Simple.shader");
	auto texture = Load<Texture>("Textures/Helicopter.texture");

	// Add a Mesh component with our loaded model.
	object->Add<Mesh>(model);
	// A Material will allow our Mesh to be rendered to the screen with our shader and texture.
	object->Add<Material>(shader, texture);

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

	mainRenderPass.Render(*object);
}
