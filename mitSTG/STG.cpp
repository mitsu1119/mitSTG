#include "STG.h"

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Point point, double speed, std::string movePattern, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, int power, int number, const Character *target): point(point), shape(shape), speed(speed), movePattern(movePattern), image(image), angle(0.0), counter(0), number(number), animationNum(image.size()), animationCount(animationCount), damagePower(power), target(target) {
}

void Shot::updateShape() const {
	double harfX = image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0, harfY = image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0;
	if(shape->getType() == RECT_SHAPE) shape->resetCoord(point.getX() - harfX, point.getY() - harfY, point.getX() + harfX, point.getY() + harfY);
	else shape->resetCoord(point.getX(), point.getY());
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

int Shot::getPower() const {
	return damagePower;
}

void Shot::draw() const {
	DrawRotaGraph((int)point.getX(), (int)point.getY(), 1.0, angle + M_PI / 2, image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- ShotMover class -----------------------------------------
ShotMover::ShotMover(const Player *player): player(player) {
	moveFuncTable["player1"] = &ShotMover::player1;
	moveFuncTable["player2"] = &ShotMover::player2;
	moveFuncTable["target"] = &ShotMover::target;
	moveFuncTable["swirl"] = &ShotMover::swirl;
}

void ShotMover::player1(Shot *shot) {
	shot->moveY(shot->speed);
	shot->angle = -M_PI / 2.0;
	shot->counter++;
}

void ShotMover::player2(Shot *shot) {
	double t = (1.0 / shot->speed) * shot->counter;
	double p0x = player->getPointPt()->getX(), p0y = player->getPointPt()->getY();
	double p3x = shot->target->getPointPt()->getX(), p3y = shot->target->getPointPt()->getY();
	double rad = atan2(p0y - p3y, p0x - p3x);
	double p1x = p0x - 20, p1y = p0y + 200;
	double p2x = (p0x + p3x) / 2 + 3 * cos(rad + M_PI / 2), p2y = (p0y + p3y) / 2 + 3 * sin(rad + M_PI / 2);
	double x = (1 - t) * (1 - t) * (1 - t) * p0x + 3 * (1 - t) * (1 - t) * t * p1x + 3 * (1 - t) * t * t * p2x + t * t * t * p3x;
	double y = (1 - t) * (1 - t) * (1 - t) * p0y + 3 * (1 - t) * (1 - t) * t * p1y + 3 * (1 - t) * t * t * p2y + t * t * t * p3y;
	shot->angle = atan2(shot->point.getY() - y, shot->point.getX() - x);
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

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount,  Shape *shape, std::string shotName, int HP): animationNum(image.size()), animationCount(animationCount), point(p), speed(speed), shotPattern(shotPattern), shotSpeed(shotSpeed), shotInterval(shotInterval), image(image), shape(shape), shotName(shotName), counter(0), HP(HP) {
}

void Character::updateShape() {
	double harfX = image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0, harfY = image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0;
	if(shape->getType() == RECT_SHAPE) shape->resetCoord(point.getX() - harfX, point.getY() - harfY, point.getX() + harfX, point.getY() + harfY);
	else shape->resetCoord(point.getX(), point.getY());
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

int Character::damaged(int damageValue) {
	HP -= damageValue;
	return HP;
}

void Character::info() const {
	printfDx("(%f, %f), size(%d, %d)\n", point.getX(), point.getY(), image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX(), image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY());
}

void Character::draw() const {
	DrawGraph(int(point.getX() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeX() / 2.0), int(point.getY() - (double)image[(counter % (animationNum * animationCount)) / animationCount]->getSizeY() / 2.0), image[(counter % (animationNum * animationCount)) / animationCount]->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- Player class ----------------------------------------------
Player::Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, std::vector<const IMG *> leftImage, std::vector<const IMG *> rightImage, unsigned long animationCount, Shape *shape, std::string shotName, int maxLife, const IMG *deathEffectImage) : Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, animationCount, shape, shotName, maxLife), centerImage(image), leftImage(leftImage), rightImage(rightImage), deathEffectImage(deathEffectImage) {
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

const IMG *Player::getDeathEffectImage() const {
	return deathEffectImage;
}
// -------------------------------------------------------------------------------------

// -------------------------- Enemy class --------------------------------------------
Enemy::Enemy(double initPx, double initPy, std::string movePattern,  double speed, double moveAngle, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, std::string shotName, int HP): Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, animationCount, shape, shotName, HP), movePattern(movePattern), moveAngle(moveAngle), shotCnt(0) {
}

void Enemy::incShotCnt() {
	shotCnt++;
}

int Enemy::getShotCnt() const {
	return shotCnt;
}
// -------------------------------------------------------------------------------------

// ---------------------- EnemyMover class ---------------------------------------------
EnemyMover::EnemyMover() {
	moveFuncTable["straight"] = &EnemyMover::straight;
}

void EnemyMover::straight(Enemy *enemy) {
	enemy->point.moveX(enemy->speed * cos(enemy->moveAngle));
	enemy->point.moveY(enemy->speed * sin(enemy->moveAngle));
	enemy->counter++;
}
// ------------------------------------------------------------------------------------------

// ----------------------- Effect class -------------------------------------------------
void Effect::add(double initX, double initY, const IMG *image, double speed, double angle) {
	moveimages.emplace_back(Point(initX, initY), image, speed, angle);
}

void Effect::applyNext() {
	double speed, angle;
	for(auto &i: moveimages) {
		speed = std::get<EFAC_SPEED>(i), angle = std::get<EFAC_ANGLE>(i);
		std::get<EFAC_COORD>(i).moveX(speed * cos(angle));
		std::get<EFAC_COORD>(i).moveY(speed * sin(angle));
	}
}

void Effect::drawNextMove() {
	const IMG *image;
	double harfX, harfY;
	applyNext();
	for(const auto &i: moveimages) {
		image = std::get<EFAC_IMG>(i);
		harfX = image->getSizeX() / 2.0;
		harfY = image->getSizeY() / 2.0;
		DrawGraph((int)(std::get<EFAC_COORD>(i).getX() - harfX), (int)(std::get<EFAC_COORD>(i).getY() - harfY), image->getHandle(), true);
	}
}

bool Effect::areAllEffectsOutOfArea(double areaLeft, double areaTop, double areaRight, double areaBottom) const {
	const Point *point;
	double harfX, harfY;
	if(moveimages.size() == 0) return false;
	for(const auto &i: moveimages) {
		point = &std::get<EFAC_COORD>(i);
		harfX = std::get<EFAC_IMG>(i)->getSizeX() / 2.0;
		harfY = std::get<EFAC_IMG>(i)->getSizeY() / 2.0;
		if(point->getX() + harfX >= areaLeft && point->getY() + harfY >= areaTop && point->getX() - harfX <= areaRight && point->getY() - harfY <= areaBottom) return false;
	}
	return true;
}
// ------------------------------------------------------------------------------------
