#include "game.h"

Scene::Scene(): counter(0), backGround(nullptr) {
}

Game::Game(Player *player, const char *stagePath, const CharDataBase &enemyDB, const CharDataBase &shotDB, const EffectDataBase &effectDB, const OptionDataBase &optionDB, int leftX, int topY, int rightX, int bottomY, const IMG *lifeImg): appearAllEnemyFlag(false), nonEnemyOnDisplay(true), player(player), playerInvincibleFlag(-1), playerOriginalSpeed(player->getSpeed()), playerNonDrawFlag(false), playerLowEffectIndex(0), enemDB(enemyDB), shotDB(shotDB), effectDB(effectDB), optionDB(optionDB), leftX(leftX), rightX(rightX), topY(topY), bottomY(bottomY), keyDirection(CENTER), checkKeyPShotBt(false), checkKeyLowPlayer(false), playerLockonLazerFlag(false), timeOfLastPShot(-9999), timeOfLastPLazer(-9999), enemCount(0), lifeImg(lifeImg) {
	counter = 0;
	effectPool = std::vector<Effect *>(MAX_EFFECT_DISP, nullptr);
	effectPoolFlags = std::vector<bool>(MAX_EFFECT_DISP, false);
	enemyPool = std::vector<Enemy *>(MAX_ENEMY_DISP, nullptr);
	enemyPoolFlags = std::vector<bool>(MAX_ENEMY_DISP, false);
	shotPool = std::vector<Shot *>(MAX_SHOT_DISP, nullptr);
	shotPoolFlags = std::vector<bool>(MAX_SHOT_DISP, false);
	playerShotPool = std::vector<Shot *>(MAX_SHOT_DISP, nullptr);
	playerShotPoolFlags = std::vector<bool>(MAX_SHOT_DISP, false);

	emover = new EnemyMover();
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
		stage.emplace_back(parsed[0], std::stod(parsed[1]), std::stod(parsed[2]), std::stoi(parsed[3]), parsed[4], std::stod(parsed[5]), std::stod(parsed[6]) * M_PI / 180, parsed[7], parsed[8], std::stod(parsed[9]), std::stod(parsed[10]), std::stoi(parsed[11]));
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

	for(size_t i = 0; i < MAX_EFFECT_DISP; i++) {
		if(effectPool[i] != nullptr) delete effectPool[i];
	}

	delete emover;
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

		if(padkey & PAD_INPUT_1) checkKeyPShotBt = true;
		else checkKeyPShotBt = false;

		if(padkey & PAD_INPUT_4 || padkey & PAD_INPUT_2) checkKeyLowPlayer = true;
		else checkKeyLowPlayer = false;
}

void Game::drawShape(const Shape &shape) {
	if(shape.getType() == RECT_SHAPE) DrawBox((int)shape.getLeft(), (int)shape.getTop(), (int)shape.getRight(), (int)shape.getBottom(), GREEN, true);
	else DrawCircle((int)shape.getCenter()->getX(), (int)shape.getCenter()->getY(), (int)shape.getRadius(), RED);
}

size_t Game::searchAddableEffectPool() const {
	for(size_t i = 0; i < MAX_EFFECT_DISP; i++) {
		if(!effectPoolFlags[i]) return i;
	}
	return 0;
}

StagePart Game::getNextEnemyData() {
	enemCount++;
	if(enemCount == stage.size()) appearAllEnemyFlag = true;
	return stage[enemCount - 1];
}

int Game::getNextEnemyTiming() {
	if(enemCount >= stage.size()) return -1;;
	return std::get<STA_TIMING>(stage[enemCount]);
}

void Game::destroyEffectPool(size_t index) {
	if(effectPoolFlags[index]) {
		delete effectPool[index];
		effectPool[index] = nullptr;
		effectPoolFlags[index] = false;
	}
}

void Game::destroyEnemyPool(size_t index) {
	if(enemyPoolFlags[index] == true) {
		for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
			if(playerShotPoolFlags[i] && playerShotPool[i]->getTarget() == enemyPool[index]) playerShotPool[i]->setNullTarget();
		}
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
	if(keyDirection == CENTER) {
		player->move(CENTER);
	} else {
		player->move(keyDirection);
		if(player->getPointPt()->getX() - harfX / 2.0 < leftX) player->setCoord(leftX + harfX / 2.0, player->getPointPt()->getY());
		else if(player->getPointPt()->getX() + harfX / 2.0 > rightX) player->setCoord(rightX - harfX / 2.0, player->getPointPt()->getY());
		if(player->getPointPt()->getY() - harfY / 2.0 < topY) player->setCoord(player->getPointPt()->getX(), topY + harfY / 2.0);
		else if(player->getPointPt()->getY() + harfY / 2.0 > bottomY) player->setCoord(player->getPointPt()->getX(), bottomY - harfY / 2.0);
	}
	playerShotFlagProcessing();
	if(checkKeyLowPlayer) {
		if(player->getSpeed() != playerOriginalSpeed * 0.6) {
			size_t effectIndex = searchAddableEffectPool();
			if(effectIndex != 0 || effectPoolFlags[0] == false) {
				effectPoolFlags[effectIndex] = true;
				effectPool[effectIndex] = new Effect();
				effectPool[effectIndex]->add(Point(0, 0), std::get<EFDB_IMG>(effectDB.at("lazerRange")), std::get<EFDB_ANIM_COUNT>(effectDB.at("lazerRange")), player);
				playerLowEffectIndex = effectIndex;
			}
		}
		player->setSpeed(playerOriginalSpeed * 0.6);
	} else {
		if(player->getSpeed() != playerOriginalSpeed) {
			destroyEffectPool(playerLowEffectIndex);
		}
		player->setSpeed(playerOriginalSpeed);
	}
}

