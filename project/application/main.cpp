#include "GameCore.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) 
{
	// ゲーム本体のインスタンス作成
	std::unique_ptr<TYEngine::Framework::TYFramework> game = std::make_unique<GameCore>();

	// メインループ実行
	game->Run();
	
	return 0;
}