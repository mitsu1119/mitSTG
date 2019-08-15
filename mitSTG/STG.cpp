#include "STG.h"

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Point point, double speed, std::string movePattern, const IMG *image, Shape *shape, int number, const Character *target): point(point), shape(shape), speed(speed), movePattern(movePattern), image(image), angle(0.0), counter(0), number(number), target(target) {
}

Point Shot::getImageSize() const {
	return Point(image->getSizeX(), image->getSizeY());
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

void Shot::draw() const {
	DrawRotaGraph((int)point.getX(), (int)point.getY(), 1.0, angle + M_PI / 2, image->getHandle(), true);
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
Character::Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotName): point(p), speed(speed), shotPattern(shotPattern), shotSpeed(shotSpeed), shotInterval(shotInterval), image(image), shape(shape), shotName(shotName), counter(0) {
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
	return image;
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

void Character::info() const {
	printfDx("(%f, %f), size(%d, %d)\n", point.getX(), point.getY(), image->getSizeX(), image->getSizeY());
}

void Character::draw() const {
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- Player class ----------------------------------------------
Player::Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotName) : Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, shape, shotName) {
}

void Player::move(Direction dir) {
	if(dir == CENTER) return;
	point.moveX(speed * cos(dir * M_PI / 4));
	point.moveY(-1 * speed * sin(dir * M_PI / 4));
	
	double playerHarfX = image->getSizeX() / 2.0, playerHarfY = image->getSizeY() / 2.0;
	if(shape->getType() == RECT_SHAPE) shape->resetCoord(point.getX() - playerHarfX, point.getY() - playerHarfY, point.getX() + playerHarfX, point.getY() + playerHarfY);
	else shape->resetCoord(point.getX(), point.getY());
}
// -------------------------------------------------------------------------------------

// -------------------------- Enemy class --------------------------------------------
Enemy::Enemy(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotName): Character(Point(initPx, initPy), speed, shotPattern, shotSpeed, shotInterval, image, shape, shotName), shotCnt(0) {
}

void Enemy::move(Direction dir) {
	counter++;
}

void Enemy::incShotCnt() {
	shotCnt++;
}

int Enemy::getShotCnt() const {
	return shotCnt;
}
// -------------------------------------------------------------------------------------