void Game::playerProcessing() {
	playerKeyProcessing();
	if(playerInvincibleFlag != -1 && counter - playerInvincibleFlag > 100) playerInvincibleFlag = -1;

	if(player->getSpeed() != playerOriginalSpeed && playerLockons.size() < MAX_PLAYER_LOCKONS && counter - timeOfLastPLazer > 60) {
		for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
			if(std::find_if(playerLockons.begin(), playerLockons.end(), [&](std::pair<Character *, size_t> &ch) { return ch.first == enemyPool[i]; }) != playerLockons.end()) continue;
			if(enemyPoolFlags[i] && std::pow(enemyPool[i]->getPointPt()->getX() - player->getPointPt()->getX(), 2) + std::pow(enemyPool[i]->getPointPt()->getY() - player->getPointPt()->getY(), 2) < std::pow(180, 2)) {
				size_t effectIndex = searchAddableEffectPool();
				if(effectIndex != 0 || !effectPoolFlags[0]) {
					effectPool[effectIndex] = new Effect();
					effectPool[effectIndex]->add(Point(0, 0), std::get<EFDB_IMG>(effectDB.at("lockon")), std::get<EFDB_ANIM_COUNT>(effectDB.at("lockon")), enemyPool[i]);
					effectPoolFlags[effectIndex] = true;
					playerLockons.emplace_back(enemyPool[i], effectIndex);
					if(playerLockons.size() == MAX_PLAYER_LOCKONS) break;
				}
			}
		}
	}
	if(player->getSpeed() == playerOriginalSpeed && !playerLockons.empty()) {
		for(auto i: playerLockons) destroyEffectPool(i.second);
		playerLockonLazerFlag = true;
	}
}

