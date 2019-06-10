#include "STG.h"

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Point point, int movePattern, const IMG *image): point(point), movePattern(movePattern), image(image) {
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

int Shot::getMovePattern() const {
	return movePattern;
}

void Shot::draw() const {
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- ShotMover class -----------------------------------------
ShotMover::ShotMover() {
	moveFuncList.push_back(&ShotMover::target);
}

void ShotMover::target(Shot *shot) {
	shot->moveX(5.0);
	shot->moveY(5.0);
}
// --------------------------------------------------------------------------------------

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, const IMG *image, const IMG *shotImage): point(p), speed(speed), image(image), shotImage(shotImage) {
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
Enemy::Enemy(double initPx, double initPy, double speed, const IMG *image, const IMG *shotImage): Character(Point(initPx, initPy), speed, image, shotImage) {
}

void Enemy::move(Direction dir) {
}
// -------------------------------------------------------------------------------------
