#include "STG.h"

// ------------------------ Bullet class ------------------------------------------------
Bullet::Bullet(Point point, const IMG *image): point(point), image(image) {
}

Point Bullet::getImageSize() {
	return Point((double)image->getSizeX(), (double)image->getSizeY());
}

const Point *Bullet::getPointPt() {
	return &point;
}

void Bullet::moveX(double dx) {
	point.moveX(dx);
}

void Bullet::moveY(double dy) {
	point.moveY(dy);
}

void Bullet::draw() const {
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------ Shot class -------------------------------------------------
Shot::Shot(Bullet bullet, int movePattern): bullet(bullet), movePattern(movePattern) {
}

Point Shot::getImageSize() {
	return bullet.getImageSize();
}

const Point *Shot::getPointPt() {
	return bullet.getPointPt();
}

void Shot::moveX(double dx) {
	bullet.moveX(dx);
}

void Shot::moveY(double dy) {
	bullet.moveY(dy);
}

int Shot::getMovePattern() const {
	return movePattern;
}

void Shot::draw() const {
	bullet.draw();
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
