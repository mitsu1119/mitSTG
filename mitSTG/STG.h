#pragma once
#include "util.h"
#include "DxLib.h"

class Character {
protected:
	Character(Point p, IMG *image);
	
	Point point;
	IMG *image;
};

class Player: public Character {
public:
	Player(double initPx, double initPy, IMG *image);

	void info();	// for debug
};