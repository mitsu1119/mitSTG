#include "game.h"

Game::Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages): player(player), keyDirection(CENTER), counter(0) {
	std::fstream ifs(stagePath);
	if(ifs.fail()) return;

	std::string buf;
	std::vector<std::string> parsed;
	auto enemImages = enemyImages;
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) {
		parsed = split_str(buf, ',');
		stage.emplace_back(enemImages[parsed[0]], std::stoi(parsed[1]));
	}
	ifs.close();
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
	counter++;
	checkKey();

	playerKeyProcessing();
	player->draw();

	ScreenFlip();
}