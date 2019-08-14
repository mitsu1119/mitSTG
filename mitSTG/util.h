#pragma once
#include <vector>
#include <unordered_map>
#include <string>

enum Direction {
	RIGHT, RUP, UP, LUP, LEFT, LDOWN, DOWN, RDOWN, CENTER
};

Direction dirRev(Direction dir);

std::vector<std::string> split_str(const std::string &s, char delim);

class Point {
private:
	double x, y;

public:
	Point();
	Point(double x, double y);

	double getX() const;
	double getY() const;
	void moveX(double dx);
	void moveY(double dy);
	void setX(double x);
	void setY(double y);
};

class IMG {
private:
	int handle;
	int sizeX, sizeY;

public:
	IMG();
	IMG(const char *path);

	int getSizeX() const;
	int getSizeY() const;
	int getHandle() const;
};