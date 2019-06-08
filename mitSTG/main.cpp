#include "DxLib.h"
#include "STG.h"
#include "game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	ChangeWindowMode(true);
	SetGraphMode(960, 720, 64);
	if(DxLib_Init() == -1) return -1;

	SetDrawScreen(DX_SCREEN_BACK);

	IMG playerIMG("dat\\image\\player\\player1.png");
	Player player(100, 100,  5.0, &playerIMG);
	
	Game game(&player);
	while(ProcessMessage() == 0) {
		game.mainLoop();
	}

	DxLib_End();
	return 0;
}