#include "game.h"

Game::Game(Player *player, const char *stagePath, const CharDataBase &enemyDB, const CharDataBase &shotDB, int leftX, int topY, int rightX, int bottomY): player(player), enemDB(enemyDB), shotDB(shotDB), leftX(leftX), rightX(rightX), topY(topY), bottomY(bottomY), keyDirection(CENTER), checkKeyPShotBt(false), timeOfLastPShot(-9999), enemCount(0), counter(0) {
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
		stage.emplace_back(parsed[0], std::stod(parsed[1]), std::stod(parsed[2]), parsed[5], std::stod(parsed[6]), std::stoi(parsed[7]), std::stoi(parsed[3]), parsed[4]);
	}
	bgY = bottomY - bgImg->getSizeY();
	ifs.close();
}

Game::~Game() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPool[i] != nullptr) {
			delete enemyPool[i]->getShapePt();
			delete enemyPool[i];
		}
	}
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPool[i] != nullptr) {
			delete shotPool[i]->getShapePt();
			delete shotPool[i];
		}
		if(playerShotPool[i] != nullptr) {
			delete playerShotPool[i]->getShapePt();
			delete playerShotPool[i];
		}
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

void Game::destroyEnemyPool(size_t index) {
	if(enemyPoolFlags[index] == true) {
		delete enemyPool[index]->getShapePt();
		delete enemyPool[index];
		enemyPool[index] = nullptr;
		enemyPoolFlags[index] = false;
	}
}

void Game::destroyEshotPool(size_t index) {
	if(shotPoolFlags[index] == true) {
		delete shotPool[index]->getShapePt();
		delete shotPool[index];
		shotPool[index] = nullptr;
		shotPoolFlags[index] = false;
	}
}

void Game::destroyPshotPool(size_t index) {
	if(playerShotPoolFlags[index] == true) {
		delete playerShotPool[index]->getShapePt();
		delete playerShotPool[index];
		playerShotPool[index] = nullptr;
		playerShotPoolFlags[index] = false;
	}
}

void Game::playerKeyProcessing() {
	double harfX = player->getImage()->getSizeX() / 2.0;
	double harfY = player->getImage()->getSizeY() / 2.0;
	if(keyDirection != CENTER) {
		player->move(keyDirection);
		if(player->getPointPt()->getX() - harfX / 2.0 < leftX) player->setCoord(leftX + harfX / 2.0, player->getPointPt()->getY());
		else if(player->getPointPt()->getX() + harfX / 2.0 > rightX) player->setCoord(rightX - harfX / 2.0, player->getPointPt()->getY());
		if(player->getPointPt()->getY() - harfY / 2.0 < topY) player->setCoord(player->getPointPt()->getX(), topY + harfY / 2.0);
		else if(player->getPointPt()->getY() + harfY / 2.0 > bottomY) player->setCoord(player->getPointPt()->getX(), bottomY - harfY / 2.0);
	}
	if(checkKeyPShotBt) playerShotFlagProcessing();
}

void Game::playerShotFlagProcessing() {
	// flag on
	Shape *pshotShape;
	double initPx = 0, initPy = 0, pshotHarfX = 0, pshotHarfY = 0;

	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(playerShotPoolFlags[i] == false && counter - timeOfLastPShot > player->getShotInterval()) {
			initPx = player->getPointPt()->getX();
			initPy = player->getPointPt()->getY();
			pshotHarfX = std::get<CHDB_IMG>(shotDB.at(player->getShotName()))->getSizeX() / 2.0;
			pshotHarfY = std::get<CHDB_IMG>(shotDB.at(player->getShotName()))->getSizeY() / 2.0;
			if(std::get<CHDB_SHAPE>(shotDB.at(player->getShotName())) == "rect") {
				pshotShape = new Shape(initPx - pshotHarfX, initPy - pshotHarfY, initPx + pshotHarfX, initPy + pshotHarfY);
			} else {
				pshotShape = new Shape(initPx, initPy, 10);
			}

			playerShotPool[i] = new Shot(player->getPoint(), player->getShotSpeed(), player->getShotPattern(), std::get<CHDB_IMG>(shotDB.at(player->getShotName())), pshotShape, 0, enemyPool[0]);
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
				destroyPshotPool(i);
			}
		}
	}
}

