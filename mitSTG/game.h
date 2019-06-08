#pragma once
#include <iostream>
#include <fstream>
#include "STG.h"

class Game {
private:
	Player *player;
	Direction keyDirection;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

public:
	Game(Player *player, const char *stagePath, EnemyIMGDataBase *enemyImages);

	void mainLoop();
	void playerKeyProcessing();
};