void Game::playerShotFlagProcessing() {
	// flag on
	Shape *pshotShape;
	double initPx = 0, initPy = 0, harfshapesize1 = 0, harfshapesize2 = 0;
	bool allocatedPshotFlag = false;

	size_t i = 0;

	// main player
	if(checkKeyPShotBt) {
		for(i = 0; i < MAX_SHOT_DISP; i++) {
			if(playerShotPoolFlags[i] == false && counter - timeOfLastPShot > player->getShotInterval()) {
				initPx = player->getPointPt()->getX();
				initPy = player->getPointPt()->getY();
				harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(shotDB.at(player->getShotName())) / 2.0;
				harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(shotDB.at(player->getShotName())) / 2.0;
				if(std::get<CHDB_SHAPE>(shotDB.at(player->getShotName())) == "rect") {
					pshotShape = new Shape(initPx - harfshapesize1, initPy - harfshapesize2, initPx + harfshapesize1, initPy + harfshapesize2);
				} else {
					pshotShape = new Shape(initPx, initPy, harfshapesize1);
				}

				playerShotPool[i] = new Shot(player->getPoint(), player->getShotSpeed(), player->getShotPattern(), 0.0, std::get<CHDB_IMG>(shotDB.at(player->getShotName())), std::get<CHDB_ANIM_COUNT>(shotDB.at(player->getShotName())), pshotShape, std::get<CHDB_HP_OR_POWER>(shotDB.at(player->getShotName())));
				playerShotPoolFlags[i] = true;
				allocatedPshotFlag = true;
				break;
			}
		}
	}

	// player options
	std::vector<Option *> playerOptions = *player->getOptionsPt();
	bool allocatedPLazerFlag = false;
	for(const auto &opt: playerOptions) {
		if(opt->getShotPattern() != "option2" && !checkKeyPShotBt) continue;
		if(opt->getShotPattern() == "option2" && (!playerLockonLazerFlag || playerLockons.empty())) continue;
		if(opt->getShotPattern() != "option2") allocatedPshotFlag = false;
		else allocatedPLazerFlag = false;
		for(i = (i == 0) ? i : (i + 1); i < MAX_SHOT_DISP; i++) {
			if(!playerShotPoolFlags[i] && ((opt->getShotPattern() != "option2" && counter - timeOfLastPShot > opt->getShotInterval()) || (opt->getShotPattern() == "option2" && counter - timeOfLastPLazer > opt->getShotInterval()))) {
				initPx = opt->getCoordPt()->getX();
				initPy = opt->getCoordPt()->getY();
				harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(shotDB.at(opt->getShotName())) / 2.0;
				harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(shotDB.at(opt->getShotName())) / 2.0;
				if(std::get<CHDB_SHAPE>(shotDB.at(opt->getShotName())) == "rect") {
					pshotShape = new Shape(initPx - harfshapesize1, initPy - harfshapesize2, initPx + harfshapesize1, initPy + harfshapesize2);
				} else {
					pshotShape = new Shape(initPx, initPy, harfshapesize1);
				}

				if(opt->getShotPattern() != "option2") {
					playerShotPool[i] = new Shot(*opt->getCoordPt(), opt->getShotSpeed(), opt->getShotPattern(), opt->getShotAngle(), std::get<CHDB_IMG>(shotDB.at(opt->getShotName())), std::get<CHDB_ANIM_COUNT>(shotDB.at(opt->getShotName())), pshotShape, std::get<CHDB_HP_OR_POWER>(shotDB.at(opt->getShotName())));
				} else {
					playerShotPool[i] = new Shot(*opt->getCoordPt(), opt->getShotSpeed(), opt->getShotPattern(), opt->getShotAngle(), std::get<CHDB_IMG>(shotDB.at(opt->getShotName())), std::get<CHDB_ANIM_COUNT>(shotDB.at(opt->getShotName())), pshotShape, std::get<CHDB_HP_OR_POWER>(shotDB.at(opt->getShotName())), true, 0, playerLockons.front().first);
					playerLockons.pop_front();
				}
				playerShotPoolFlags[i] = true;
				if(opt->getShotPattern() != "option2") allocatedPshotFlag = true;
				else allocatedPLazerFlag = true;
				break;
			}
		}
	}
	playerLockonLazerFlag = false;
	if(allocatedPshotFlag) timeOfLastPShot = counter;
	if(allocatedPLazerFlag) timeOfLastPLazer = counter;
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
			if(pt->getX() + harfX < leftX -100 || pt->getX() - harfX > rightX + 100 || pt->getY() + harfY < topY - 100 || pt->getY() - harfY > bottomY + 100) {
				destroyPshotPool(i);
			} else if(playerShotPool[i]->isLazer() && playerShotPool[i]->lazerDestroyableFlag()) destroyPshotPool(i);
		}
	}
}

void Game::enemyShotFlagProcessing() {
	// flag on
	int waynum;
	Shape *eshotShape;
	double initEx = 0, initEy = 0, harfshapesize1 = 0, harfshapesize2 = 0;
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i] == true && enemyPool[i]->getShotFlag()) {
			// main enemy
			size_t j = 0;
			waynum = ShotMover::getWayNum(enemyPool[i]->getShotPattern());
			for(int way = 0; way < waynum; way++) {
				if(enemyPool[i]->getCounter() % enemyPool[i]->getShotInterval() == 0) {
					for(j = 0; j < MAX_SHOT_DISP; j++) {
						if(shotPoolFlags[j] == false) {
							initEx = enemyPool[i]->getPoint().getX();
							initEy = enemyPool[i]->getPoint().getY();
							harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(shotDB.at(enemyPool[i]->getShotName())) / 2.0;
							harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(shotDB.at(enemyPool[i]->getShotName())) / 2.0;
							if(std::get<CHDB_SHAPE>(shotDB.at(enemyPool[i]->getShotName())) == "rect") {
								eshotShape = new Shape(initEx - harfshapesize1, initEy - harfshapesize2, initEx + harfshapesize1, initEy + harfshapesize2);
							} else {
								eshotShape = new Shape(initEx, initEy, harfshapesize1);
							}
							shotPool[j] = new Shot(enemyPool[i]->getPoint(), enemyPool[i]->getShotSpeed(), enemyPool[i]->getShotPattern(), enemyPool[i]->getShotAngle(), std::get<CHDB_IMG>(shotDB.at(enemyPool[i]->getShotName())), std::get<CHDB_ANIM_COUNT>(shotDB.at(enemyPool[i]->getShotName())), eshotShape, 0, false, enemyPool[i]->getShotCnt(), nullptr);
							shotPoolFlags[j] = true;
							enemyPool[i]->incShotCnt();
							break;
						}
					}
				}
			}
			const std::vector<Option *> option = *enemyPool[i]->getOptionsPt();
			for(const auto &opt: option) {
				if(enemyPool[i]->getCounter() % opt->getShotInterval() == 0) {
					for(j = j + 1; j < MAX_SHOT_DISP; j++) {
						if(!shotPoolFlags[j]) {
							initEx = opt->getCoordPt()->getX();
							initEy = opt->getCoordPt()->getY();
							harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(shotDB.at(opt->getShotName())) / 2.0;
							harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(shotDB.at(opt->getShotName())) / 2.0;
							if(std::get<CHDB_SHAPE>(shotDB.at(opt->getShotName())) == "rect") {
								eshotShape = new Shape(initEx - harfshapesize1, initEy - harfshapesize2, initEx + harfshapesize1, initEy + harfshapesize2);
							} else {
								eshotShape = new Shape(initEx, initEy, harfshapesize1);
							}
							shotPool[j] = new Shot(*opt->getCoordPt(), opt->getShotSpeed(), opt->getShotPattern(), 0.0, std::get<CHDB_IMG>(shotDB.at(opt->getShotName())), std::get<CHDB_ANIM_COUNT>(shotDB.at(opt->getShotName())), eshotShape, std::get<CHDB_HP_OR_POWER>(shotDB.at(opt->getShotName())), false, opt->getShotCnt(), nullptr);
							shotPoolFlags[j] = true;
							opt->incShotCnt();
							break;
						}
					} 
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

void Game::enemyMoving() {
	// move
	nonEnemyOnDisplay = true;
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i]) {
			emover->operator()(enemyPool[i]);
			nonEnemyOnDisplay = false;
		}
	}
}

