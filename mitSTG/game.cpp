#include "game.h"

Game::Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages): player(player), keyDirection(CENTER), enemCount(0), counter(0) {
	enemyPool = std::vector<const Enemy *>(MAX_ENEMY_DISP);
	enemyPoolFlags = std::vector<bool>(MAX_ENEMY_DISP, false);

	std::fstream ifs(stagePath);
	if(ifs.fail()) return;

	std::string buf;
	std::vector<std::string> parsed;
	auto enemImages = enemyImages;
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) {
		parsed = split_str(buf, ',');
		stage.emplace_back(enemImages[parsed[0]], std::stod(parsed[1]), std::stod(parsed[2]), std::stoi(parsed[3]));
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

StagePart Game::getNextEnemyData() {
	enemCount++;
	return stage[enemCount - 1];
}

int Game::getNextEnemyTiming() {
	if(enemCount >= stage.size()) return -1;;
	return std::get<STA_TIMING>(stage[enemCount]);
}

void Game::playerKeyProcessing() {
	if(keyDirection != CENTER) player->move(keyDirection);
}

void Game::enemyProcessing() {
	while(true) {
		if(counter == getNextEnemyTiming()) {
			// create enemy in the enemy pool
			auto [poolImage, poolInitPx, poolInitPy, poolTiming] = getNextEnemyData();
			Enemy *enem = new Enemy(poolInitPx, poolInitPy, 10, poolImage);
			for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
				if(enemyPoolFlags[i] == false) {
					enemyPool[i] = enem;
					enemyPoolFlags[i] = true;
					break;
				}
			}
		} else {
			break;
		}
	}
}

void Game::enemyDrawing() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true) {
			enemyPool[i]->draw();
			printfDx(">>>>>>>>>>>>>>> drawing\n");
		}
	}
}

void Game::mainLoop() {
	ClearDrawScreen();
	counter++;
	printfDx("%d\n", counter);
	checkKey();

	enemyProcessing();
	playerKeyProcessing();

	player->draw();
	enemyDrawing();
	ScreenFlip();
}