#pragma once
#include <vector>

enum Direction {
	RIGHT, RUP, UP, LUP, LEFT, LDOWN, DOWN, RDOWN, CENTER
};

class Point {
private:
	double x, y;

public:
	Point();
	Point(double x, double y);

	double getX();
	double getY();
	void moveX(double dx);
	void moveY(double dy);
};

class IMG {
private:
	int handle;
	int sizeX, sizeY;

public:
	IMG(const char *path);

	int getSizeX();
	int getSizeY();
	int getHandle();
};
typedef std::vector<IMG *> IMGDataBase;