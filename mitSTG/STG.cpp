#include "STG.h"

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Point point, double speed, std::string movePattern, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, int power, int number, Character *target): point(point), shape(shape), speed(speed), movePattern(movePattern), image(image), angle(0.0), counter(0), number(number), animationNum(image.size()), animationCount(animationCount), damagePower(power), target(target) {
}

void Shot::updateShape() const {
	if(shape->getType() == RECT_SHAPE) {
		double harfX = (shape->getRight() - shape->getLeft()) / 2.0, harfY = (shape->getBottom() - shape->getTop()) / 2.0;
		shape->resetCoord(point.getX() - harfX, point.getY() - harfY, point.getX() + harfX, point.getY() + harfY);
	}  else shape->resetCoord(point.getX(), point.getY());
}

Point Shot::getImageSize() const {
	return Point(image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX(), image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY());
}

const Shape *Shot::getShapePt() const {
	return shape;
}

const Point *Shot::getPointPt() const {
	return &point;
}

void Shot::moveX(double dx) {
	point.moveX(dx);
}

void Shot::moveY(double dy) {
	point.moveY(dy);
}

std::string Shot::getMovePattern() const {
	return movePattern;
}

Character *Shot::getTarget() const {
	return target;
}

void Shot::setNullTarget() {
	target = nullptr;
}

int Shot::getPower() const {
	return damagePower;
}