void Game::enemyProcessing() {
	// flag on
	double harfshapesize1 = 0, harfshapesize2 = 0;
	while(true) {
		if(counter == getNextEnemyTiming()) {
			auto [poolEnemyName, poolInitPx, poolInitPy, poolTiming, poolMovePattern, poolMoveSpeed, poolMoveAngle, poolShotName, poolShotPattern, poolShotSpeed, poolShotAngle, poolShotInterval] = getNextEnemyData();
			Enemy *enem;
			Shape *enemShape;
			for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
				if(enemyPoolFlags[i] == false) {
					// create enemy in the enemy pool
					harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(enemDB.at(poolEnemyName)) / 2.0;
					harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(enemDB.at(poolEnemyName)) / 2.0;
					if(std::get<CHDB_SHAPE>(enemDB.at(poolEnemyName)) == "rect")
						enemShape = new Shape(poolInitPx - harfshapesize1, poolInitPy - harfshapesize2, poolInitPx + harfshapesize1, poolInitPy + harfshapesize2);
					else
						enemShape = new Shape(poolInitPx, poolInitPy, harfshapesize1);

					std::vector<Option *> options;
					if(optionDB.count(poolEnemyName) == 1) {
						for(const auto &opt: optionDB.at(poolEnemyName)) {
							options.emplace_back(new Option(Point(std::get<OPDB_X>(opt), std::get<OPDB_Y>(opt)), std::get<OPDB_IMG>(opt), std::get<OPDB_ANIM_COUNT>(opt), std::get<OPDB_SHOTNAME>(opt), std::get<OPDB_SHOTTYPE>(opt), std::get<OPDB_SHOTSPEED>(opt), std::get<OPDB_SHOTANGLE>(opt), std::get<OPDB_SHOTINTERVAL>(opt)));
						}
					}

					enem = new Enemy(poolInitPx, poolInitPy, poolMovePattern, poolMoveSpeed, poolMoveAngle, poolShotPattern, poolShotSpeed, poolShotAngle, poolShotInterval, std::get<CHDB_IMG>(enemDB.at(poolEnemyName)), std::get<CHDB_ANIM_COUNT>(enemDB.at(poolEnemyName)), enemShape, poolShotName, std::get<CHDB_HP_OR_POWER>(enemDB.at(poolEnemyName)), options);

					enemyPool[i] = enem;
					enemyPoolFlags[i] = true;
					break;
				}
			}
		} else {
			break;
		}
	}
	
	// If enemy goes outside or HP is less, destroy the enemy.
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i]) {
			harfshapesize1 = enemyPool[i]->getImage()->getSizeX() / 2.0;
			harfshapesize2 = enemyPool[i]->getImage()->getSizeY() / 2.0;
			if(enemyPool[i]->getPointPt()->getX() + harfshapesize1 < leftX - 1000 || enemyPool[i]->getPointPt()->getY() + harfshapesize2 < topY - 1000 || enemyPool[i]->getPointPt()->getX() - harfshapesize1 > rightX + 1000 || enemyPool[i]->getPointPt()->getY() - harfshapesize2 > bottomY + 1000) destroyEnemyPool(i);
			else if(enemyPool[i]->damaged(0) <= 0) {
				size_t effectIndex = searchAddableEffectPool();
				if(effectIndex != 0 || effectPoolFlags[0] == false) {
					effectPoolFlags[effectIndex] = true;
					effectPool[effectIndex] = new Effect();
					effectPool[effectIndex]->add(enemyPool[i]->getPoint(), std::get<EFDB_IMG>(effectDB.at("miniBomb")), std::get<EFDB_ANIM_COUNT>(effectDB.at("miniBomb")));
				}
				destroyEnemyPool(i);
			}

		}
	}
}

