#pragma once
#include "STG.h"

class Game {
private:
	Player *player;
	Direction keyDirection;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

public:
	Game(Player *player);

	void mainLoop();
	void playerKeyProcessing();
};