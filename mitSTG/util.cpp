#include "util.h"
#include "DxLib.h"

// ------------------------- Direction -------------------------------------------------
Direction dirRev(Direction dir) {
	switch(dir) {
	case RIGHT:
		return LEFT;
	case RUP:
		return LDOWN;
	case UP:
		return DOWN;
	case LUP:
		return RDOWN;
	case LEFT:
		return RIGHT;
	case LDOWN:
		return RUP;
	case DOWN:
		return UP;
	case RDOWN:
		return LUP;
	default:
		return dir;
	}
}

// ------------------------- out of all class --------------------------------------------
std::vector<std::string> split_str(const std::string &s, char delim) {
	std::vector<std::string> ret;
	std::string itm;
	for(char chr : s) {
		if(chr == delim) {
			if(!itm.empty()) ret.emplace_back(itm);
			itm.clear();
		} else {
			itm += chr;
		}
	}
	if(!itm.empty()) ret.emplace_back(itm);
	return ret;
}

// ------------------------- Point class ------------------------------------------------
Point::Point(): x(0), y(0) {
}

Point::Point(double x, double y) : x(x), y(y) {
}

double Point::getX() const {
	return x;
}

double Point::getY() const {
	return y;
}

void Point::moveX(double dx) {
	x += dx;
}

void Point::moveY(double dy) {
	y += dy;
}

void Point::setX(double x) {
	this->x = x;
}

void Point::setY(double y) {
	this->y = y;
}
// --------------------------------------------------------------------------------------

// ------------------------- IMG class --------------------------------------------------
IMG::IMG(const char *path) {
	handle = LoadGraph(path);
	GetGraphSize(handle, &sizeX, &sizeY);
}

IMG::IMG(int imageHandle) {
	handle = imageHandle;
	GetGraphSize(handle, &sizeX, &sizeY);
}

IMG::IMG() {
	handle = 0;
	sizeX = 0;
	sizeY = 0;
}

int IMG::getSizeX() const {
	return sizeX;
}

int IMG::getSizeY() const {
	return sizeY;
}

int IMG::getHandle() const {
	return handle;
}
// ----------------------------------------------------------------------------------------
