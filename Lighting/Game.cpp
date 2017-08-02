#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Light.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Mesh.h>
#include <Jewel3D/Rendering/ParticleEmitter.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Rendering/Sprite.h>
#include <Jewel3D/Resource/Model.h>
#include <Jewel3D/Sound/SoundListener.h>
#include <Jewel3D/Sound/SoundSystem.h>

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
	auto groundModel = Load<Model>("Models/Ground");
	auto monkeyModel = Load<Model>("Models/Monkey");
	auto checkerTexture = Load<Texture>("Textures/RoomChecker");
	auto monkeyTexture = Load<Texture>("Textures/Monkey");
	auto bulbTexture = Load<Texture>("Textures/LightBulb");
	auto spriteShader = Load<Shader>("Shaders/Default/Sprite");
	auto lightingShader = Load<Shader>("Shaders/MultipleLights");
	if (!groundModel || !monkeyModel || !checkerTexture || !monkeyTexture || !bulbTexture || !spriteShader || !lightingShader)
		return false;

	ground->Add<Mesh>(groundModel);
	ground->Add<Material>(lightingShader, checkerTexture);

	monkey->Add<Mesh>(monkeyModel);
	monkey->Add<Material>(lightingShader, monkeyTexture);

	// Setup Lighting.
	light1->Add<Material>(spriteShader, bulbTexture).SetBlendMode(BlendFunc::Linear);
	light1->Add<Sprite>().SetCenteredX(true);
	light1->Get<Sprite>().SetCenteredY(true);
	light1->Get<Sprite>().SetBillBoarded(true);
	light1->Add<Light>(vec3(0.2f, 7.0f, 7.0f));
	
	// Duplicate a couple more lights.
	light2 = light1->Duplicate();
	light2->Get<Light>().color = vec3(5.5f, 4.0f, 1.5f);

	light3 = light1->Duplicate();
	light3->Get<Light>().color = vec3(7.5f, 7.0f, 7.0f);
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
	rootEntity->AddChild(monkey);
	rootEntity->AddChild(rootLight);

	light1->position = vec3(4.5f, 3.0f, -1.0f);
	light2->position = vec3(-4.5f, 3.0f, -1.0f);
	light3->position = vec3(1.0f, 5.5f, 3.0f);

	ground->position.z -= 2.0f;
	ground->scale = vec3(1.33f);
	monkey->position.y += 2.0f;
	monkey->scale = vec3(2.0f);

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

	// Keep the spotlight focused on the monkey.
	light3->LookAt(*monkey);
	
	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