void Shot::draw() const {
	DrawGraph((int)point.getX() - image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0, (int)point.getY() - image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0, image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
	// DrawRotaGraph((int)point.getX(), (int)point.getY(), 1.0, angle + M_PI / 2, image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- ShotMover class -----------------------------------------
ShotMover::ShotMover(const Player *player): player(player) {
	moveFuncTable["player1"] = &ShotMover::player1;
	moveFuncTable["option1"] = &ShotMover::option1;
	moveFuncTable["option2"] = &ShotMover::option2;
	moveFuncTable["target"] = &ShotMover::target;
	moveFuncTable["swirl"] = &ShotMover::swirl;
}

void ShotMover::player1(Shot *shot) {
	shot->moveY(shot->speed);
	shot->angle = -M_PI / 2.0;
	shot->counter++;
}

void ShotMover::option1(Shot *shot) {
	player1(shot);
}

void ShotMover::option2(Shot *shot) {
	if(shot->target == nullptr) {
		shot->moveX(shot->speed / 2 * cos(shot->angle));
		shot->moveY(shot->speed / 2 * sin(shot->angle));
		shot->counter++;
		return;
	}
	double t = (1.0 / shot->speed) * shot->counter;
	double p0x = player->getPointPt()->getX(), p0y = player->getPointPt()->getY();
	double p3x = shot->target->getPointPt()->getX(), p3y = shot->target->getPointPt()->getY();
	double rad = atan2(p0y - p3y, p0x - p3x);
	double p1x = p0x - 20, p1y = p0y + 200;
	double p2x = (p0x + p3x) / 2 + 3 * cos(rad + M_PI / 2), p2y = (p0y + p3y) / 2 + 3 * sin(rad + M_PI / 2);
	double x = (1 - t) * (1 - t) * (1 - t) * p0x + 3 * (1 - t) * (1 - t) * t * p1x + 3 * (1 - t) * t * t * p2x + t * t * t * p3x;
	double y = (1 - t) * (1 - t) * (1 - t) * p0y + 3 * (1 - t) * (1 - t) * t * p1y + 3 * (1 - t) * t * t * p2y + t * t * t * p3y;
	shot->angle = atan2(y - shot->point.getY(), x - shot->point.getX());
	shot->point.setX(x);
	shot->point.setY(y);
	shot->counter++;
}

void ShotMover::target(Shot *shot) {
	if(shot->counter == 0) shot->angle = atan2(player->getPointPt()->getY() - shot->point.getY(), player->getPointPt()->getX() - shot->point.getX());
	shot->moveX(shot->speed * cos(shot->angle));
	shot->moveY(shot->speed * sin(shot->angle));
	shot->counter++;
}

void ShotMover::swirl(Shot *shot) {
	if(shot->counter== 0) shot->angle = (double)shot->number * M_PI / 10.8;
	shot->moveX(shot->speed * cos(shot->angle));
	shot->moveY(shot->speed * sin(shot->angle));
	shot->counter++;
}

// -------------------------------------------------------------------------------------

// ------------------------- Option class ---------------------------------------------------------------------------
Option::Option(Point p, std::vector<const IMG *> image, unsigned long animationCount, std::string shotName, std::string shotPattern, double shotSpeed, int shotInterval): point(p), coord(Point(0, 0)), image(image), animationCount(animationCount), shotPattern(shotPattern), shotName(shotName), shotSpeed(shotSpeed), shotInterval(shotInterval), animationNum(image.size()), counter(0), shotCnt(0) {
	update(0, 0);
}

void Option::update(double ownerX, double ownerY) {
	coord.setX(ownerX + point.getX());
	coord.setY(ownerY + point.getY());
	counter++;
}

void Option::incShotCnt() {
	shotCnt++;
}

const Point *Option::getCoordPt() const {
	return &coord;
}

std::string Option::getShotName() const {
	return shotName;
}

std::string Option::getShotPattern() const {
	return shotPattern;
}

double Option::getShotSpeed() const {
	return shotSpeed;
}

int Option::getShotInterval() const {
	return shotInterval;
}

int Option::getShotCnt() const {
	return shotCnt;
}

void Option::draw() const {
	DrawGraph(int(coord.getX() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0), int(coord.getY() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0), image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
}
// --------------------------------------------------------------------------------------------------------------------

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount,  Shape *shape, std::string shotName, int HP, std::vector<Option *> options): animationNum(image.size()), animationCount(animationCount), point(p), speed(speed), shotPattern(shotPattern), shotSpeed(shotSpeed), shotInterval(shotInterval), image(image), shape(shape), shotName(shotName), counter(0), HP(HP), options(options) {
	optionUpdate();
}

void Character::updateShape() {
	if(shape->getType() == RECT_SHAPE) {
		double harfX = (shape->getRight() - shape->getLeft()) / 2.0, harfY = (shape->getBottom() - shape->getTop()) / 2.0;
		shape->resetCoord(point.getX() - harfX, point.getY() - harfY, point.getX() + harfX, point.getY() + harfY);
	}  else shape->resetCoord(point.getX(), point.getY());
}

void Character::optionUpdate() {
	for(auto &i: options) i->update(point.getX(), point.getY());
}

void Character::setCoord(double x, double y) {
	point.setX(x);
	point.setY(y);
	updateShape();
}

Point Character::getPoint() const {
	return point;
}

const Shape *Character::getShapePt() const {
	return shape;
}

const Point *Character::getPointPt() const {
	return &point;
}

const IMG *Character::getImage() const {
	return image[(counter % (animationNum * animationCount)) / animationCount];
}

std::string Character::getShotName() const {
	return shotName;
}

std::string Character::getShotPattern() const{
	return shotPattern;
}

double Character::getSpeed() const {
	return speed;
}

double Character::getShotSpeed() const {
	return shotSpeed;
}

int Character::getShotInterval() const {
	return shotInterval;
}

int Character::getCounter() const {
	return counter;
}

const std::vector<Option *> *Character::getOptionsPt() const {
	return &options;
}

int Character::damaged(int damageValue) {
	HP -= damageValue;
	return HP;
}

void Character::info() const {
	printfDx("(%f, %f), size(%d, %d)\n", point.getX(), point.getY(), image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX(), image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY());
}

void Character::optionsDraw() const {
	for(const auto &i: options) i->draw();
}
// -------------------------------------------------------------------------------------

// ------------------------- Player class ----------------------------------------------
Player::Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, std::vector<const IMG *> leftImage, std::vector<const IMG *> rightImage, unsigned long animationCount, Shape *shape, std::string shotName, int maxLife, std::vector<Option *> options) : Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, animationCount, shape, shotName, maxLife, options), centerImage(image), leftImage(leftImage), rightImage(rightImage) {
	for(auto &i: options) i->update(point.getX(), point.getY());
}

void Player::changeImage(Direction dir) {
	switch(dir) {
	case RIGHT:
		if(image == rightImage) return;
		image = rightImage;
		break;
	case LEFT:
		if(image == leftImage) return;
		image = leftImage;
		break;
	case CENTER:
		if(image == centerImage) return;
		image = centerImage;
	}
}

