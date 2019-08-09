#include "game.h"

Game::Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages, const IMGDataBase &shotImages, int leftX, int topY, int rightX, int bottomY): player(player), leftX(leftX), rightX(rightX), topY(topY), bottomY(bottomY), keyDirection(CENTER), checkKeyPShotBt(false), timeOfLastPShot(-9999), enemCount(0), counter(0) {
	enemyPool = std::vector<Enemy *>(MAX_ENEMY_DISP, nullptr);
	enemyPoolFlags = std::vector<bool>(MAX_ENEMY_DISP, false);
	shotPool = std::vector<Shot *>(MAX_SHOT_DISP, nullptr);
	shotPoolFlags = std::vector<bool>(MAX_SHOT_DISP, false);
	playerShotPool = std::vector<Shot *>(MAX_SHOT_DISP, nullptr);
	playerShotPoolFlags = std::vector<bool>(MAX_SHOT_DISP, false);

	smover = new ShotMover(player);
	collider = new ShapeCollider();

	std::fstream ifs(stagePath);
	if(ifs.fail()) return;

	// loading stage file
	std::string buf;
	std::vector<std::string> parsed;
	getline(ifs, buf);		// skip one line
	while(getline(ifs, buf)) {
		// stage background data
		if(buf[0] == '#') break;
		parsed = split_str(buf, ',');
		bgImg = new IMG(("dat\\image\\bg\\" + parsed[0]).c_str());
		scrollSpeed = std::stoi(parsed[1]);
	}
	while(getline(ifs, buf)) {
		// stage enemys data
		parsed = split_str(buf, ',');
		stage.emplace_back(enemyImages.at(parsed[0]), std::stod(parsed[1]), std::stod(parsed[2]), parsed[5], std::stod(parsed[6]), std::stoi(parsed[7]), std::stoi(parsed[3]), shotImages.at(parsed[4]));
	}
	bgY = bottomY - bgImg->getSizeY();
	ifs.close();
}

Game::~Game() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPool[i] != nullptr) delete enemyPool[i];
	}
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPool[i] != nullptr) delete shotPool[i];
		if(playerShotPool[i] != nullptr) delete playerShotPool[i];
	}

	delete smover;
	delete collider;

	delete bgImg;
}

void Game::checkKey() {
		int padkey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		if(padkey & PAD_INPUT_LEFT) {
			if(padkey & PAD_INPUT_DOWN) keyDirection = LDOWN;
			else if(padkey & PAD_INPUT_UP) keyDirection = LUP;
			else keyDirection = LEFT;
		} else if(padkey & PAD_INPUT_RIGHT) {
			if(padkey & PAD_INPUT_DOWN) keyDirection = RDOWN;
			else if(padkey & PAD_INPUT_UP) keyDirection = RUP;
			else keyDirection = RIGHT;
		} else if(padkey & PAD_INPUT_DOWN) {
			keyDirection = DOWN;
		} else if(padkey & PAD_INPUT_UP) {
			keyDirection = UP;
		} else {
			keyDirection = CENTER;
		}

		if(padkey & PAD_INPUT_A) checkKeyPShotBt = true;
		else checkKeyPShotBt = false;
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
	if(checkKeyPShotBt) playerShotFlagProcessing();
}

void Game::playerShotFlagProcessing() {
	// flag on
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(playerShotPoolFlags[i] == false && counter - timeOfLastPShot > player->getShotInterval()) {
			if(playerShotPool[i] != nullptr) delete playerShotPool[i];
			playerShotPool[i] = new Shot(player->getPoint(), player->getShotSpeed(), player->getShotPattern(), player->getShotImage(), 0, enemyPool[0]);
			playerShotPoolFlags[i] = true;
			timeOfLastPShot = counter;
			break;
		}
	}
}

void Game::playerShotMoving() {
	// move
	const Point *pt;
	int harfX, harfY;
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(playerShotPoolFlags[i] == true) {
			smover->operator()(playerShotPool[i]);
			pt = playerShotPool[i]->getPointPt();
			harfX = int(playerShotPool[i]->getImageSize().getX() / 2.0);
			harfY = int(playerShotPool[i]->getImageSize().getY() / 2.0);
			if(pt->getX() + harfX < leftX || pt->getX() - harfX > rightX || pt->getY() + harfY < topY || pt->getY() - harfY > bottomY) {
				// destroy player shot
				playerShotPoolFlags[i] = false;
			}
		}
	}
}

