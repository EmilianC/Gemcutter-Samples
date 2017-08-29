#include "Game.h"

#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Math/Math.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Light.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Mesh.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/UniformBuffer.h>
#include <Jewel3D/Utilities/Random.h>

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
			orb1->Get<Light>().color = orb1Color;
			orb2->Get<Light>().color = orb2Color;
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

	/* Load Models and Textures */
	auto skybox = Load<Texture>("Textures/Skybox");
	auto groundModel = Load<Model>("Models/ground");
	auto groundTexture = Load<Texture>("Textures/ground");
	auto shackModel = Load<Model>("Models/shack");
	auto shackTexture = Load<Texture>("Textures/shack");
	auto orbModel = Load<Model>("Models/Orb");
	if (!skybox || !groundModel || !groundTexture || !shackModel || !shackTexture || !orbModel)
		return false;

	ground->Add<Mesh>(groundModel);
	shack->Add<Mesh>(shackModel);
	orb1->Add<Mesh>(orbModel);

	ground->Add<Material>(staticGeometryProgram, groundTexture);
	shack->Add<Material>(staticGeometryProgram, shackTexture);
	orb1->Add<Material>(flatColorProgram);

	orb1->Add<Light>();
	orb2 = orb1->Duplicate();

	directionalLight->Add<Light>(vec3(0.08f, 0.08f, 0.15f), Light::Type::Directional);
	directionalLight->LookAt(vec3(10.0f, 10.0f, 3.5f), vec3(0.0f));

	staticGeometryProgram->buffers.Add(orb1->Get<Light>().GetBuffer(), 0);
	staticGeometryProgram->buffers.Add(orb2->Get<Light>().GetBuffer(), 1);
	staticGeometryProgram->buffers.Add(directionalLight->Get<Light>().GetBuffer(), 2);

	orb1->Get<Material>().CreateUniformBuffer(0);
	orb2->Get<Material>().CreateUniformBuffer(0);
	orb1ColorHandle = orb1->Get<Material>().buffers[0]->MakeHandle<vec3>("Color");
	orb2ColorHandle = orb2->Get<Material>().buffers[0]->MakeHandle<vec3>("Color");
	orb1ColorHandle = orb1Color;
	orb2ColorHandle = orb2Color;
	orb1->Get<Light>().color = orb1Color;
	orb2->Get<Light>().color = orb2Color;

	/* Allocate FrameBuffers */
	MSAA_Level = config.GetInt("MSAA");

	GBuffer->Init(Application.GetScreenWidth(), Application.GetScreenHeight(), 1, true, MSAA_Level);
	GBuffer->CreateAttachment(0, TextureFormat::RGB_8, TextureFilter::Point);
	if (!GBuffer->Validate())
		return false;

	godRaysBuffer1->Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, true, MSAA_Level);
	godRaysBuffer1->CreateAttachment(0, TextureFormat::RGB_8, TextureFilter::Linear);
	if (!godRaysBuffer1->Validate())
		return false;

	godRaysBuffer2->Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, true, MSAA_Level);
	godRaysBuffer2->CreateAttachment(0, TextureFormat::RGB_8, TextureFilter::Linear);
	if (!godRaysBuffer2->Validate())
		return false;

	workBuffer1->Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, false);
	workBuffer1->CreateAttachment(0, TextureFormat::RGB_16, TextureFilter::Linear);
	if (!workBuffer1->Validate())
		return false;

	workBuffer2->Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, false);
	workBuffer2->CreateAttachment(0, TextureFormat::RGB_16, TextureFilter::Linear);
	if (!workBuffer2->Validate())
		return false;

	// Create MSAA resolve buffers
	if (MSAA_Level > 1)
	{
		GBufferResolve = RenderTarget::MakeNew();
		godRaysBuffer1Resolve = RenderTarget::MakeNew();
		godRaysBuffer2Resolve = RenderTarget::MakeNew();

		if (!GBufferResolve->InitAsResolve(*GBuffer)) return false;
		if (!godRaysBuffer1Resolve->InitAsResolve(*godRaysBuffer1)) return false;
		if (!godRaysBuffer2Resolve->InitAsResolve(*godRaysBuffer2)) return false;
	}

	/* Setup Scene Graph */
	rootEntity->AddChild(ground);
	rootEntity->AddChild(shack);
	rootEntity->AddChild(orbParent);
	cameraParent->AddChild(camera);
	orbParent->AddChild(orb1);
	orbParent->AddChild(orb2);

	orb1->position = vec3(0.0f, 0.5f, 0.0f);
	orb2->position = vec3(4.0f, 1.25f, 0.0f);
	ground->scale = vec3(0.18f);
	shack->scale = vec3(0.33f);

	/* Setup Render Passes */
	GBufferCall.SetCamera(camera);
	GBufferCall.SetSkybox(skybox);
	GBufferCall.SetTarget(GBuffer);

	godRaysCall1.SetCamera(camera);
	godRaysCall1.SetTarget(godRaysBuffer1);

	godRaysCall2.SetCamera(camera);
	godRaysCall2.SetTarget(godRaysBuffer2);

	godRaysRadial1.SetShader(godRaysRadialBlur);
	godRaysRadial1.SetTarget(workBuffer1);
	godRaysRadial1.textures.Add((MSAA_Level > 1) ? godRaysBuffer1Resolve->GetColorTexture(0) : godRaysBuffer1->GetColorTexture(0), 0);

	godRaysRadial2.SetShader(godRaysRadialBlur);
	godRaysRadial2.SetTarget(workBuffer2);
	godRaysRadial2.textures.Add((MSAA_Level > 1) ? godRaysBuffer2Resolve->GetColorTexture(0) : godRaysBuffer2->GetColorTexture(0), 0);

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
	if (Input.IsDown(Key::Right))
	{
		angleY += deltaTime * 20.0f;
	}
	else if (Input.IsDown(Key::Left))
	{
		angleY -= deltaTime * 20.0f;
	}

	if (Input.IsDown(Key::Down))
	{
		angleX += deltaTime * 20.0f;
	}
	else if (Input.IsDown(Key::Up))
	{
		angleX -= deltaTime * 20.0f;
	}

	cameraParent->rotation.SetIdentity();
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
	ground->Get<Material>().shader = staticGeometryProgram;
	shack->Get<Material>().shader = staticGeometryProgram;
	orb1ColorHandle = orb1Color * 5.0f;
	orb2ColorHandle = orb2Color * 5.0f;
	GBufferCall.Render(*rootEntity);

	// Switch the geometry to a dark silhouettes to occlude the upcoming radial blurs of light.
	ground->Get<Material>().shader = flatColorProgram;
	shack->Get<Material>().shader = flatColorProgram;

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
		GBuffer->ResolveMultisampling(*GBufferResolve);
		godRaysBuffer1->ResolveMultisampling(*godRaysBuffer1Resolve);
		godRaysBuffer2->ResolveMultisampling(*godRaysBuffer2Resolve);
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
