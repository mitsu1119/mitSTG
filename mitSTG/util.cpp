#include "util.h"
#include "DxLib.h"

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