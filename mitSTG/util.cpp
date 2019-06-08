#include "util.h"
#include "DxLib.h"

// ------------------------- Point class ------------------------------------------------
Point::Point(): x(0), y(0) {
}

Point::Point(double x, double y) : x(x), y(y) {
}

double Point::getX() {
	return x;
}

double Point::getY() {
	return y;
}

void Point::moveX(double dx) {
	x += dx;
}

void Point::moveY(double dy) {
	y += dy;
}
// --------------------------------------------------------------------------------------

// ------------------------- IMG class --------------------------------------------------
IMG::IMG(const char *path) {
	handle = LoadGraph(path);
	GetGraphSize(handle, &sizeX, &sizeY);
	harfSizeX = sizeX / 2;
	harfSizeY = sizeY / 2;
}

int IMG::getSizeX() {
	return sizeX;
}

int IMG::getSizeY() {
	return sizeY;
}

int IMG::getHandle() {
	return handle;
}
// ----------------------------------------------------------------------------------------