void Game::enemyShotFlagProcessing() {
	// flag on
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true  && enemyPool[i]->getCounter() % enemyPool[i]->getShotInterval() == 0) {
			for(size_t j = 0; j < MAX_SHOT_DISP; j++) {
				if(shotPoolFlags[j] == false) {
					if(shotPool[j] != nullptr) delete shotPool[j];
					shotPool[j] = new Shot(enemyPool[i]->getPoint(), enemyPool[i]->getShotSpeed(), enemyPool[i]->getShotPattern(), enemyPool[i]->getShotImage(), enemyPool[i]->getShotCnt());
					shotPoolFlags[j] = true;
					enemyPool[i]->incShotCnt();
					break;
				}
			}
		}
	}
}

void Game::enemyShotMoving() {
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
				// destroy enemy shot
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

void Game::collisionProcessing() {
	// enemy and player
	const Point *playerPt, *enemPt;
	double playerHarfX, playerHarfY, enemHarfX, enemHarfY;
	Shape *sPlayer, *sEnem;

	playerPt = player->getPointPt();
	playerHarfX = player->getImage()->getSizeX() / 2.0;
	playerHarfY = player->getImage()->getSizeY() / 2.0;
	sPlayer = new Shape(playerPt->getX() - playerHarfX, playerPt->getY() - playerHarfY, playerPt->getX() + playerHarfX, playerPt->getY() + playerHarfY);
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true) {
			enemPt = enemyPool[i]->getPointPt();
			enemHarfX = enemyPool[i]->getImage()->getSizeX() / 2.0;
			enemHarfY = enemyPool[i]->getImage()->getSizeY() / 2.0;
			sEnem = new Shape(enemPt->getX() - enemHarfX, enemPt->getY() - enemHarfY, enemPt->getX() + enemHarfX, enemPt->getY() + enemHarfY);
			if(collider->operator()(*sPlayer, *sEnem) == true) player->move(dirRev(keyDirection));
			delete sEnem;
		}
	}
	delete sPlayer;

	// enemy and player shots
	bool delFlag = false;
	const Point *pShotPt;
	Shape *sPshot;
	double pShotHarfX, pShotHarfY;

	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(playerShotPoolFlags[i] == true) {
			pShotPt = playerShotPool[i]->getPointPt();
			pShotHarfX = playerShotPool[i]->getImageSize().getX() / 2.0;
			pShotHarfY = playerShotPool[i]->getImageSize().getY() / 2.0;
			sPshot = new Shape(pShotPt->getX() - pShotHarfX, pShotPt->getY() - pShotHarfY, pShotPt->getX() + pShotHarfX, pShotPt->getY() + pShotHarfY);
			// enemys
			for(size_t j = 0; j < MAX_ENEMY_DISP; j++) {
				if(enemyPoolFlags[j] == true) {
					enemPt = enemyPool[j]->getPointPt();
					enemHarfX = enemyPool[j]->getImage()->getSizeX() / 2.0;
					enemHarfY = enemyPool[j]->getImage()->getSizeY() / 2.0;
					sEnem = new Shape(enemPt->getX() - enemHarfX, enemPt->getY() - enemHarfY, enemPt->getX() + enemHarfX, enemPt->getY() + enemHarfY);
					if(collider->operator()(*sPshot, *sEnem) == true) {
						delFlag = true;
						delete sEnem;
						break;
					}
					delete sEnem;
				}
			}
			if(delFlag == true) {
				playerShotPoolFlags[i] = false;
				delFlag = false;
			}
			delete sPshot;
		}
	}
}

void Game::bgProcessing() {
	bgY += scrollSpeed;
}

void Game::bgDrawing() {
	int bgYbuf = bgY;

	do {
		DrawGraph(0, bgYbuf, bgImg->getHandle(), true);
		bgYbuf -= bgImg->getSizeY();
	} while(bgYbuf >= -bgImg->getSizeY());
}

void Game::playerAndEnemyShotDrawing() {
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPoolFlags[i] == true) {
			shotPool[i]->draw();
		}
		if(playerShotPoolFlags[i] == true) {
			playerShotPool[i]->draw();
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
	enemyShotFlagProcessing();

	playerShotMoving();
	enemyShotMoving();

	collisionProcessing();
	bgProcessing();

	bgDrawing();
	player->draw();
	enemyDrawing();
	playerAndEnemyShotDrawing();
	ScreenFlip();
}