void Player::move(Direction dir) {
	switch(dir) {
	case CENTER:
	case UP:
	case DOWN:
		changeImage(CENTER);
		break;
	case RUP:
	case RIGHT:
	case RDOWN:
		changeImage(RIGHT);
		break;
	case LUP:
	case LEFT:
	case LDOWN:
		changeImage(LEFT);
	}

	if(dir == CENTER) return;

	point.moveX(speed * cos(dir * M_PI / 4));
	point.moveY(-1 * speed * sin(dir * M_PI / 4));
	updateShape();
}

void Player::setSpeed(double newSpeed) {
	speed = newSpeed;
}

void Player::draw() {
	optionUpdate();
	optionsDraw();
	DrawGraph(int(point.getX() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0), int(point.getY() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0), image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// -------------------------- Enemy class --------------------------------------------
Enemy::Enemy(double initPx, double initPy, std::string movePattern, double speed, double moveAngle, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, std::string shotName, int HP, std::vector<Option *> options): Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, animationCount, shape, shotName, HP, options), movePattern(movePattern), moveAngle(moveAngle), shotCnt(0), shotFlag(true) {
}

void Enemy::incShotCnt() {
	shotCnt++;
}

int Enemy::getShotCnt() const {
	return shotCnt;
}

bool Enemy::getShotFlag() const {
	return shotFlag;
}

void Enemy::draw() {
	for(auto &i: options) i->update(point.getX(), point.getY());
	DrawGraph(int(point.getX() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0), int(point.getY() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0), image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
	optionsDraw();
}
// -------------------------------------------------------------------------------------

// ---------------------- EnemyMover class ---------------------------------------------
EnemyMover::EnemyMover(): bossBuf(nullptr) {
	moveFuncTable["straight"] = &EnemyMover::straight;
	moveFuncTable["fuji"] = &EnemyMover::fuji;
	moveFuncTable["fuji_left"] = &EnemyMover::fuji_left;
	moveFuncTable["fuji_right"] = &EnemyMover::fuji_right;
	moveFuncTable["fuji_back_left"] = &EnemyMover::fuji_back_left;
	moveFuncTable["fuji_back_right"] = &EnemyMover::fuji_back_right;
}

void EnemyMover::straight(Enemy *enemy) {
	enemy->point.moveX(enemy->speed * cos(enemy->moveAngle));
	enemy->point.moveY(enemy->speed * sin(enemy->moveAngle));
	enemy->counter++;
}

void EnemyMover::fuji(Enemy *enemy) {
	bossBuf = enemy;
	if(enemy->counter < 250) {
		enemy->shotFlag = false;
		enemy->point.moveY(enemy->speed * enemy->moveAngle);
	}  else {
		enemy->shotFlag = true;
		enemy->point.moveX(enemy->speed);
		enemy->point.moveY(2.0 / 3.0 * cos(enemy->counter / 30));
		if(enemy->point.getX() > 430) enemy->speed *= -1;
		if(enemy->point.getX() < 110) enemy->speed *= -1;
	}
	enemy->counter++;
}

void EnemyMover::fuji_left(Enemy *enemy) {
	enemy->shotFlag = bossBuf->shotFlag;
	if(bossBuf->HP <= 0) {
		enemy->HP = 0;
		return;
	}
	enemy->point.setX(bossBuf->getPointPt()->getX() + 131);
	enemy->point.setY(bossBuf->getPointPt()->getY() + 23);
	enemy->counter++;
}

void EnemyMover::fuji_right(Enemy *enemy) {
	enemy->shotFlag = bossBuf->shotFlag;
	if(bossBuf->HP <= 0) {
		enemy->HP = 0;
		return;
	}
	enemy->point.setX(bossBuf->getPointPt()->getX() - 131);
	enemy->point.setY(bossBuf->getPointPt()->getY() + 23);
	enemy->counter++;
}

void EnemyMover::fuji_back_left(Enemy *enemy) {
	enemy->shotFlag = bossBuf->shotFlag;
	if(bossBuf->HP <= 0) {
		enemy->HP = 0;
		return;
	}
	enemy->point.setX(bossBuf->getPointPt()->getX() + 50);
	enemy->point.setY(bossBuf->getPointPt()->getY() - 133);
	enemy->counter++;
}	 

void EnemyMover::fuji_back_right(Enemy *enemy) {
	enemy->shotFlag = bossBuf->shotFlag;
	if(bossBuf->HP <= 0) {
		enemy->HP = 0;
		return;
	}
	enemy->point.setX(bossBuf->getPointPt()->getX() - 50);
	enemy->point.setY(bossBuf->getPointPt()->getY() - 133);
	enemy->counter++;
}
// ------------------------------------------------------------------------------------------

// ----------------------- Effect class -------------------------------------------------
Effect::Effect(): counter(0) {
}

void Effect::add(double initX, double initY, const IMG *image, double speed, double angle) {
	moveimages.emplace_back(Point(initX, initY), image, speed, angle);
}

void Effect::add(Point point, std::vector<const IMG *> image, unsigned long animationCount) {
	animimages.emplace_back(point, image, animationCount);
}

void Effect::add(Point point, std::vector<const IMG *> image, unsigned long animationCount, const Character *owner) {
	dependimages.emplace_back(std::make_tuple(point, image, animationCount), owner);
}

void Effect::applyNext() {
	double speed, angle;
	for(auto &i: moveimages) {
		speed = std::get<MEFAC_SPEED>(i), angle = std::get<MEFAC_ANGLE>(i);
		std::get<MEFAC_COORD>(i).moveX(speed * cos(angle));
		std::get<MEFAC_COORD>(i).moveY(speed * sin(angle));
	}
	counter++;
}

void Effect::drawNextMove() {
	const IMG *image;
	double harfX, harfY;
	applyNext();
	for(const auto &i: moveimages) {
		image = std::get<MEFAC_IMG>(i);
		harfX = image->getSizeX() / 2.0;
		harfY = image->getSizeY() / 2.0;
		DrawGraph((int)(std::get<MEFAC_COORD>(i).getX() - harfX), (int)(std::get<MEFAC_COORD>(i).getY() - harfY), image->getHandle(), true);
	}

	for(const auto &i: animimages) {
		image = std::get<AEFAC_IMG>(i).at((counter % (std::get<AEFAC_IMG>(i).size() * std::get<AEFAC_ANIM_COUNT>(i))) / std::get<AEFAC_ANIM_COUNT>(i));
		harfX = image->getSizeX() / 2.0;
		harfY = image->getSizeY() / 2.0;
		DrawGraph((int)(std::get<AEFAC_COORD>(i).getX() - harfX), (int)(std::get<AEFAC_COORD>(i).getY() - harfY), image->getHandle(), true);
	}

	for(const auto &i: dependimages) {
		image = std::get<AEFAC_IMG>(i.first).at((counter % (std::get<AEFAC_IMG>(i.first).size() * std::get<AEFAC_ANIM_COUNT>(i.first))) / std::get<AEFAC_ANIM_COUNT>(i.first));
		harfX = image->getSizeX() / 2.0;
		harfY = image->getSizeY() / 2.0;
		DrawGraph((int)(i.second->getPointPt()->getX() + std::get<AEFAC_COORD>(i.first).getX() - harfX), (int)(i.second->getPointPt()->getY() + std::get<AEFAC_COORD>(i.first).getY() - harfY), image->getHandle(), true);
	}
}

bool Effect::areAllEffectsOutOfArea(double areaLeft, double areaTop, double areaRight, double areaBottom) const {
	const Point *point;
	double harfX, harfY;
	if(moveimages.size() == 0) return false;
	for(const auto &i: moveimages) {
		point = &std::get<MEFAC_COORD>(i);
		harfX = std::get<MEFAC_IMG>(i)->getSizeX() / 2.0;
		harfY = std::get<MEFAC_IMG>(i)->getSizeY() / 2.0;
		if(point->getX() + harfX >= areaLeft && point->getY() + harfY >= areaTop && point->getX() - harfX <= areaRight && point->getY() - harfY <= areaBottom) return false;
	}
	return true;
}

int Effect::minAnimationLoopNum() {
	if(animimages.size() == 0) return -1;

	int ret = counter / (std::get<AEFAC_IMG>(animimages[0]).size() * std::get<AEFAC_ANIM_COUNT>(animimages[0]));
	int buf;
	for(size_t i = 1; i < animimages.size(); i++) {
		buf = counter / (std::get<AEFAC_IMG>(animimages[i]).size() * std::get<AEFAC_ANIM_COUNT>(animimages[i]));
		if(ret < buf) ret = buf;
	}
	return ret;
}
// ------------------------------------------------------------------------------------