void Game::collisionProcessing() {
	// enemy and player
	size_t effectIndex;
	const Shape *sPlayer = player->getShapePt(), *sEnem;
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i]) {
			sEnem = enemyPool[i]->getShapePt();
			if(collider->operator()(*sPlayer, *sEnem) && playerInvincibleFlag == -1) {
				player->damaged(1);
				playerInvincibleFlag = counter;
				effectIndex = searchAddableEffectPool();
				if(effectIndex == 0 && effectPoolFlags[0] == true) continue;			// Full of effectPool
				effectPoolFlags[effectIndex] = true;
				effectPool[effectIndex] = new Effect();
				for(size_t j = 0; j < 30; j++) effectPool[effectIndex]->add(player->getPointPt()->getX(), player->getPointPt()->getY(), std::get<EFDB_IMG>(effectDB.at("playerDamaged"))[0], 8, 2 * M_PI / 30 * j);
			}
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
						enemyPool[j]->damaged(playerShotPool[i]->getPower());
						break;
					}
				}
			}
			if(delFlag) {
				if(!playerShotPool[i]->isLazer()) destroyPshotPool(i);
				else playerShotPool[i]->incLazerState();
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
				if(playerInvincibleFlag != -1) continue;
				player->damaged(1);
				playerInvincibleFlag = counter;
				effectIndex = searchAddableEffectPool();
				if(effectIndex == 0 && effectPoolFlags[0] == true) continue;			// Full of effectPool
				effectPoolFlags[effectIndex] = true;
				effectPool[effectIndex] = new Effect();
				size_t x = std::get<EFDB_IMG>(effectDB.at("playerDamaged")).size();

				for(size_t j = 0; j < 30; j++) effectPool[effectIndex]->add(player->getPointPt()->getX(), player->getPointPt()->getY(), std::get<EFDB_IMG>(effectDB.at("playerDamaged"))[0], 8, 2 * M_PI / 30 * j);
			}

		}
	}
}

