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
typedef std::tuple<const IMG *, double, double, std::string, double, int, int, const IMG *> StagePart;
typedef std::vector<StagePart> Stage;
enum StageAccessing {
	STA_ENEMYIMG, STA_INITPX, STA_INITPY, STA_SPATTERN, STA_SPEED, STA_INTERVAL, STA_TIMING, STA_SHOTIMG
};
// -----------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------- Shot -----------------------------------------------------------------
class ShotMover;
class Shot {
friend ShotMover;
private:
	Point point;
	const IMG *image;
	std::string movePattern;
	double speed, angle;
	int counter;

public:
	Shot(Point point, double speed, std::string movePattern, const IMG *image);

	Point getImageSize() const;
	const Point *getPointPt() const;
	void moveX(double dx);
	void moveY(double dy);
	std::string getMovePattern() const;
	void draw() const;
};
// ---------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Shot Mover ------------------------------------------------------------
class Player;
class ShotMover {
private:
	typedef void(ShotMover:: *SFUNC)(Shot *shot);
	std::unordered_map<std::string, SFUNC> moveFuncTable;
	const Player *player;

	void target(Shot *shot);

public:
	ShotMover(const Player *player);

	void operator ()(Shot *shot) {
		(this->*moveFuncTable[shot->getMovePattern()])(shot);
	}
};
// ---------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Character --------------------------------------------------------------
// base
class Character {
protected:
	Character(Point p, double speed, const IMG *mage, const IMG *shotImage);
	
	Point point;
	const IMG *image, *shotImage;
	double speed;
	int counter;

public:
	virtual void move(Direction dir) = 0;

	Point getPoint() const;
	const Point *getPointPt() const;
	const IMG *getImage() const;
	const IMG *getShotImage() const;
	double getSpeed() const;
	int getCounter() const;
	void info() const;		// for debug
	void draw() const;
};

// player
class Player: public Character {
public:
	Player(double initPx, double initPy, double speed, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
};

// enemy
class Enemy: public Character {
private:
	std::string shotPattern;
	double shotSpeed;
	int shotInterval;

public:
	Enemy(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
	std::string getShotPattern();
	double getShotSpeed();
	int getShotInterval();
};
// ---------------------------------------------------------------------------------------------------------------------------------
