#include "Game.h"

#include "VelocityFunc.h"
#include "WaveFunc.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Event.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/ParticleEmitter.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/UniformBuffer.h>
#include <Jewel3D/Sound/SoundSystem.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	keyPressed.callback = [](auto& e)
	{
		if (e.key != Key::Space)
			return;

		for (auto& emitter : All<ParticleEmitter>())
		{
			emitter.isPaused = !emitter.isPaused;
		}
	};
}

bool Game::Init()
{
	/* Load Assets */
	auto shader = Load<Shader>("Shaders/Default/ParticleBillBoard.shader");
	auto sparkleTexture = Load<Texture>("Textures/sparkle.texture");
	auto smokeTexture = Load<Texture>("Textures/smoke.texture");
	auto fireTexture = Load<Texture>("Textures/Block.texture");
	if (!shader || !sparkleTexture || !smokeTexture || !fireTexture)
		return false;

	auto& sparkleEmitter = sparkle->Add<ParticleEmitter>();
	auto& smokeEmitter = smoke->Add<ParticleEmitter>();
	auto& fireEmitter = fire->Add<ParticleEmitter>();

	auto& sparkleMaterial = sparkle->Add<Material>(shader, sparkleTexture);
	auto& smokeMaterial = smoke->Add<Material>(shader, smokeTexture);
	auto& fireMaterial = fire->Add<Material>(shader, fireTexture);

	/* Initialize Sparkle Effect */
	sparkleMaterial.SetBlendMode(BlendFunc::Additive);
	sparkleMaterial.SetDepthMode(DepthFunc::TestOnly);
	sparkleEmitter.spawnPerSecond = 2.0f;
	sparkleEmitter.SetLocalSpace(true);
	sparkleEmitter.Warmup(1.5f);

	/* Initialize Smoke Effect */
	smokeMaterial.SetBlendMode(BlendFunc::Additive);
	smokeMaterial.SetDepthMode(DepthFunc::TestOnly);
	smokeEmitter.velocity.Set(0.0f, 0.25f);
	smokeEmitter.functors.Add(RotationFunc::MakeNew(0.0f));
	smokeEmitter.SetSizeStartEnd(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f));
	smokeEmitter.SetColorStartEnd(vec3(0.2f, 0.2f, 0.2f));
	smokeEmitter.Warmup(1.5f);

	/* Initialize Fire Effect */
	fireMaterial.SetBlendMode(BlendFunc::Linear);
	fireMaterial.SetDepthMode(DepthFunc::TestOnly);
	fireEmitter.functors.Add(VelocityFunc::MakeNew());
	fireEmitter.functors.Add(WaveFunc::MakeNew());
	fireEmitter.SetSizeStartEnd(vec2(1.5f, 1.5f), vec2(0.0f, 0.0f));
	fireEmitter.SetColorStartEnd(vec3(1.0f, 0.5f, 0.25f), vec3(0.5f, 0.5f, 0.33f));
	fireEmitter.SetLocalSpace(true);
	fireEmitter.Warmup(1.5f);

	/* Setup Scene */
	rootEntity->AddChild(sparkle);
	rootEntity->AddChild(smoke);
	rootEntity->AddChild(fire);

	sparkle->position.x = 8.0f;
	fire->position.x = -8.0f;

	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->LookAt(vec3(0.0f, 15.0f, 15.0f), vec3(0.0f));

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

	rootEntity->RotateY(Application.GetDeltaTime() * 12.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
