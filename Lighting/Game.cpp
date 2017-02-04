#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/SoundListener.h>
#include <Jewel3D/Entity/Components/Material.h>
#include <Jewel3D/Entity/Components/Mesh.h>
#include <Jewel3D/Entity/Components/ParticleEmitter.h>
#include <Jewel3D/Entity/Components/Light.h>
#include <Jewel3D/Entity/Components/Sprite.h>
#include <Jewel3D/Sound/SoundSystem.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/Model.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	// Keep aspect ratio in sync with the window's dimensions.
	resize.callback = [this](auto&)
	{
		camera->Get<Camera>().SetAspectRatio(Application.GetAspectRatio());
	};
}

bool Game::Init()
{
	// Load game assets.
	auto model = Load<Model>("Models/Ground.model");
	auto checkerTexture = Load<Texture>("Textures/RoomChecker.png");
	auto bulbTexture = Load<Texture>("Textures/LightBulb.png");
	auto spriteShader = Load<Shader>("Shaders/Default/Sprite.shader");
	auto lightingShader = Load<Shader>("Shaders/MultipleLights.shader");
	if (!model || !checkerTexture || !bulbTexture || !spriteShader || !lightingShader) return false;

	ground->Add<Mesh>(model);
	ground->Add<Material>(lightingShader, checkerTexture);

	model = Load<Model>("Models/Teapot.model");

	teapot->Add<Mesh>(model);
	teapot->Add<Material>(lightingShader, checkerTexture);

	// Setup Lighting.
	light1->Add<Material>(spriteShader, bulbTexture).SetBlendMode(BlendFunc::Linear);
	light1->Add<Sprite>().SetCenteredX(true);
	light1->Get<Sprite>().SetCenteredY(true);
	light1->Get<Sprite>().SetBillBoarded(true);
	light1->Add<Light>(vec3(0.0f, 0.70f, 0.70f));
	
	// Duplicate a couple more lights.
	light2 = light1->Duplicate();
	light2->Get<Light>().color = vec3(0.55f, 0.4f, 0.15f);

	light3 = light1->Duplicate();
	light3->Get<Light>().color = vec3(0.75f, 0.7f, 0.7f);
	light3->Get<Light>().type = Light::Type::Spot;

	// Attach lights to the matching shader buffers.
	lightingShader->buffers.Add(light1->Get<Light>().GetBuffer(), 0);
	lightingShader->buffers.Add(light2->Get<Light>().GetBuffer(), 1);
	lightingShader->buffers.Add(light3->Get<Light>().GetBuffer(), 2);
	
	// Setup camera, it is also the listener for all sound in the scene.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 10.0f, 15.0f), vec3(0.0f));

	// Setup scene.
	rootLight->AddChild(light1);
	rootLight->AddChild(light2);
	rootLight->AddChild(light3);
	rootEntity->AddChild(ground);
	rootEntity->AddChild(teapot);
	rootEntity->AddChild(rootLight);

	light1->position = vec3(4.5f, 3.0f, -1.0f);
	light2->position = vec3(-4.5f, 3.0f, -1.0f);
	light3->position = vec3(1.0f, 5.5f, 3.0f);

	ground->position.z -= 2.0f;
	ground->scale = vec3(1.33f);

	teapot->scale = vec3(4.0f);

	// Set up renderer.
	mainRenderPass.SetCamera(camera);

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

	// Rotate all the lights around the scene.
	rootLight->RotateY(deltaTime * 40.0f);

	// Modulate the cone angle over time.
	elapsed += deltaTime;
	light3->Get<Light>().angle = abs(sin(elapsed)) * 40.0f + 10.0f;

	// Keep the spotlight focused on the teapot.
	light3->LookAt(*teapot);
	
	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
