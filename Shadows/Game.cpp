#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Light.h>
#include <Jewel3D/Rendering/Mesh.h>
#include <Jewel3D/Math/Matrix.h>

Game::Game(ConfigTable &config)
	: config(config)
{
}

bool Game::Init()
{
	// Prepare ground object.
	auto model = Load<Model>("Models/Ground");
	auto texture = Load<Texture>("Textures/Ground");
	lambertShadow = Load<Shader>("Shaders/LambertShadow");
	if (!model || !texture || !lambertShadow)
		return false;
	
	ground->Add<Material>(lambertShadow, texture);
	ground->Add<Mesh>(model);
	ground->scale = vec3(15.0f);

	// Prepare shack object.
	model = Load<Model>("Models/Shack");
	texture = Load<Texture>("Textures/Shack");
	if (!model || !texture)
		return false;

	shack->Add<Material>(lambertShadow, texture);
	shack->Add<Mesh>(model);
	shack->scale = vec3(1.33f);

	// Setup Scene.
	rootNode->AddChild(ground);
	rootNode->AddChild(shack);

	// Setup Light.
	shadowCamera->Add<Camera>(-20.0f, 20.0f, 20.0f, -20.0f, -100.0f, 100.0f);
	shadowCamera->Add<Light>(vec3(1.0f), Light::Type::Directional);
	shadowCamera->LookAt(vec3(2.0f, 1.0f, 0.5f), vec3(0.0f));

	lambertShadow->buffers.Add(shadowCamera->Get<Light>().GetBuffer(), 0);
	worldToShadow = lambertShadow->buffers[2]->MakeHandle<mat4>("WorldToShadow");

	// Setup Camera.
	mainCamera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 10000.0f);
	mainCamera->position = vec3(0.0f, 15.0f, 40.0f);
	mainCamera->RotateX(-20.0f);

	// Setup up renderer.
	shadowMap = RenderTarget::MakeNew(2048, 2048, 0, true);
	if (!shadowMap->Validate())
		return false;

	shadowRenderPass.SetCamera(shadowCamera);
	shadowRenderPass.SetShader(Shader::MakeNewPassThrough());
	shadowRenderPass.SetTarget(shadowMap);

	mainRenderPass.textures.Add(shadowMap->GetDepthTexture(), 1);
	mainRenderPass.SetCamera(mainCamera);

	// Set background color to black.
	SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	rootNode->RotateY(Application.GetDeltaTime() * -12.0f);

	// Keep shadow direction and world-space to shadowMap matrix up to date.
	worldToShadow = 
		mat4(0.5, 0.0, 0.0, 0.5,
			0.0, 0.5, 0.0, 0.5,
			0.0, 0.0, 0.5, 0.5,
			0.0, 0.0, 0.0, 1.0) * shadowCamera->Get<Camera>().GetViewProjMatrix();

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();
	shadowMap->Clear();

	shadowRenderPass.Render(*rootNode);
	mainRenderPass.Render(*rootNode);
}
