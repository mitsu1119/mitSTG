#include "DxLib.h"
#include "game.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(true);
	SetGraphMode(540, 780, 32);
	if(DxLib_Init() == -1) return -1;

	MitSTG stg;
	SetDrawScreen(DX_SCREEN_BACK);
	while(ProcessMessage() == 0 && (!CheckHitKey(KEY_INPUT_ESCAPE) || !CheckHitKey(KEY_INPUT_F10))) {
		stg.update();
		stg.draw();
	}

	DxLib_End();
	return 0;
}
