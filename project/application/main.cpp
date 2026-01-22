#include "GameCore.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) 
{
	std::unique_ptr<TYFrameWork> game = std::make_unique<GameCore>();

	game->run();
	
	return 0;
}