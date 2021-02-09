#include "Game.h"

#include <gemcutter/Application/Application.h>
#include <gemcutter/Input/Input.h>
#include <gemcutter/Rendering/Camera.h>
#include <gemcutter/Rendering/Rendering.h>

Game::Game(ConfigTable& _config)
	: config(_config)
{
}

bool Game::Init()
{
	// Setup Camera.
	camera->Add<Camera>(60.0f, Application.GetAspectRatio(), 1.0f, 1000.0f);

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
	
	// Toggle fullscreen.
	if (Input.IsDown(Key::F))
	{
		if (canFullScreen)
		{
			Application.SetFullscreen(!Application.IsFullscreen());
			canFullScreen = false;
		}
	}
	else
	{
		canFullScreen = true;
	}

	// Toggle window border.
	if (Input.IsDown(Key::B))
	{
		if (canChangeBorder)
		{
			Application.SetBordered(!Application.IsBordered());
			canChangeBorder = false;
		}
	}
	else
	{
		canChangeBorder = true;
	}

	// Toggle window resize.
	if (Input.IsDown(Key::R))
	{
		if (canResize)
		{
			Application.SetResizable(!Application.IsResizable());
			canResize = false;
		}
	}
	else
	{
		canResize = true;
	}

	// Set resolution.
	if (Input.IsDown(Key::KeyBoard1))
	{
		if (canChangeResolution)
		{
			Application.SetResolution(800, 600);
			canChangeResolution = false;
		}
	}
	else if (Input.IsDown(Key::KeyBoard2))
	{
		if (canChangeResolution)
		{
			Application.SetResolution(1280, 720);
			canChangeResolution = false;
		}
	}
	else if (Input.IsDown(Key::KeyBoard3))
	{
		if (canChangeResolution)
		{
			Application.SetResolution(1600, 900);
			canChangeResolution = false;
		}
	}
	else if (Input.IsDown(Key::KeyBoard4))
	{
		if (canChangeResolution)
		{
			Application.SetResolution(1920, 1080);
			canChangeResolution = false;
		}
	}
	else
	{
		canChangeResolution = true;
	}

	// Engine systems and components are updated here.
	Application.UpdateEngine();
}

void Game::Draw()
{
	ClearBackBuffer();

	mainRenderPass.Render(*rootEntity);
}
