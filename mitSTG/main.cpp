
/*
TODO: Write function for display error messages
*/

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "DxLib.h"
#include "STG.h"
#include "game.h"
#include "collision.h"
#include "util.h"

CharDataBase players;
CharDataBase enemys;
CharDataBase shots;
int loading();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(true);
	SetGraphMode(540, 780, 32);
	if(DxLib_Init() == -1) return -1;
	if(loading() == -1) return -1;

	RECT rect;
	GetClientRect(GetMainWindowHandle(), &rect);
	Point WndCenter(((double)rect.right - (double)rect.left) / 2.0, ((double)rect.bottom - (double)rect.top) / 2.0);
	SetDrawScreen(DX_SCREEN_BACK);

	double initPx = WndCenter.getX(), initPy = (double)rect.bottom - 100;
	Shape *pShape;
	double harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(players["Shirokami_chann"]) / 2.0;
	double harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(players["Shirokami_chann"]) / 2.0;
	if(std::get<CHDB_SHAPE>(players["Shirokami_chann"]) == "rect") 
		pShape = new Shape(initPx - harfshapesize1, initPy - harfshapesize2, initPx + harfshapesize1, initPy + harfshapesize2);
	else
		pShape = new Shape(initPx, initPy, harfshapesize1);

	Player player(initPx, initPy, 5.0, "player1", -18.0, 8, std::get<CHDB_IMG>(players["Shirokami_chann"]), pShape, "Varistor");
	Game game(&player, "dat\\stage\\stage1.csv", enemys, shots, 0, 0, rect.right, rect.bottom);
	while(ProcessMessage() == 0) {
		game.mainLoop();
	}

	delete pShape;

	DxLib_End();
	return 0;
}

int loading() {
	std::string buf;
	std::vector<std::string> parsed;
	double shapeDataBuf;
	// --------------------------------------------- Loading Database ---------------------------------------------------------
	// players
	std::ifstream ifs("dat\\database\\playerDB.csv");
	if(ifs.fail()) return -1;	
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');
		std::get<CHDB_IMG>(players[parsed[0]]) = new IMG(("dat\\image\\player\\" + parsed[1]).c_str());
		std::get<CHDB_SHAPE>(players[parsed[0]]) = parsed[2];

		if(parsed[3] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(players[parsed[0]])->getSizeX();
		else if(parsed[3] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(players[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[3]);
		std::get<CHDB_SHAPE_DATA1>(players[parsed[0]]) = shapeDataBuf;
		
		if(parsed[4] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(players[parsed[0]])->getSizeX();
		else if(parsed[4] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(players[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[4]);
		std::get<CHDB_SHAPE_DATA2>(players[parsed[0]]) = shapeDataBuf;
		
		std::get<CHDB_HP_OR_POWER>(players[parsed[0]]) = 5;
	}
	ifs.close();

	// enemys
	ifs.open("dat\\database\\enemyDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');
		std::get<CHDB_IMG>(enemys[parsed[0]]) = new IMG(("dat\\image\\enemy\\" + parsed[1]).c_str());
		std::get<CHDB_SHAPE>(enemys[parsed[0]]) = parsed[2];

		if(parsed[3] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(enemys[parsed[0]])->getSizeX();
		else if(parsed[3] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(enemys[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[3]);
		std::get<CHDB_SHAPE_DATA1>(enemys[parsed[0]]) = shapeDataBuf;

		if(parsed[4] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(enemys[parsed[0]])->getSizeX();
		else if(parsed[4] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(enemys[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[4]);
		std::get<CHDB_SHAPE_DATA2>(enemys[parsed[0]]) = shapeDataBuf;

		std::get<CHDB_HP_OR_POWER>(enemys[parsed[0]]) = 10;
	}
	ifs.close();

	// shots
	ifs.open("dat\\database\\shotDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');
		std::get<CHDB_IMG>(shots[parsed[0]]) = new IMG(("dat\\image\\shot\\" + parsed[1]).c_str());
		std::get<CHDB_SHAPE>(shots[parsed[0]]) = parsed[2];

		if(parsed[3] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(shots[parsed[0]])->getSizeX();
		else if(parsed[3] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(shots[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[3]);
		std::get<CHDB_SHAPE_DATA1>(shots[parsed[0]]) = shapeDataBuf;

		if(parsed[4] == "ImageSizeX") shapeDataBuf = std::get<CHDB_IMG>(shots[parsed[0]])->getSizeX();
		else if(parsed[4] == "ImageSizeY") shapeDataBuf = std::get<CHDB_IMG>(shots[parsed[0]])->getSizeY();
		else shapeDataBuf = std::stod(parsed[4]);
		std::get<CHDB_SHAPE_DATA2>(shots[parsed[0]]) = shapeDataBuf;

		std::get<CHDB_HP_OR_POWER>(shots[parsed[0]]) = 1;
	}
	ifs.close();
	// --------------------------------------------------------------------------------------------------------------------------

	return 0;
}
