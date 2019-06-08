#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "STG.h"

class Game {
private:
	Player *player;
	Stage stage;
	Direction keyDirection;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

public:
	Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages);

	void mainLoop();
	void playerKeyProcessing();
};