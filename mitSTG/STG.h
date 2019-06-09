#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <deque>
#include <tuple>
#include <unordered_map>
#include "util.h"
#include "DxLib.h"

// -------------------------------------------------------- Stage -------------------------------------------------------------------
// stage[0][STG_ENEMYIMG] => 1th enemy's image
typedef std::tuple<const IMG *, double, double, int, const IMG *> StagePart;
typedef std::vector<StagePart> Stage;
enum StageAccessing {
	STA_ENEMYIMG, STA_INITPX, STA_INITPY, STA_TIMING, STA_SHOTIMG
};
// -----------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Character --------------------------------------------------------------
class Character {
protected:
	Character(Point p, double speed, const IMG *mage, const IMG *shotImage);
	
	Point point;
	const IMG *image, *shotImage;
	double speed;

public:
	virtual void move(Direction dir) = 0;
	void info() const;		// for debug
	void draw() const;
};

class Player: public Character {
public:
	Player(double initPx, double initPy, double speed, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
};

class Enemy: public Character {
public:
	Enemy(double initPx, double initPy, double speed, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
};
// ---------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------- Shot -----------------------------------------------------------------
class Bullet {
private:
	Point point;
	const IMG *image;

public:
	Bullet(Point point, const IMG *image);

	void draw() const;
};
// ---------------------------------------------------------------------------------------------------------------------------------