void Game::enemyShotFlagProcessing() {
	// flag on
	Shape *eshotShape;
	double initEx = 0, initEy = 0, eshotHarfX = 0, eshotHarfY = 0;
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true  && enemyPool[i]->getCounter() % enemyPool[i]->getShotInterval() == 0) {
			for(size_t j = 0; j < MAX_SHOT_DISP; j++) {
				if(shotPoolFlags[j] == false) {
					initEx = enemyPool[i]->getPoint().getX();
					initEy = enemyPool[i]->getPoint().getY();
					eshotHarfX = std::get<CHDB_IMG>(shotDB.at(enemyPool[i]->getShotName()))->getSizeX() / 2.0;
					eshotHarfY = std::get<CHDB_IMG>(shotDB.at(enemyPool[i]->getShotName()))->getSizeY() / 2.0;
					if(std::get<CHDB_SHAPE>(shotDB.at(enemyPool[i]->getShotName())) == "rect") {
						eshotShape = new Shape(initEx - eshotHarfX, initEy - eshotHarfY, initEx + eshotHarfX, initEy + eshotHarfY);
					} else {
						eshotShape = new Shape(initEx, initEy, 10);
					}
					shotPool[j] = new Shot(enemyPool[i]->getPoint(), enemyPool[i]->getShotSpeed(), enemyPool[i]->getShotPattern(), std::get<CHDB_IMG>(shotDB.at(enemyPool[i]->getShotName())), eshotShape, enemyPool[i]->getShotCnt());
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
				destroyEshotPool(i);
			}
		}
	}
}

void Game::enemyProcessing() {
	// flag on
	double enemHarfX = 0, enemHarfY = 0;
	while(true) {
		if(counter == getNextEnemyTiming()) {
			// create enemy in the enemy pool
			auto [poolEnemyName, poolInitPx, poolInitPy, poolShotPattern, poolShotSpeed, poolShotInterval, poolTiming, poolShotName] = getNextEnemyData();
			Enemy *enem;
			Shape *enemShape;
			for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
				if(enemyPoolFlags[i] == false) {
					enemHarfX = std::get<CHDB_IMG>(enemDB.at(poolEnemyName))->getSizeX() / 2.0;
					enemHarfY = std::get<CHDB_IMG>(enemDB.at(poolEnemyName))->getSizeY() / 2.0;
					if(std::get<CHDB_SHAPE>(enemDB.at(poolEnemyName)) == "rect")
						enemShape = new Shape(poolInitPx - enemHarfX, poolInitPy - enemHarfY, poolInitPx + enemHarfX, poolInitPy + enemHarfY);
					else
						enemShape = new Shape(poolInitPx, poolInitPy, 21);
					enem = new Enemy(poolInitPx, poolInitPy, 10, poolShotPattern, poolShotSpeed, poolShotInterval, std::get<CHDB_IMG>(enemDB.at(poolEnemyName)), enemShape, poolShotName);
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
		if(enemyPoolFlags[i]) enemyPool[i]->move(CENTER);
	}
}

void Game::collisionProcessing() {
	// enemy and player
	const Shape *sPlayer = player->getShapePt(), *sEnem;
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i]) {
			sEnem = enemyPool[i]->getShapePt();
			if(collider->operator()(*sPlayer, *sEnem)) player->move(dirRev(keyDirection));
		}
	}
	
	// enemy and player shots
	bool delFlag = false;
	const Shape *sPshot;
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(playerShotPoolFlags[i]) {
			sPshot = playerShotPool[i]->getShapePt();
			// enemys
			for(size_t j = 0; j < MAX_ENEMY_DISP; j++) {
				if(enemyPoolFlags[j]) {
					sEnem = enemyPool[j]->getShapePt();
					if(collider->operator()(*sPshot, *sEnem)) {
						delFlag = true;
						break;
					}
				}
			}
			if(delFlag) {
				destroyPshotPool(i);
				delFlag = false;
			}
		}
	}

	// player and enemy shots
	const Shape *sEshot;
	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPoolFlags[i]) {
			sEshot = shotPool[i]->getShapePt();
			sPlayer = player->getShapePt();
			if(collider->operator()(*sEshot, *sPlayer)) {
				destroyEshotPool(i);
			}
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