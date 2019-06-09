#include "STG.h"

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, const IMG *image, const IMG *shotImage): point(p), speed(speed), image(image), shotImage(shotImage) {
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

// ------------------------ Bullet class ------------------------------------------------
Bullet::Bullet(Point point, const IMG *image): point(point), image(image) {
}

void Bullet::draw() const {
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------
