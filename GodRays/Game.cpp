#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Math/Math.h>
#include <Jewel3D/Utilities/Random.h>
#include <Jewel3D/Entity/Components/Mesh.h>
#include <Jewel3D/Entity/Components/Material.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/Name.h>
#include <Jewel3D/Entity/Components/Light.h>
#include <Jewel3D/Resource/UniformBuffer.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	/* Setup Camera */
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);
	camera->position = vec3(0.0f, 1.0f, 8.0f);

	/* Load Shaders */
	staticGeometryProgram = Load<Shader>("Shaders/Default/Lambert.shader");
	flatColorProgram = Load<Shader>("Shaders/FlatColor.shader");
	godRaysRadialBlur = Load<Shader>("Shaders/GodRays/Radial.shader");
	godRaysComposite = Load<Shader>("Shaders/GodRays/Composite.shader");
	auto flatColorShader = Load<Shader>("Shaders/FlatColor.shader");
	if (!staticGeometryProgram || !flatColorProgram || !godRaysRadialBlur || !godRaysComposite || !flatColorShader) return false;

	screenSpaceRadialPos = godRaysRadialBlur->buffers[0]->GetUniformHandle<vec2>("LightPositionOnScreen");

	/* Load Models and Textures */
	auto groundModel = Load<Model>("Models/ground.model");
	auto groundTexture = Load<Texture>("Textures/ground.png");
	auto shackModel = Load<Model>("Models/shack.model");
	auto shackTexture = Load<Texture>("Textures/shack.png");
	auto orbModel = Load<Model>("Models/Orb.model");
	if (!groundModel || !groundTexture || !shackModel || !shackTexture || !orbModel) return false;

	ground->Add<Mesh>(groundModel);
	shack->Add<Mesh>(shackModel);
	orb1->Add<Mesh>(orbModel);

	ground->Add<Material>(staticGeometryProgram, groundTexture);
	shack->Add<Material>(staticGeometryProgram, shackTexture);
	orb1->Add<Material>(flatColorShader);

	skybox = Texture::MakeNew();
	if (!skybox->LoadCubeMap(
		"./Assets/Textures/Skybox_PosX.png",
		"./Assets/Textures/Skybox_NegX.png",
		"./Assets/Textures/Skybox_PosY.png",
		"./Assets/Textures/Skybox_NegY.png",
		"./Assets/Textures/Skybox_PosZ.png",
		"./Assets/Textures/Skybox_NegZ.png",
		TextureFilterMode::Linear))
	{
		return false;
	}

	orb1->Add<Light>();
	orb2 = orb1->Duplicate();

	directionalLight->Add<Light>(vec3(0.08f, 0.08f, 0.15f), Light::Type::Directional);
	directionalLight->LookAt(vec3(10.0f, 10.0f, 3.5f), vec3(0.0f));

	staticGeometryProgram->buffers.Add(orb1->Get<Light>().GetBuffer(), 0);
	staticGeometryProgram->buffers.Add(orb2->Get<Light>().GetBuffer(), 1);
	staticGeometryProgram->buffers.Add(directionalLight->Get<Light>().GetBuffer(), 2);

	orb1->Get<Material>().CreateUniformBuffer(0);
	orb2->Get<Material>().CreateUniformBuffer(0);
	orb1ColorHandle = orb1->Get<Material>().buffers[0]->GetUniformHandle<vec3>("Color");
	orb2ColorHandle = orb2->Get<Material>().buffers[0]->GetUniformHandle<vec3>("Color");
	orb1ColorHandle = orb1Color;
	orb2ColorHandle = orb2Color;
	orb1->Get<Light>().color = orb1Color;
	orb2->Get<Light>().color = orb2Color;

	/* Allocate FrameBuffers */
	MSAA_Level = config.GetInt("MSAA");

	GBuffer.Init(Application.GetScreenWidth(), Application.GetScreenHeight(), 1, true, MSAA_Level);
	GBuffer.CreateAttachment(0, TextureFormat::RGB_8, TextureFilterMode::Point);
	if (!GBuffer.Validate()) return false;

	godRaysBuffer1.Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, true, MSAA_Level);
	godRaysBuffer1.CreateAttachment(0, TextureFormat::RGB_8, TextureFilterMode::Point);
	if (!godRaysBuffer1.Validate()) return false;

	godRaysBuffer2.Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, true, MSAA_Level);
	godRaysBuffer2.CreateAttachment(0, TextureFormat::RGB_8, TextureFilterMode::Point);
	if (!godRaysBuffer2.Validate()) return false;

	workBuffer1.Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, false);
	workBuffer1.CreateAttachment(0, TextureFormat::RGB_16, TextureFilterMode::Linear);
	if (!workBuffer1.Validate()) return false;

	workBuffer2.Init(Application.GetScreenWidth() / 2, Application.GetScreenHeight() / 2, 1, false);
	workBuffer2.CreateAttachment(0, TextureFormat::RGB_16, TextureFilterMode::Linear);
	if (!workBuffer2.Validate()) return false;

	// Create MSAA resolve buffers
	if (MSAA_Level > 1)
	{
		if (!GBufferResolve.InitAsResolve(GBuffer, TextureFilterMode::Point)) return false;
		if (!godRaysBuffer1Resolve.InitAsResolve(godRaysBuffer1, TextureFilterMode::Linear)) return false;
		if (!godRaysBuffer2Resolve.InitAsResolve(godRaysBuffer2, TextureFilterMode::Linear)) return false;
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
	godRaysRadial1.textures.Add((MSAA_Level > 1) ? godRaysBuffer1Resolve.GetColorTexture(0) : godRaysBuffer1.GetColorTexture(0), 0);

	godRaysRadial2.SetShader(godRaysRadialBlur);
	godRaysRadial2.SetTarget(workBuffer2);
	godRaysRadial2.textures.Add((MSAA_Level > 1) ? godRaysBuffer2Resolve.GetColorTexture(0) : godRaysBuffer2.GetColorTexture(0), 0);

	composite.SetShader(godRaysComposite);
	composite.textures.Add((MSAA_Level > 1) ? GBufferResolve.GetColorTexture(0) : GBuffer.GetColorTexture(0), 0);
	composite.textures.Add(workBuffer1.GetColorTexture(0), 1);
	composite.textures.Add(workBuffer2.GetColorTexture(0), 2);

	return true;
}

