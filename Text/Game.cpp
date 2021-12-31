#include "Game.h"

#include <gemcutter/Application/Application.h>
#include <gemcutter/Application/FileSystem.h>
#include <gemcutter/Application/Logging.h>
#include <gemcutter/Entity/Hierarchy.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Rendering/Text.h>
#include <gemcutter/Resource/Material.h>
#include <gemcutter/Resource/Shader.h>
#include <gemcutter/Utilities/String.h>

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
	ParseDirectory(dir, "Fonts/");
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

	if (fonts.empty())
	{
		Error("Could not find any fonts.");
		return false;
	}

	auto material = Load<Material>("Materials/Text");
	if (!material)
		return false;

	helloWorldText->Add<Text>(fonts[currentFont], "Hello World!\n-ABC-\n-123-", material).centeredX = true;
	rootEntity->Get<Hierarchy>().AddChild(helloWorldText);

	instructionText->Add<Text>(fonts[currentFont], "Use the arrow keys to cycle\nthrough different fonts.", material);
	rootUiEntity->Get<Hierarchy>().AddChild(instructionText);
	instructionText->position = vec3(20.0f, 55.0f, 0.0f);

	UpdateFonts();

	// Setup Cameras.
	camera->Add<Camera>(65.0f, Application.GetAspectRatio(), 1.0f, 10000.0f);
	camera->LookAt(vec3(0.0f, 7.0f, 55.0f), vec3(0.0f));

	cameraUI->Add<Camera>().SetOrthographic(Application.GetScreenViewport(), -1.0f, 1.0f);

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

	mainRenderPass.Bind();
	mainRenderPass.RenderRoot(*rootEntity);

	UIRenderPass.Bind();
	UIRenderPass.RenderRoot(*rootUiEntity);
}

void Game::UpdateFonts()
{
	helloWorldText->Get<Text>().font = fonts[currentFont];
	instructionText->Get<Text>().font = fonts[currentFont];

	// Normalize the size of the text vs. the resolution of the font.
	helloWorldText->scale = vec3(7.0f / static_cast<float>(fonts[currentFont]->GetFontWidth()));
	instructionText->scale = vec3(33.0f / static_cast<float>(fonts[currentFont]->GetFontWidth()));
}
