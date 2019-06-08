#include "STG.h"

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, double speed, IMG *image): point(p), speed(speed), image(image) {
}

void Character::info() {
	printfDx("(%f, %f), size(%d, %d)\n", point.getX(), point.getY(), image->getSizeX(), image->getSizeY());
}

void Character::draw() {
	DrawGraph(int(point.getX() - (double)image->getSizeX() / 2.0), int(point.getY() - (double)image->getSizeY() / 2.0), image->getHandle(), true);
}
// -------------------------------------------------------------------------------------

// ------------------------- Player class ----------------------------------------------
Player::Player(double initPx, double initPy, double speed, IMG *image) : Character(Point(initPx, initPy), speed, image) {
}

void Player::move(Direction dir) {
	if(dir == CENTER) return;
	point.moveX(speed * cos(dir * M_PI / 4));
	point.moveY(-1 * speed * sin(dir * M_PI / 4));
}
// -------------------------------------------------------------------------------------

// -------------------------- Enemy class --------------------------------------------
Enemy::Enemy(double initPx, double initPy, double speed, IMG *image): Character(Point(initPx, initPy), speed, image) {
}

void Enemy::move(Direction dir) {
}
// -------------------------------------------------------------------------------------