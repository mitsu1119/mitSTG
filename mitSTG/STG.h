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
class Character;
class Shot {
friend ShotMover;
private:
	Point point;
	const IMG *image;
	std::string movePattern;
	double speed, angle;
	int counter;

	const Character *target;

	// The number of bullet shot by an enemy
	int number;

public:
	Shot(Point point, double speed, std::string movePattern, const IMG *image, int number = 0, const Character *target = nullptr);

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

	void player1(Shot *shot);
	void player2(Shot *shot);
	void target(Shot *shot);
	void swirl(Shot *shot);

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
	Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, const IMG *shotImage);
	
	Point point;
	const IMG *image, *shotImage;
	std::string shotPattern;
	double speed, shotSpeed;
	int counter, shotInterval;

public:
	virtual void move(Direction dir) = 0;

	Point getPoint() const;
	const Point *getPointPt() const;
	const IMG *getImage() const;
	const IMG *getShotImage() const;
	std::string getShotPattern() const;
	double getSpeed() const;
	double getShotSpeed() const;
	int getShotInterval() const;
	int getCounter() const;
	void info() const;		// for debug
	void draw() const;
};

// player
class Player: public Character {
public:
	Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
};

// enemy
class Enemy: public Character {
private:
	// The number of bullets shot by this enemy
	int shotCnt;

public:
	Enemy(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, const IMG *shotImage);

	virtual void move(Direction dir);
	void incShotCnt();
	int getShotCnt();
};
// ---------------------------------------------------------------------------------------------------------------------------------
