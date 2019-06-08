#include "game.h"

Game::Game(Player *player): player(player), keyDirection(CENTER) {
}

void Game::checkKey() {
	if(CheckHitKey(KEY_INPUT_LEFT)) {
		if(CheckHitKey(KEY_INPUT_DOWN)) keyDirection = LDOWN;
		else if(CheckHitKey(KEY_INPUT_UP)) keyDirection = LUP;
		else keyDirection = LEFT;
	} else if(CheckHitKey(KEY_INPUT_RIGHT)) {
		if(CheckHitKey(KEY_INPUT_DOWN)) keyDirection = RDOWN;
		else if(CheckHitKey(KEY_INPUT_UP)) keyDirection = RUP;
		else keyDirection = RIGHT;
	} else if(CheckHitKey(KEY_INPUT_DOWN)) {
		keyDirection = DOWN;
	} else if(CheckHitKey(KEY_INPUT_UP)) {
		keyDirection = UP;
	} else {
		keyDirection = CENTER;
	}
}

void Game::playerKeyProcessing() {
	if(keyDirection != CENTER) player->move(keyDirection);
}

void Game::mainLoop() {
	ClearDrawScreen();
	checkKey();

	playerKeyProcessing();
	player->draw();

	ScreenFlip();
}