#include <Jewel3D/Application/Application.h>
#include <Jewel3D/Application/Logging.h>
#include <Jewel3D/Input/Input.h>
#include <Jewel3D/Rendering/Primitives.h>
#include <Jewel3D/Rendering/Rendering.h>
#include <Jewel3D/Resource/ConfigTable.h>
#include <Jewel3D/Resource/Texture.h>
#include <Jewel3D/Sound/SoundSystem.h>
#include <Jewel3D/Utilities/Random.h>

#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	using namespace Jwl;

#ifdef _DEBUG
	CreateConsoleWindow();
#endif

	Log("-Started Execution-");

	/* Register default configuration values */
	ConfigTable config;
	config.SetDefaultValue("bordered", true);
	config.SetDefaultValue("resizable", false);
	config.SetDefaultValue("fullscreen", false);
	config.SetDefaultValue("resolution_x", 1280);
	config.SetDefaultValue("resolution_y", 720);
	config.SetDefaultValue("openGL_Major", 3);
	config.SetDefaultValue("openGL_Minor", 3);
	config.SetDefaultValue("updatesPerSecond", 120);
	config.SetDefaultValue("FPSCap", 120);
	config.SetDefaultValue("MSAA", 1);
	config.SetDefaultValue("anisotropic_level", 2);
	config.SetDefaultValue("texture_filter", "trilinear");
	config.SetDefaultValue("vsync", "adaptive");

	if (!config.LoadConfig("./Settings.cfg"))
	{
		Warning("Could not load \"Settings.cfg\". Generating a new default.");

		if (!config.SaveConfig("./Settings.cfg"))
		{
			Warning("Failed to Generate a new Settings.cfg file.");
		}
	}

	/* Resolve 'auto' settings */
	RECT desktop;
	bool desktopResolutionOkay = GetWindowRect(GetDesktopWindow(), &desktop) == TRUE;

	if (config.GetInt("resolution_x") == 0)
	{
		config.SetValue("resolution_x", desktopResolutionOkay ? desktop.right : 1280);
	}

	if (config.GetInt("resolution_y") == 0)
	{
		config.SetValue("resolution_y", desktopResolutionOkay ? desktop.bottom : 720);
	}

	/* Apply texture Settings */
	{
		Texture::SetDefaultAnisotropicLevel(config.GetFloat("anisotropic_level"));

		std::string filterMode = config.GetString("texture_filter");
		if (filterMode == "trilinear")
		{
			Texture::SetDefaultFilterMode(TextureFilterMode::Trilinear);
		}
		else if (filterMode == "bilinear")
		{
			Texture::SetDefaultFilterMode(TextureFilterMode::Bilinear);
		}
		else if (filterMode == "linear")
		{
			Texture::SetDefaultFilterMode(TextureFilterMode::Linear);
		}
		else
		{
			Texture::SetDefaultFilterMode(TextureFilterMode::Point);
		}
	}

	/* Seed RNG */
	SeedRandomNumberGenerator();

	/* Initialize Audio System */
	if (!SoundSystem.Init()) return EXIT_FAILURE;
	Log("Initialized SoundSystem.");

	/* Create Window */
	Application.SetResolution(config.GetInt("resolution_x"), config.GetInt("resolution_y"));
	Application.SetBordered(config.GetBool("bordered"));
	Application.SetResizable(config.GetBool("resizable"));
	Application.SetFullscreen(config.GetBool("fullscreen"));
	Application.SetUpdatesPerSecond(config.GetInt("updatesPerSecond"));
	Application.SetFPSCap(config.GetInt("FPSCap"));

	if (!Application.CreateGameWindow("Jewel3D Sample - Sprites", config.GetInt("openGL_Major"), config.GetInt("openGL_Minor")))
	{
		MoveConsoleWindowToForeground();
		ErrorBox("Failed to create window. See \"Log_Output.txt\" for details.");
		return EXIT_FAILURE;
	}
	Log("Initialized Window. ( OpenGL " + Application.GetOpenGLVersionString() + " )");

	// Apply VSync mode.
	{
		std::string vsyncMode = config.GetString("vsync");

		if (vsyncMode == "off")
		{
			SetVSync(VSyncMode::Off);
		}
		else if (vsyncMode == "on")
		{
			SetVSync(VSyncMode::On);
		}
		else if (vsyncMode == "adaptive")
		{
			SetVSync(VSyncMode::Adaptive);
		}
	}

	if (!Primitives.Load())
	{
		return EXIT_FAILURE;
	}
	Log("Initialized Primitives.");

	/* Initialize Game */
	Game *game = new Game(config);
	if (!game->Init())
	{
		MoveConsoleWindowToForeground();
		ErrorBox("Failed to initialize Game. See \"Log_Output.txt\" for details.");

		delete game;
		Application.DestroyGameWindow();

		return EXIT_FAILURE;
	}
	Log("Initialized Game.");

	/* Start Game Loop */
	Application.GameLoop(std::bind(&Game::Update, game), std::bind(&Game::Draw, game));

	Log("Clean up...");
	delete game;
	Primitives.Unload();
	SoundSystem.Unload();
	Application.DestroyGameWindow();

	Log("-Execution Finished-");

#ifdef _DEBUG
	DestroyConsoleWindow();
#endif

	return EXIT_SUCCESS;
}
