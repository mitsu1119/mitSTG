#include "STG.h"

// ------------------------- Character class ------------------------------------------
Character::Character(Point p, IMG *image): point(p), image(image) {
}
// -------------------------------------------------------------------------------------

// ------------------------- Player class ----------------------------------------------
Player::Player(double initPx, double initPy, IMG *image) : Character(Point(initPx, initPy), image) {
}

void Player::info() {
	printfDx("(%f, %f), size(%d, %d)\n", point.getX(), point.getY(), image->getSizeX(), image->getSizeY());
}
// -------------------------------------------------------------------------------------