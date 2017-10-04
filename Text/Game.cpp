#include "Game.h"

#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/FileSystem.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Math/Math.h>
#include <Jewel3D/Rendering/Camera.h>
#include <Jewel3D/Rendering/Material.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Rendering/Text.h>
#include <Jewel3D/Resource/Shader.h>
#include <Jewel3D/Sound/SoundSystem.h>
#include <Jewel3D/Utilities/String.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
	onKeyPressed = [this](auto& e)
	{
		switch (e.key)
		{
		case Key::Right:
			currentFont++;
			if (currentFont == fonts.size())
			{
				currentFont = 0;
			}

			UpdateFonts();
			break;

		case Key::Left:
			if (currentFont == 0)
			{
				currentFont = fonts.size() - 1;
			}
			else
			{
				currentFont--;
			}

			UpdateFonts();
			break;

		case Key::Escape:
			Application.Exit();
			return;
		}
	};
}

bool Game::Init()
{
	// Find all the font assets.
	DirectoryData dir;
	ParseDirectory(dir, RootAssetDirectory + "Fonts/");
	for (auto& file : dir.files)
	{
		// Filter for fonts.
		if (!CompareLowercase(ExtractFileExtension(file), ".font"))
			continue;

		auto font = Load<Font>("Fonts/" + file);
		if (!font)
			return false;

		fonts.push_back(font);
	}

	if (fonts.size() == 0)
	{
		Error("Could not find any fonts.");
		return false;
	}

	auto shader = Load<Shader>("Shaders/Default/Font");
	if (!shader)
		return false;

	helloWorldText->Add<Material>(shader).SetBlendMode(BlendFunc::Linear);
	helloWorldText->Add<Text>("Hello World!\n-ABC-\n-123-").centeredX = true;
	rootEntity->AddChild(helloWorldText);

	instructionText->Add<Material>(shader).SetBlendMode(BlendFunc::Linear);
	instructionText->Add<Text>("Use the arrow keys to cycle\nthrough the different fonts.");
	rootUiEntity->AddChild(instructionText);
	instructionText->position = vec3(20.0f, 55.0f, 0.0f);

	UpdateFonts();

	// Setup Cameras.
	camera->Add<Camera>(65.0f, Application.GetAspectRatio(), 1.0f, 10000.0f);
	camera->LookAt(vec3(0.0f, 7.0f, 55.0f), vec3(0.0f));

	cameraUI->Add<Camera>().SetOrthograpic(Application.GetScreenViewport(), -1.0f, 1.0f);

	// Setup up renderers.
	mainRenderPass.SetCamera(camera);
	UIRenderPass.SetCamera(cameraUI);

	// Setup background color to cornflower blue.
	SetClearColor(0.35f, 0.7f, 0.9f, 0.0f);

	return true;
}

void Game::Update()
{
	float deltaTime = Application.GetDeltaTime();

	if (direction)
	{
		angle -= deltaTime * 25.0f;
		direction = angle > -45.0f;
	}
	else
	{
		angle += deltaTime * 25.0f;
		direction = angle > 45.0f;
	}

	helloWorldText->rotation.SetIdentity();
	helloWorldText->RotateY(angle);

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
	UIRenderPass.Render(*rootUiEntity);
}

void Game::UpdateFonts()
{
	helloWorldText->Get<Text>().font = fonts[currentFont];
	instructionText->Get<Text>().font = fonts[currentFont];

	// Normalize the size of the text vs. the resolution of the font.
	helloWorldText->scale = vec3(7.0f / static_cast<float>(fonts[currentFont]->GetFontWidth()));
	instructionText->scale = vec3(33.0f / static_cast<float>(fonts[currentFont]->GetFontWidth()));
}
