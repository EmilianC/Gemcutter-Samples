#include <gemcutter/Application/Application.h>
#include <gemcutter/Application/Logging.h>
#include <gemcutter/Rendering/Primitives.h>
#include <gemcutter/Rendering/Rendering.h>
#include <gemcutter/Resource/ConfigTable.h>
#include <gemcutter/Resource/Resource.h>
#include <gemcutter/Sound/SoundSystem.h>
#include <gemcutter/Utilities/Random.h>

#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	using namespace gem;

	OpenOutputLog();
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
	config.SetDefaultValue("vsync", "adaptive");
	config.SetDefaultValue("asset_directory", "./");

	if (!config.Load("./Settings.cfg"))
	{
		Warning("Could not load \"Settings.cfg\". Generating a new default.");

		if (!config.Save("./Settings.cfg"))
		{
			Warning("Failed to Generate a new Settings.cfg file.");
		}
	}

	/* Set Asset Directory */
	RootAssetDirectory = config.GetString("asset_directory");

	/* Resolve 'auto' settings */
	RECT desktop;
	bool desktopResolutionOkay = GetWindowRect(GetDesktopWindow(), &desktop) == TRUE;

	if (config.GetInt("resolution_x") == 0)
	{
		config.SetValue("resolution_x", desktopResolutionOkay ? static_cast<int>(desktop.right) : 1280);
	}

	if (config.GetInt("resolution_y") == 0)
	{
		config.SetValue("resolution_y", desktopResolutionOkay ? static_cast<int>(desktop.bottom) : 720);
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

	if (!Application.CreateGameWindow("Gemcutter Sample - Physics 2D", config.GetInt("openGL_Major"), config.GetInt("openGL_Minor")))
	{
		FocusConsoleWindow();
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
	auto game = new Game(config);
	if (!game->Init())
	{
		FocusConsoleWindow();
		ErrorBox("Failed to initialize Game. See \"Log_Output.txt\" for details.");

		delete game;
		Application.DestroyGameWindow();

		return EXIT_FAILURE;
	}
	Log("Initialized Game.");

	/* Start Game Loop */
	Application.GameLoop(
		[=]() { game->Update(); },
		[=]() { game->Draw(); });

	Log("Clean up...");
	delete game;
	Primitives.Unload();
	SoundSystem.Unload();
	Application.DestroyGameWindow();

	Log("-Execution Finished-");

#ifdef _DEBUG
	DestroyConsoleWindow();
#endif
	CloseOutputLog();

	return EXIT_SUCCESS;
}
