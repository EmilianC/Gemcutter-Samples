#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/FileSystem.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/Shader.h>
#include <Jewel3D/Math/Math.h>
#include <Jewel3D/Sound/SoundSystem.h>
#include <Jewel3D/Entity/Components/Camera.h>
#include <Jewel3D/Entity/Components/Text.h>
#include <Jewel3D/Entity/Components/Material.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Load all fonts in the assets directory
	DirectoryData dir;
	ParseDirectory("./Assets/Fonts/", dir);
	for (auto file : dir.files)
	{
		if (file.find(".font") == std::string::npos)
		{
			// Not a font file.
			continue;
		}

		auto font = Load<Font>("Fonts/" + file);
		if (font == nullptr)
		{
			return false;
		}

		fonts.push_back(font);
	}

	if (fonts.size() == 0)
	{
		Error("Could not find any fonts.");
		return false;
	}

	auto shader = Load<Shader>("Shaders/Default/Font.shader");
	if (!shader) return false;

	helloWorldText->Add<Material>(shader).SetBlendMode(BlendFunc::Linear);
	helloWorldText->Add<Text>("Hello World!\n-ABC-\n-123-", fonts[currentFont]).centeredX = true;
	rootEntity->AddChild(helloWorldText);
	helloWorldText->scale = vec3(0.1f);

	instructionText->Add<Material>(shader).SetBlendMode(BlendFunc::Linear);
	instructionText->Add<Text>("Use the arrow keys to cycle through the different fonts.", fonts[0]);
	rootUiEntity->AddChild(instructionText);
	instructionText->scale = vec3(0.5f);
	instructionText->position = vec3(15.0f, 15.0f, 0.0f);

	// Setup Cameras
	camera->Add<Camera>().SetPerspective(65.0f, Application.GetAspectRatio(), 1.0f, 10000.0f);
	camera->LookAt(vec3(0.0f, 7.0f, 55.0f), vec3(0.0f));

	cameraUI->Add<Camera>().SetOrthograpic(Application.GetScreenViewport(), -1.0f, 1.0f);

	// Setup up renderers
	mainRenderPass.SetCamera(camera);
	UIRenderPass.SetCamera(cameraUI);

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

	float deltaTime = Application.GetDeltaTime();

	if (direction)
	{
		angle -= deltaTime * 25.0f;

		if (angle < -45.0f)
		{
			direction = !direction;
		}
	}
	else
	{
		angle += deltaTime * 25.0f;

		if (angle > 45.0f)
		{
			direction = !direction;
		}
	}

	helloWorldText->rotation.SetIdentity();
	helloWorldText->RotateY(angle);

	// Cycle fonts
	if (Input.IsDown(Key::Right))
	{
		if (canChangeFont)
		{
			currentFont++;
			if (currentFont == fonts.size())
			{
				currentFont = 0;
			}

			canChangeFont = false;
			helloWorldText->Get<Text>().SetFont(fonts[currentFont]);
		}
	}
	else if (Input.IsDown(Key::Left))
	{
		if (canChangeFont)
		{
			if (currentFont == 0)
			{
				currentFont = fonts.size() - 1;
			}
			else
			{
				currentFont--;
			}

			canChangeFont = false;
			helloWorldText->Get<Text>().SetFont(fonts[currentFont]);
		}
	}
	else
	{
		canChangeFont = true;
	}

	// Engine systems and components are updated here.
	Application.UpdateEngine();

}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
	UIRenderPass.Render(*rootUiEntity);
}
