#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include "util.h"
#include "DxLib.h"

class Character {
protected:
	Character(Point p, double speed, IMG *image);
	
	Point point;
	IMG *image;
	double speed;

public:
	virtual void move(Direction dir) = 0;
};

class Player: public Character {
public:
	Player(double initPx, double initPy, double speed, IMG *image);

	void info();	// for debug
	virtual void move(Direction dir);
};