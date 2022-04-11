#include "Game.h"

#include "VelocityFunc.h"
#include "WaveFunc.h"

#include <gemcutter/Application/Event.h>
#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/ParticleEmitter.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Material.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	onResized = [this](auto& e)
	{
		camera->Get<Camera>().SetAspectRatio(e.GetAspectRatio());
	};

	onKeyPressed = [](auto& e)
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
	auto sparkleMaterial = Load<Material>("Materials/ParticleEffects/Sparkle");
	auto smokeMaterial = Load<Material>("Materials/ParticleEffects/Smoke");
	auto fireMaterial = Load<Material>("Materials/ParticleEffects/Fire");
	if (!sparkleMaterial || !smokeMaterial || !fireMaterial)
		return false;

	/* Initialize Effects */
	auto& sparkleEmitter = sparkle->Add<ParticleEmitter>(sparkleMaterial);
	sparkleEmitter.radius.Set(0.0f, 5.0f);
	sparkleEmitter.spawnPerSecond = 2.0f;
	sparkleEmitter.SetLocalSpace(true);
	sparkleEmitter.Warmup(1.5f);

	auto& smokeEmitter = smoke->Add<ParticleEmitter>(smokeMaterial);
	smokeEmitter.radius.Set(0.0f, 2.0f);
	smokeEmitter.velocity.Set(0.0f, 0.25f);
	smokeEmitter.functors.Add(RotationFunc::MakeNew(0.0f));
	smokeEmitter.SetSizeStartEnd(vec2(0.0f, 0.0f), vec2(2.0f, 2.0f));
	smokeEmitter.SetColorStartEnd(vec3(0.15f, 0.15f, 0.15f));
	smokeEmitter.Warmup(1.5f);

	auto& fireEmitter = fire->Add<ParticleEmitter>(fireMaterial);
	fireEmitter.functors.Add(VelocityFunc::MakeNew());
	fireEmitter.functors.Add(WaveFunc::MakeNew());
	fireEmitter.radius.Set(0.0f, 5.0f);
	fireEmitter.SetSizeStartEnd(vec2(1.5f, 1.5f), vec2(0.0f, 0.0f));
	fireEmitter.SetColorStartEnd(vec3(1.0f, 0.5f, 0.25f), vec3(0.5f, 0.5f, 0.33f));
	fireEmitter.SetLocalSpace(true);
	fireEmitter.Warmup(1.5f);

	/* Setup Scene */
	rootEntity->Get<Hierarchy>().AddChild(sparkle);
	rootEntity->Get<Hierarchy>().AddChild(smoke);
	rootEntity->Get<Hierarchy>().AddChild(fire);

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

	mainRenderPass.Bind();
	mainRenderPass.RenderRoot(*rootEntity);
}
