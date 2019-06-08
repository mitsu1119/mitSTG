#include "DxLib.h"
#include "STG.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	ChangeWindowMode(true);
	SetGraphMode(960, 720, 64);
	if(DxLib_Init() == -1) return -1;

	SetDrawScreen(DX_SCREEN_BACK);

	IMG playerIMG("dat\\image\\player\\player1.png");
	Player player(100, 100, &playerIMG);

	player.info();
	while(ProcessMessage() == 0) {
		ClearDrawScreen();

		ScreenFlip();
	}

	DxLib_End();
	return 0;
}