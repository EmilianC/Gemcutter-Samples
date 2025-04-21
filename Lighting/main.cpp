#include <gemcutter/Application/Application.h>
#include <gemcutter/Application/Logging.h>
#include <gemcutter/Resource/ConfigTable.h>
#ifdef GEM_WINDOWS
#include <windows.h>
#endif

#include "Game.h"

namespace
{
	Game* game = nullptr;
}

#ifdef GEM_WINDOWS
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
#else
int main()
#endif
{
	using namespace gem;

	if (!Application.Startup())
	{
		FocusConsoleWindow();
		ErrorBox("Failed to startup the application. See \"Log_Output.txt\" for details.");
		return EXIT_FAILURE;
	}

	ConfigTable config = LoadApplicationConfig();
	SetCurrentDirectory("./assets");

	if (!Application.CreateGameWindow("Gemcutter Sample - Lighting", config))
	{
		FocusConsoleWindow();
		ErrorBox("Failed to create a window. See \"Log_Output.txt\" for details.");
		return EXIT_FAILURE;
	}

	int exitCode = EXIT_SUCCESS;
	game = new Game(config);
	if (game->Init())
	{
		/* Run the game loop */
		Application.GameLoop(
			[] { game->Update(); },
			[] { game->Draw(); }
		);
	}
	else
	{
		FocusConsoleWindow();
		ErrorBox("Failed to initialize the game. See \"Log_Output.txt\" for details.");
		exitCode = EXIT_FAILURE;
	}

	delete game;
	Application.DestroyGameWindow();
	Application.Shutdown();

	return exitCode;
}
