#include "Game.h"

#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Math/Math.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Light.h>
#include <gemcutter/Rendering/Mesh.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/Material.h>
#include <gemcutter/Resource/Model.h>
#include <gemcutter/Resource/UniformBuffer.h>
#include <gemcutter/Utilities/Random.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	// Toggle colors with the spacebar.
	onKeyPressed = [this](auto& e)
	{
		switch (e.key)
		{
		case Key::Space:
			orb1Color = RandomColor() * 0.25f;
			orb2Color = RandomColor() * 0.25f;

			orb1ColorHandle = orb1Color;
			orb2ColorHandle = orb2Color;
			orb1->Get<Light>().color = orb1Color * 3.0f;
			orb2->Get<Light>().color = orb2Color * 3.0f;
			break;
		}
	};

	onResized = [this](auto& e)
	{
		camera->Get<Camera>().SetAspectRatio(e.GetAspectRatio());

		GBuffer->Resize(e.width, e.height);

		const unsigned halfWidth = e.width / 2;
		const unsigned halfHeight = e.height / 2;

		godRaysBuffer1->Resize(halfWidth, halfHeight);
		godRaysBuffer2->Resize(halfWidth, halfHeight);
		workBuffer1->Resize(halfWidth, halfHeight);
		workBuffer2->Resize(halfWidth, halfHeight);

		if (MSAA_Level > 1)
		{
			GBufferResolve->Resize(e.width, e.height);
			godRaysBuffer1Resolve->Resize(halfWidth, halfHeight);
			godRaysBuffer2Resolve->Resize(halfWidth, halfHeight);
		}
	};
}

