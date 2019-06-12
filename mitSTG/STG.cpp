#include "STG.h"

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Point point, double speed, std::string movePattern, const IMG *image): point(point), speed(speed), movePattern(movePattern), image(image), angle(0.0), counter(0) {
}

Point Shot::getImageSize() const {
	return Point(image->getSizeX(), image->getSizeY());
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
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- ShotMover class -----------------------------------------
ShotMover::ShotMover(const Player *player): player(player) {
	moveFuncTable["target"] = &ShotMover::target;
}

void ShotMover::target(Shot *shot) {
	if(shot->counter == 0) shot->angle = atan2(player->getPointPt()->getY() - shot->point.getY(), player->getPointPt()->getX() - shot->point.getX());
	shot->moveX(shot->speed * cos(shot->angle));
	shot->moveY(shot->speed * sin(shot->angle));
	shot->counter++;
}
// --------------------------------------------------------------------------------------

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, const IMG *image, const IMG *shotImage): point(p), speed(speed), image(image), shotImage(shotImage), counter(0) {
}

Point Character::getPoint() const {
	return point;
}

const Point *Character::getPointPt() const {
	return &point;
}

const IMG *Character::getImage() const {
	return image;
}

const IMG *Character::getShotImage() const {
	return shotImage;
}

double Character::getSpeed() const {
	return speed;
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
Player::Player(double initPx, double initPy, double speed, const IMG *image, const IMG *shotImage) : Character(Point(initPx, initPy), speed, image, shotImage) {
}

void Player::move(Direction dir) {
	if(dir == CENTER) return;
	point.moveX(speed * cos(dir * M_PI / 4));
	point.moveY(-1 * speed * sin(dir * M_PI / 4));
}
// -------------------------------------------------------------------------------------

// -------------------------- Enemy class --------------------------------------------
Enemy::Enemy(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, const IMG *shotImage): Character(Point(initPx, initPy), speed, image, shotImage), shotPattern(shotPattern), shotSpeed(shotSpeed), shotInterval(shotInterval) {
}

void Enemy::move(Direction dir) {
	counter++;
}

std::string Enemy::getShotPattern() {
	return shotPattern;
}

double Enemy::getShotSpeed() {
	return shotSpeed;
}

int Enemy::getShotInterval() {
	return shotInterval;
}
// -------------------------------------------------------------------------------------