void Game::animationProcessing() {
	// effect flags
	for(size_t i = 0; i < MAX_EFFECT_DISP; i++) {
		if(effectPoolFlags[i]  && (effectPool[i]->areAllEffectsOutOfArea(leftX, topY, rightX, bottomY) || effectPool[i]->minAnimationLoopNum() == 1)) destroyEffectPool(i);
	}

	// Invincible player blinking
	if(playerInvincibleFlag != -1 && (counter - playerInvincibleFlag) % 4 < 2) playerNonDrawFlag = true;
	else playerNonDrawFlag = false;
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

void Game::effectDrawing() {
	for(size_t i = 0; i < MAX_EFFECT_DISP; i++) {
		if(effectPoolFlags[i]) effectPool[i]->drawNextMove();
	}
}

void Game::systemDrawing() {
	// player life
	for(int i = 0; i < player->damaged(0); i++) DrawGraph(10 * (i + 1) + lifeImg->getSizeX() * i, 10, lifeImg->getHandle(), true);
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

void Game::shapeDrawing() {
	for(size_t i = 0; i < MAX_ENEMY_DISP; i++) {
		if(enemyPoolFlags[i]) drawShape(*enemyPool[i]->getShapePt());
	}

	for(size_t i = 0; i < MAX_SHOT_DISP; i++) {
		if(shotPoolFlags[i]) drawShape(*shotPool[i]->getShapePt());
		if(playerShotPoolFlags[i]) drawShape(*playerShotPool[i]->getShapePt());
	}

	drawShape(*player->getShapePt());
}

void Game::playerDrawing() {
	if(!playerNonDrawFlag) player->draw();
}

int Game::update() {
	counter++;
	checkKey();

	// flag process
	playerProcessing();		// with player shots processing and player moving processing.
	enemyProcessing();
	enemyShotFlagProcessing();

	// move process
	enemyMoving();
	playerShotMoving();
	enemyShotMoving();

	// other process
	collisionProcessing();
	bgProcessing();
	animationProcessing();

	if(player->damaged(0) == 0 || (appearAllEnemyFlag && nonEnemyOnDisplay)) {
		player->setSpeed(playerOriginalSpeed);
		return -1;
	}

	return 0;
}

void Game::draw() {
	ClearDrawScreen();

	bgDrawing();
	enemyDrawing();
	playerDrawing();
	effectDrawing();
	playerAndEnemyShotDrawing();
	systemDrawing();

	// shapeDrawing();

	ScreenFlip();
}

// ------------------------------- TitleScene class --------------------------------------------------
TitleScene::TitleScene(): drawPressxkeyFlag(true), isKeyInputableFlag(false) {
	int pressxkeybuf[9];
	counter = 0;
	backGround = new IMG("dat\\image\\title\\bg.png");
	LoadDivGraph("dat\\image\\title\\Pressxkey.png", 9, 9, 1, 50, 50, pressxkeybuf);
	for(size_t i = 0; i < 9; i++) pressXKey.emplace_back(new IMG(pressxkeybuf[i]));
}

TitleScene::~TitleScene() {
	delete backGround;
	for(auto &i: pressXKey) delete i;
}

void TitleScene::animation() {
	if(counter % 120 < 60) drawPressxkeyFlag = true;
	else drawPressxkeyFlag = false;
}

int TitleScene::update() {
	animation();

	int padkey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	if(padkey & PAD_INPUT_1) {
		if(isKeyInputableFlag) {
			isKeyInputableFlag = false;
			return 1;
		}
	} else {
		isKeyInputableFlag = true;
	}

	counter++;
	return 0;
}

void TitleScene::draw() {
	ClearDrawScreen();
	
	// bg
	DrawGraph(0, 0, backGround->getHandle(), true);

	// Press x key
	if(drawPressxkeyFlag) {
		for(size_t i = 0; i < 5; i++) DrawGraph(90 - 22 * (int)i + pressXKey[i]->getSizeX() * (int)i, 480, pressXKey[i]->getHandle(), true);
		DrawGraph(250, 480, pressXKey[5]->getHandle(), true);
		for(size_t i = 6; i < pressXKey.size(); i++) DrawGraph(300 - 22 * (int)(i - 6) + pressXKey[i]->getSizeX() * (int)(i - 6), 480, pressXKey[i]->getHandle(), true);
	}

	ScreenFlip();
}
// ----------------------------------------------------------------------------------------------------

// ----------------------------- GameOverScene class -----------------------------------------
GameOverScene::GameOverScene(): isKeyInputableFlag(false) {
	backGround = new IMG("dat\\image\\title\\gameover.png");
}

GameOverScene::~GameOverScene() {
	delete backGround;
}

int GameOverScene::update() {
	int padkey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	if(padkey & PAD_INPUT_1) {
		if(isKeyInputableFlag) {
			isKeyInputableFlag = false;
			return 1;
		}
	} else {
		isKeyInputableFlag = true;
	}

	return 0;
}

void GameOverScene::draw() {
	ClearDrawScreen();
	DrawGraph(0, 0, backGround->getHandle(), true);
	ScreenFlip();
}
// --------------------------------------------------------------------------------------------------

// ----------------------------- SceneManager class -----------------------------------------------
MitSTG::MitSTG() {
	nowScene = nullptr;
	nowSceneType = SCENE_TITLE;

	loading();

	// GetClientRect(GetMainWindowHandle(), &wndArea);
	wndArea.left = 0;
	wndArea.right = 540;
	wndArea.top = 0;
	wndArea.bottom = 780;
	Point WndCenter(((double)wndArea.right - (double)wndArea.left) / 2.0, ((double)wndArea.bottom - (double)wndArea.top) / 2.0);

	// Create player datas.
	double harfshapesize1 = std::get<CHDB_SHAPE_DATA1>(players["Shirokami_chann"]) / 2.0;
	double harfshapesize2 = std::get<CHDB_SHAPE_DATA2>(players["Shirokami_chann"]) / 2.0;
	std::vector<Option *> playerOptions;
	initPx = WndCenter.getX(), initPy = (double)wndArea.bottom - 100;
	if(std::get<CHDB_SHAPE>(players["Shirokami_chann"]) == "rect")
		pShape = new Shape(initPx - harfshapesize1, initPy - harfshapesize2, initPx + harfshapesize1, initPy + harfshapesize2);
	else
		pShape = new Shape(initPx, initPy, harfshapesize1);
	for(const auto &i: options["Shirokami_chann"]) playerOptions.emplace_back(new Option(Point(std::get<OPDB_X>(i), std::get<OPDB_Y>(i)), std::get<OPDB_IMG>(i), std::get<OPDB_ANIM_COUNT
	>(i), std::get<OPDB_SHOTNAME>(i), std::get<OPDB_SHOTTYPE>(i), std::get<OPDB_SHOTSPEED>(i), std::get<OPDB_SHOTANGLE>(i), std::get<OPDB_SHOTINTERVAL>(i)));

	initPlayerLifeNum = std::get<CHDB_HP_OR_POWER>(players["Shirokami_chann"]);
	player = new Player(initPx, initPy, 5.0, "player1",-12, 8, std::get<CHDB_IMG>(players["Shirokami_chann"]), playerLeftImages["Shirokami_chann"], playerRightImages["Shirokami_chann"], std::get<CHDB_ANIM_COUNT>(players["Shirokami_chann"]), pShape, "Varistor", initPlayerLifeNum, playerOptions);

	// Create other datas.
	lifeImg = new IMG("dat\\image\\system\\life.png");

	changeScene(SCENE_TITLE);
}

MitSTG::~MitSTG() {
	if(nowScene != nullptr) delete nowScene;
	delete pShape;
	delete player;
	delete lifeImg;
}

int MitSTG::loading() {
	std::string buf;
	size_t optionIndex;
	int *loadDivHandles, divNum, bufX, bufY;
	std::vector<std::string> parsed;
	double shapeDataBuf;
	// --------------------------------------------- Loading Database ---------------------------------------------------------
	// players
	std::ifstream ifs("dat\\database\\playerDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');

		divNum = std::stoi(parsed[4]);

		loadDivHandles = new int[divNum];
		GetImageSize_File(("dat\\image\\player\\" + parsed[1]).c_str(), &bufX, &bufY);
		LoadDivGraph(("dat\\image\\player\\" + parsed[1]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) std::get<CHDB_IMG>(players[parsed[0]]).emplace_back(new IMG(loadDivHandles[i]));
		LoadDivGraph(("dat\\image\\player\\" + parsed[2]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) playerLeftImages[parsed[0]].emplace_back(new IMG(loadDivHandles[i]));
		LoadDivGraph(("dat\\image\\player\\" + parsed[3]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) playerRightImages[parsed[0]].emplace_back(new IMG(loadDivHandles[i]));

		std::get<CHDB_ANIM_COUNT>(players[parsed[0]]) = std::stoul(parsed[5]);

		std::get<CHDB_SHAPE>(players[parsed[0]]) = parsed[6];

		shapeDataBuf = std::stod(parsed[7]);
		std::get<CHDB_SHAPE_DATA1>(players[parsed[0]]) = shapeDataBuf;

		shapeDataBuf = std::stod(parsed[8]);
		std::get<CHDB_SHAPE_DATA2>(players[parsed[0]]) = shapeDataBuf;

		std::get<CHDB_HP_OR_POWER>(players[parsed[0]]) = 9999;

		delete[] loadDivHandles;
	}
	ifs.close();

	// options
	ifs.open("dat\\database\\optionDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');

		divNum = std::stoi(parsed[2]);

		loadDivHandles = new int[divNum];
		GetImageSize_File(("dat\\image\\option\\" + parsed[1]).c_str(), &bufX, &bufY);
		LoadDivGraph(("dat\\image\\option\\" + parsed[1]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		optionIndex = options[parsed[0]].size();
		options[parsed[0]].emplace_back();
		for(size_t i = 0; i < divNum; i++) std::get<OPDB_IMG>(options[parsed[0]].at(optionIndex)).emplace_back(new IMG(loadDivHandles[i]));
		size_t b = options[parsed[0]].size();

		std::get<OPDB_ANIM_COUNT>(options[parsed[0]].at(optionIndex)) = std::stoul(parsed[3]);

		std::get<OPDB_X>(options[parsed[0]].at(optionIndex)) = std::stoi(parsed[4]);
		std::get<OPDB_Y>(options[parsed[0]].at(optionIndex)) = std::stoi(parsed[5]);
		
		std::get<OPDB_SHOTNAME>(options[parsed[0]].at(optionIndex)) = parsed[6];
		std::get<OPDB_SHOTTYPE>(options[parsed[0]].at(optionIndex)) = parsed[7];
		std::get<OPDB_SHOTSPEED>(options[parsed[0]].at(optionIndex)) = std::stod(parsed[8]);
		std::get<OPDB_SHOTANGLE>(options[parsed[0]].at(optionIndex)) = std::stod(parsed[9]);
		std::get<OPDB_SHOTINTERVAL>(options[parsed[0]].at(optionIndex)) = std::stoi(parsed[10]);
		

		delete[] loadDivHandles;
	}
	ifs.close();

	// enemys
	ifs.open("dat\\database\\enemyDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');

		divNum = std::stoi(parsed[2]);

		loadDivHandles = new int[divNum];
		GetImageSize_File(("dat\\image\\enemy\\" + parsed[1]).c_str(), &bufX, &bufY);
		LoadDivGraph(("dat\\image\\enemy\\" + parsed[1]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) std::get<CHDB_IMG>(enemys[parsed[0]]).emplace_back(new IMG(loadDivHandles[i]));
		
		std::get<CHDB_ANIM_COUNT>(enemys[parsed[0]]) = std::stoul(parsed[3]);

		std::get<CHDB_SHAPE>(enemys[parsed[0]]) = parsed[4];

		shapeDataBuf = std::stod(parsed[5]);
		std::get<CHDB_SHAPE_DATA1>(enemys[parsed[0]]) = shapeDataBuf;

		 shapeDataBuf = std::stod(parsed[6]);
		std::get<CHDB_SHAPE_DATA2>(enemys[parsed[0]]) = shapeDataBuf;

		std::get<CHDB_HP_OR_POWER>(enemys[parsed[0]]) = std::stoi(parsed[7]);
		delete[] loadDivHandles;
	}
	ifs.close();

	// shots
	ifs.open("dat\\database\\shotDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');

		divNum = std::stoi(parsed[2]);

		loadDivHandles = new int[divNum];
		GetImageSize_File(("dat\\image\\shot\\" + parsed[1]).c_str(), &bufX, &bufY);
		LoadDivGraph(("dat\\image\\shot\\" + parsed[1]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) std::get<CHDB_IMG>(shots[parsed[0]]).emplace_back(new IMG(loadDivHandles[i]));

		std::get<CHDB_ANIM_COUNT>(shots[parsed[0]]) = std::stoul(parsed[3]);

		std::get<CHDB_SHAPE>(shots[parsed[0]]) = parsed[4];

		shapeDataBuf = std::stod(parsed[5]);
		std::get<CHDB_SHAPE_DATA1>(shots[parsed[0]]) = shapeDataBuf;

		shapeDataBuf = std::stod(parsed[6]);
		std::get<CHDB_SHAPE_DATA2>(shots[parsed[0]]) = shapeDataBuf;

		std::get<CHDB_HP_OR_POWER>(shots[parsed[0]]) = std::stoi(parsed[7]);

		delete[] loadDivHandles;
	}
	ifs.close();

	// effects
	ifs.open("dat\\database\\effectDB.csv");
	if(ifs.fail()) return -1;
	while(getline(ifs, buf)) {
		if(buf[0] == '#') continue;
		parsed = split_str(buf, ',');

		divNum = std::stoi(parsed[2]);

		loadDivHandles = new int[divNum];
		GetImageSize_File(("dat\\image\\effect\\" + parsed[1]).c_str(), &bufX, &bufY);
		LoadDivGraph(("dat\\image\\effect\\" + parsed[1]).c_str(), divNum, divNum, 1, bufX / divNum, bufY, loadDivHandles);
		for(size_t i = 0; i < divNum; i++) std::get<EFDB_IMG>(effects[parsed[0]]).emplace_back(new IMG(loadDivHandles[i]));
		std::get<EFDB_ANIM_COUNT>(effects[parsed[0]]) = std::stoul(parsed[3]);

		delete[] loadDivHandles;
	}
	ifs.close();
	// --------------------------------------------------------------------------------------------------------------------------

	return 0;
}

void MitSTG::changeScene(SceneType scene) {
	nowSceneType = scene;
	switch(scene) {
	case SCENE_TITLE:
		if(nowScene != nullptr) delete nowScene;
		nowScene = new TitleScene();
		break;
	case SCENE_GAME_1:
		if(nowScene != nullptr) delete nowScene;
		player->setCoord(initPx, initPy);
		player->damaged(player->damaged(0) - initPlayerLifeNum);
		nowScene = new Game(player, "dat\\stage\\stage1.csv", enemys, shots, effects, options, 0, 0, wndArea.right, wndArea.bottom, lifeImg);
		break;
	case SCENE_GAMEOVER:
		Scene *buf = new GameOverScene();
		for(int i = 0; i < 80; i++) {
			if(nowScene != nullptr) nowScene->draw();
			ClearDrawScreen();
			for(int j = 0; j < wndArea.right / 20; j++) {
				int k = j + i - wndArea.right / 20;
				if(k > 0) {
					if(k > 20) k = 20;
					SetDrawArea(wndArea.right - 20 - j * 20, 0, wndArea.right - (j * 20 - k), wndArea.bottom);
					DrawGraph(0, 0, buf->backGround->getHandle(), false);
				}
			}
			ScreenFlip();
			SetDrawArea(0, 0, wndArea.right, wndArea.bottom);
		}
		if(nowScene != nullptr) delete nowScene;
		nowScene = buf;
		break;
	}
}

void MitSTG::update() {
	int updateValue = nowScene->update();

	switch(nowSceneType) {
	case SCENE_TITLE:
		if(updateValue == 1) {
			player->damaged(player->damaged(0) - initPlayerLifeNum);
			changeScene(SCENE_GAME_1);
		}
		break;
	case SCENE_GAME_1:
		if(updateValue == -1) changeScene(SCENE_GAMEOVER);
		break;
	case SCENE_GAMEOVER:
		if(updateValue == 1) changeScene(SCENE_TITLE);
		break;
	}
}

void MitSTG::draw() {
	nowScene->draw();
}
// ----------------------------------------------------------------------------------------------------