bool Game::Init()
{
	/* Setup Camera */
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->position = vec3(0.0f, 1.0f, 8.0f);

	/* Load Shaders */
	staticGeometryProgram = Load<Shader>("Shaders/Default/Lambert");
	flatColorProgram = Load<Shader>("Shaders/Default/FlatColor");
	godRaysRadialBlur = Load<Shader>("Shaders/Radial");
	godRaysComposite = Load<Shader>("Shaders/Composite");
	if (!staticGeometryProgram || !flatColorProgram || !godRaysRadialBlur || !godRaysComposite)
		return false;

	screenSpaceRadialPos = godRaysRadialBlur->buffers[0]->MakeHandle<vec2>("LightPositionOnScreen");

	/* Load Resources */
	auto skybox = Load<Texture>("Textures/Skyboxes/GrimmNight");
	if (!skybox)
		return false;

	ground->Add<Mesh>(Load<Model>("Models/ground"), Load<Material>("Materials/Ground"));
	shack->Add<Mesh>(Load<Model>("Models/shack"), Load<Material>("Materials/Shack"));
	orb1->Add<Mesh>(Load<Model>("Models/Orb"), Load<Material>("Materials/Orb"));
	orb2->Add<Mesh>(Load<Model>("Models/Orb"), Load<Material>("Materials/Orb"));
	orb1->Add<Light>();
	orb2->Add<Light>();

	directionalLight->Add<Light>(vec3(0.08f, 0.08f, 0.15f), Light::Type::Directional);
	directionalLight->LookAt(vec3(10.0f, 10.0f, 3.5f), vec3(0.0f));

	orb1ColorHandle = orb1->Get<Renderable>().buffers[0]->MakeHandle<vec3>("Color");
	orb2ColorHandle = orb2->Get<Renderable>().buffers[0]->MakeHandle<vec3>("Color");
	orb1ColorHandle = orb1Color;
	orb2ColorHandle = orb2Color;
	orb1->Get<Light>().color = orb1Color * 3.0f;
	orb2->Get<Light>().color = orb2Color * 3.0f;

	/* Allocate FrameBuffers */
	MSAA_Level = config.GetInt("MSAA");

	const int width  = Application.GetScreenWidth();
	const int height = Application.GetScreenHeight();
	const int halfWidth  = width / 2;
	const int halfHeight = height / 2;

	GBuffer        = RenderTarget::MakeNew(width,     height,     1, true, MSAA_Level);
	godRaysBuffer1 = RenderTarget::MakeNew(halfWidth, halfHeight, 1, true, MSAA_Level);
	godRaysBuffer2 = RenderTarget::MakeNew(halfWidth, halfHeight, 1, true, MSAA_Level);
	workBuffer1    = RenderTarget::MakeNew(halfWidth, halfHeight, 1, false);
	workBuffer2    = RenderTarget::MakeNew(halfWidth, halfHeight, 1, false);

	GBuffer->       InitTexture(0, TextureFormat::sRGB_8, TextureFilter::Point);
	godRaysBuffer1->InitTexture(0, TextureFormat::sRGB_8, TextureFilter::Linear);
	godRaysBuffer2->InitTexture(0, TextureFormat::sRGB_8, TextureFilter::Linear);
	workBuffer1->   InitTexture(0, TextureFormat::RGB_16, TextureFilter::Linear);
	workBuffer2->   InitTexture(0, TextureFormat::RGB_16, TextureFilter::Linear);

	if (!GBuffer->Validate() ||
		!godRaysBuffer1->Validate() ||
		!godRaysBuffer2->Validate() ||
		!workBuffer1->Validate() ||
		!workBuffer2->Validate())
	{
		return false;
	}

	// Create MSAA resolve buffers.
	if (MSAA_Level > 1)
	{
		GBufferResolve = GBuffer->MakeResolve();
		godRaysBuffer1Resolve = godRaysBuffer1->MakeResolve();
		godRaysBuffer2Resolve = godRaysBuffer2->MakeResolve();
	}

	/* Setup Scene Graph */
	rootEntity->Get<Hierarchy>().AddChild(ground);
	rootEntity->Get<Hierarchy>().AddChild(shack);
	rootEntity->Get<Hierarchy>().AddChild(orbParent);
	cameraParent->Get<Hierarchy>().AddChild(camera);
	orbParent->Get<Hierarchy>().AddChild(orb1);
	orbParent->Get<Hierarchy>().AddChild(orb2);

	orb1->position = vec3(0.0f, 0.5f, 0.0f);
	orb2->position = vec3(4.0f, 1.25f, 0.0f);
	ground->scale = vec3(2.25f);
	shack->scale = vec3(0.33f);

	/* Setup Render Passes */
	// Main diffuse color pass.
	GBufferCall.SetCamera(camera);
	GBufferCall.SetSkybox(skybox);
	GBufferCall.SetTarget(GBuffer);
	GBufferCall.buffers.Add(orb1->Get<Light>().GetBuffer(), 0);
	GBufferCall.buffers.Add(orb2->Get<Light>().GetBuffer(), 1);
	GBufferCall.buffers.Add(directionalLight->Get<Light>().GetBuffer(), 2);

	// Switch the geometry to a dark silhouettes to occlude the upcoming radial blurs of light for Orb1.
	godRaysCall1.SetCamera(camera);
	godRaysCall1.SetTarget(godRaysBuffer1);
	godRaysCall1.SetShader(flatColorProgram);

	// Switch the geometry to a dark silhouettes to occlude the upcoming radial blurs of light for Orb2-.
	godRaysCall2.SetCamera(camera);
	godRaysCall2.SetTarget(godRaysBuffer2);
	godRaysCall2.SetShader(flatColorProgram);

	// Create ray effect for Orb1.
	godRaysRadial1.SetShader(godRaysRadialBlur);
	godRaysRadial1.SetTarget(workBuffer1);
	godRaysRadial1.textures.Add((MSAA_Level > 1) ? godRaysBuffer1Resolve->GetColorTexture(0) : godRaysBuffer1->GetColorTexture(0), 0);

	// Create ray effect for Orb2.
	godRaysRadial2.SetShader(godRaysRadialBlur);
	godRaysRadial2.SetTarget(workBuffer2);
	godRaysRadial2.textures.Add((MSAA_Level > 1) ? godRaysBuffer2Resolve->GetColorTexture(0) : godRaysBuffer2->GetColorTexture(0), 0);

	// Composite the final image.
	composite.SetShader(godRaysComposite);
	composite.textures.Add((MSAA_Level > 1) ? GBufferResolve->GetColorTexture(0) : GBuffer->GetColorTexture(0), 0);
	composite.textures.Add(workBuffer1->GetColorTexture(0), 1);
	composite.textures.Add(workBuffer2->GetColorTexture(0), 2);

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

	// Camera controls.
	if (Input.IsDown(Key::Up))		angleX -= deltaTime * 20.0f;
	if (Input.IsDown(Key::Down))	angleX += deltaTime * 20.0f;
	if (Input.IsDown(Key::Left))	angleY -= deltaTime * 20.0f;
	if (Input.IsDown(Key::Right))	angleY += deltaTime * 20.0f;
	angleX = Clamp(angleX, -80.0f, 0.0f);

	cameraParent->rotation = quat::Identity;
	cameraParent->RotateX(angleX);
	cameraParent->RotateY(angleY);

	orbParent->RotateY(deltaTime * -12.0f);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBufferDepth();
	GBuffer->ClearDepth();
	godRaysBuffer1->ClearDepth();
	godRaysBuffer1->ClearColor(0, vec4(orb1Color * 0.1f, 0.0f));
	godRaysBuffer2->ClearDepth();
	godRaysBuffer2->ClearColor(0, vec4(orb2Color * 0.1f, 0.0f));

	// Compute the screenSpace positions of each orb.
	mat4 mvp = camera->Get<Camera>().GetViewProjMatrix();
	vec4 Orb1ScreenPos = mvp * vec4(orb1->GetWorldTransform().GetTranslation(), 1.0f);
	vec4 Orb2ScreenPos = mvp * vec4(orb2->GetWorldTransform().GetTranslation(), 1.0f);
	Orb1ScreenPos /= Orb1ScreenPos.w;
	Orb2ScreenPos /= Orb2ScreenPos.w;

	// Render the base Scene.
	orb1ColorHandle = orb1Color * 5.0f;
	orb2ColorHandle = orb2Color * 5.0f;
	GBufferCall.Render(*rootEntity);

	// Render the scene again with just orb1 as its true color.
	orb1ColorHandle = orb1Color;
	orb2ColorHandle = vec3::Zero;
	godRaysCall1.Render(*rootEntity);

	// Render the scene again with just orb2 as its true color.
	orb1ColorHandle = vec3::Zero;
	orb2ColorHandle = orb2Color;
	godRaysCall2.Render(*rootEntity);

	// If we are using MSAA, we need to resolve away the extra pixel samples.
	if (MSAA_Level > 1)
	{
		GBuffer->ResolveMultisamplingColor(*GBufferResolve);
		godRaysBuffer1->ResolveMultisamplingColor(*godRaysBuffer1Resolve);
		godRaysBuffer2->ResolveMultisamplingColor(*godRaysBuffer2Resolve);
	}

	// Blur out the light from orb1.
	screenSpaceRadialPos.Set(vec2(Orb1ScreenPos.x * 0.5f + 0.5f, Orb1ScreenPos.y * 0.5f + 0.5f));
	godRaysRadial1.PostProcess();

	// Blur out the light from orb2.
	screenSpaceRadialPos.Set(vec2(Orb2ScreenPos.x * 0.5f + 0.5f, Orb2ScreenPos.y * 0.5f + 0.5f));
	godRaysRadial2.PostProcess();

	// Construct the final image.
	composite.PostProcess();
}
