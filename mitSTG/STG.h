#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "util.h"
#include "DxLib.h"

// stage[0][STG_ENEMYIMG] => 1th enemy's image
typedef std::vector<std::tuple<const IMG *, int>> Stage;
enum StageAccessing {
	STG_ENEMYIMG, STG_TIMING
};

class Character {
protected:
	Character(Point p, double speed, const IMG *mage);
	
	Point point;
	const IMG *image;
	double speed;

public:
	virtual void move(Direction dir) = 0;
	void info();		// for debug
	void draw();
};

class Player: public Character {
public:
	Player(double initPx, double initPy, double speed, const IMG *image);

	virtual void move(Direction dir);
};

class Enemy: public Character {
public:
	Enemy(double initPx, double initPy, double speed, const IMG *image);

	virtual void move(Direction dir);
};
