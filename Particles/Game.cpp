#include "Game.h"

#include "VelocityFunc.h"
#include "WaveFunc.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/UniformBuffer.h>
#include <Jewel3D/Sound/SoundSystem.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/ParticleEmitter.h>
#include <Jewel3D/Entity/Components/Material.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	/* Load Assets */
	auto shader = Load<Shader>("Shaders/Default/ParticleBillBoard.shader");
	auto sparkleTexture = Load<Texture>("Textures/sparkle.png");
	auto smokeTexture = Load<Texture>("Textures/smoke.png");
	auto fireTexture = Load<Texture>("Textures/Block.png", TextureFilterMode::Point);
	if (!shader || !sparkleTexture || !smokeTexture || !fireTexture) return false;

	sparkle->Add<ParticleEmitter>();
	smoke->Add<ParticleEmitter>();
	fire->Add<ParticleEmitter>();

	sparkle->Add<Material>(shader, sparkleTexture);
	smoke->Add<Material>(shader, smokeTexture);
	fire->Add<Material>(shader, fireTexture);

	/* Initialize Sparkle Effect */
	sparkle->Get<ParticleEmitter>().spawnPerSecond = 2.0f;
	sparkle->Get<Material>().SetBlendMode(BlendFunc::Additive);
	sparkle->Get<Material>().SetDepthMode(DepthFunc::TestOnly);
	sparkle->Get<ParticleEmitter>().SetLocalSpace(true);
	sparkle->Get<ParticleEmitter>().Warmup(1.5f);

	/* Initialize Smoke Effect */
	smoke->Get<Material>().SetBlendMode(BlendFunc::Additive);
	smoke->Get<Material>().SetDepthMode(DepthFunc::TestOnly);
	smoke->Get<ParticleEmitter>().velocity.Set(0.0f, 0.25f);
	smoke->Get<ParticleEmitter>().functors.Add(RotationFunc::MakeNew(0.0f));
	smoke->Get<ParticleEmitter>().SetSizeStartEnd(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f));
	smoke->Get<ParticleEmitter>().SetColorStartEnd(vec3(0.2f, 0.2f, 0.2f));
	smoke->Get<ParticleEmitter>().Warmup(1.5f);

	/* Initialize Fire Effect */
	fire->Get<Material>().SetBlendMode(BlendFunc::Linear);
	fire->Get<Material>().SetDepthMode(DepthFunc::TestOnly);
	fire->Get<ParticleEmitter>().functors.Add(VelocityFunc::MakeNew());
	fire->Get<ParticleEmitter>().functors.Add(WaveFunc::MakeNew());
	fire->Get<ParticleEmitter>().SetSizeStartEnd(vec2(1.5f, 1.5f), vec2(0.0f, 0.0f));
	fire->Get<ParticleEmitter>().SetColorStartEnd(vec3(1.0f, 0.5f, 0.25f), vec3(0.5f, 0.5f, 0.33f));
	fire->Get<ParticleEmitter>().SetLocalSpace(true);
	fire->Get<ParticleEmitter>().Warmup(1.5f);

	/* Setup Scene */
	rootEntity->AddChild(sparkle);
	rootEntity->AddChild(smoke);
	rootEntity->AddChild(fire);

	sparkle->position.x = 8.0f;
	fire->position.x = -8.0f;

	// Setup Camera
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 15.0f, 15.0f), vec3(0.0f));

	// Setup up renderer
	mainRenderPass.SetCamera(camera);

	// Setup background color to cornflower blue
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

	if (Input.IsDown(Key::Space))
	{
		if (canChangeState)
		{
			sparkle->Get<ParticleEmitter>().isPaused = particleState;
			smoke->Get<ParticleEmitter>().isPaused = particleState;
			fire->Get<ParticleEmitter>().isPaused = particleState;

			particleState = !particleState;
			canChangeState = false;
		}
	}
	else
	{
		canChangeState = true;
	}

	rootEntity->RotateY(Application.GetDeltaTime() * 12.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
