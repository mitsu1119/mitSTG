#include "game.h"

Game::Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages, const IMGDataBase &shotImages, int leftX, int topY, int rightX, int bottomY): player(player), leftX(leftX), rightX(rightX), topY(topY), bottomY(bottomY), keyDirection(CENTER), enemCount(0), counter(0) {
	enemyPool = std::vector<Enemy *>(MAX_ENEMY_DISP, nullptr);
	enemyPoolFlags = std::vector<bool>(MAX_ENEMY_DISP, false);
	shotPool = std::vector<Shot *>(MAX_SHOT_DISP, nullptr);
	shotPoolFlags = std::vector<bool>(MAX_SHOT_DISP, false);

	smover = new ShotMover(player);

	std::fstream ifs(stagePath);
	if(ifs.fail()) return;

	// loading stage file
	std::string buf;
	std::vector<std::string> parsed;
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) {
		parsed = split_str(buf, ',');
		stage.emplace_back(enemyImages.at(parsed[0]), std::stod(parsed[1]), std::stod(parsed[2]), parsed[5], std::stod(parsed[6]), std::stoi(parsed[7]), std::stoi(parsed[3]), shotImages.at(parsed[4]));
	}
	ifs.close();
}

Game::~Game() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPool[i] != nullptr) delete enemyPool[i];
	}
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPool[i] != nullptr) delete shotPool[i];
	}

	delete smover;
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

void Game::enemyShotProcessing() {
	// flag on
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true  && enemyPool[i]->getCounter() % enemyPool[i]->getShotInterval() == 0) {
			for(size_t j = 0; j < MAX_SHOT_DISP; j++) {
				if(shotPoolFlags[j] == false) {
					if(shotPool[j] == nullptr) delete shotPool[j];
					shotPool[j] = new Shot(enemyPool[i]->getPoint(), enemyPool[i]->getShotSpeed(), enemyPool[i]->getShotPattern(), enemyPool[i]->getShotImage());
					shotPoolFlags[j] = true;
					break;
				}
			}
		}
	}

	// move
	const Point *pt;
	int harfX, harfY;
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPoolFlags[i] == true) {
			smover->operator()(shotPool[i]);
			pt = shotPool[i]->getPointPt();
			harfX = int(shotPool[i]->getImageSize().getX() / 2.0);
			harfY = int(shotPool[i]->getImageSize().getY() / 2.0);
			if(pt->getX() + harfX < leftX || pt->getX() - harfX > rightX || pt->getY() + harfY < topY || pt->getY() - harfY > bottomY) {
				shotPoolFlags[i] = false;
			}
		}
	}
}

void Game::enemyProcessing() {
	// flag on
	while(true) {
		if(counter == getNextEnemyTiming()) {
			// create enemy in the enemy pool
			auto [poolImage, poolInitPx, poolInitPy, poolShotPattern, poolShotSpeed, poolShotInterval, poolTiming, poolShotImage] = getNextEnemyData();
			Enemy *enem = new Enemy(poolInitPx, poolInitPy, 10, poolShotPattern, poolShotSpeed, poolShotInterval, poolImage, poolShotImage);
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

	// move
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true) enemyPool[i]->move(CENTER);
	}
}

void Game::enemyShotDrawing() {
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPoolFlags[i] == true) {
			shotPool[i]->draw();
		}
	}
}

void Game::enemyDrawing() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true) {
			enemyPool[i]->draw();
		}
	}
}

void Game::mainLoop() {
	ClearDrawScreen();
	counter++;
	checkKey();

	enemyProcessing();
	playerKeyProcessing();
	enemyShotProcessing();

	player->draw();
	enemyDrawing();
	enemyShotDrawing();
	ScreenFlip();
}