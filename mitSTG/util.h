#pragma once

class Point {
private:
	double x, y;

public:
	Point();
	Point(double x, double y);

	double getX();
	double getY();
};

class IMG {
private:
	int handle;
	int sizeX, sizeY;
	int harfSizeX, harfSizeY;

public:
	IMG(const char *path);

	int getSizeX();
	int getSizeY();
	int getHandle();
};