void Game::Update()
{
	if (Input.IsDown(Key::Escape))
	{
		Application.Exit();
		return;
	}

	// Toggle colors.
	if (Input.IsDown(Key::Space))
	{
		if (canChangeColor)
		{
			orb1Color = vec3(RandomRangef(0.05f, 0.25f), RandomRangef(0.05f, 0.25f), RandomRangef(0.05f, 0.25f));
			orb2Color = vec3(RandomRangef(0.05f, 0.25f), RandomRangef(0.05f, 0.25f), RandomRangef(0.05f, 0.25f));

			orb1ColorHandle = orb1Color;
			orb2ColorHandle = orb2Color;
			orb1->Get<Light>().color = orb1Color;
			orb2->Get<Light>().color = orb2Color;
			
			canChangeColor = false;
		}
	}
	else
	{
		canChangeColor = true;
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
	GBuffer.ClearDepth();
	godRaysBuffer1.ClearDepth();
	godRaysBuffer1.ClearColor(0, vec4(orb1Color * 0.1f, 0.0f));
	godRaysBuffer2.ClearDepth();
	godRaysBuffer2.ClearColor(0, vec4(orb2Color * 0.1f, 0.0f));

	// Compute the screenSpace positions of each orb.
	vec4 Orb1ScreenPos = camera->Get<Camera>().GetViewProjMatrix() * vec4(orb1->GetWorldTransform().GetTranslation(), 1.0f);
	vec4 Orb2ScreenPos = camera->Get<Camera>().GetViewProjMatrix() * vec4(orb2->GetWorldTransform().GetTranslation(), 1.0f);
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
		GBuffer.ResolveMultisampling(GBufferResolve);
		godRaysBuffer1.ResolveMultisampling(godRaysBuffer1Resolve);
		godRaysBuffer2.ResolveMultisampling(godRaysBuffer2Resolve);
	}

	// Blur out the light from orb1.
	screenSpaceRadialPos.Set(vec2(Orb1ScreenPos.x * 0.5f + 0.5f, Orb1ScreenPos.y * 0.5f + 0.5f));
	godRaysRadial1.Render();

	// Blur out the light from orb2.
	screenSpaceRadialPos.Set(vec2(Orb2ScreenPos.x * 0.5f + 0.5f, Orb2ScreenPos.y * 0.5f + 0.5f));
	godRaysRadial2.Render();

	// Construct the final image.
	composite.Render();
}
