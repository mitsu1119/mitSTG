#include <iostream>
#include <fstream>
#include <string>
#include "DxLib.h"
#include "STG.h"
#include "game.h"
#include "util.h"

IMGDataBase playerImages;
EnemyIMGDataBase enemyImages;
int loading();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(true);
	SetGraphMode(960, 720, 32);
	if(DxLib_Init() == -1) return -1;
	if(loading() == -1) return -1;

	RECT rect;
	GetClientRect(GetMainWindowHandle(), &rect);
	Point WndCenter(((double)rect.right - (double)rect.left) / 2.0, ((double)rect.bottom - (double)rect.top) / 2.0);
	SetDrawScreen(DX_SCREEN_BACK);

	Player player(WndCenter.getX(), (double)rect.bottom - 100,  5.0, playerImages[0]);
	Enemy enemy(WndCenter.getX(), 100.0, 5.0, enemyImages["yellowBox"]);
	Game game(&player, "dat\\stage\\stage1.csv", &enemyImages);
	while(ProcessMessage() == 0) {
		game.mainLoop();
	}

	DxLib_End();
	return 0;
}

int loading() {
	std::string buf;
	// --------------------------------------------- Loading Database ---------------------------------------------------------
	// player images
	std::ifstream ifs("dat\\database\\playerImages.csv");
	if(ifs.fail()) return -1;	
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) playerImages.push_back(new IMG(("dat\\image\\player\\" + buf).c_str()));
	ifs.close();

	// enemy images
	std::vector<std::string> parsed;
	ifs.open("dat\\database\\enemyImages.csv");
	if(ifs.fail()) return -1;
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) {
		parsed = split_str(buf, ',');
		enemyImages[parsed[1]] = new IMG(("dat\\image\\enemy\\" + parsed[0]).c_str());
	}
	ifs.close();
	// --------------------------------------------------------------------------------------------------------------------------

	return 0;
}