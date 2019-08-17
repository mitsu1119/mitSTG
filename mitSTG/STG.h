#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <deque>
#include <tuple>
#include <unordered_map>
#include "util.h"
#include "collision.h"
#include "DxLib.h"

// -------------------------------------------------------- Stage -------------------------------------------------------------------
// stage[0][STG_ENEMYIMG] => 1th enemy's image
typedef std::tuple<std::string, double, double, int, std::string, double, double, std::string, std::string, double, int> StagePart;
typedef std::vector<StagePart> Stage;
enum StageAccessing {
	STA_ENEMYNAME, STA_INITPX, STA_INITPY, STA_TIMING, STA_MOVEPATTERN, STA_MOVESPEED, STA_MOVEANGLE, STA_SHOTNAME, STA_SHOTPATTERN, STA_SHOTSPEED, STA_SHOTINTERVAL
};
// -----------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------- Shot -----------------------------------------------------------------
class ShotMover;
class Character;
class Shot {
friend ShotMover;
private:
	Point point;
	Shape *shape;
	const IMG *image;
	std::string movePattern;
	double speed, angle;
	int counter;

	const Character *target;

	// Damage value to the enemy when the owner is player.
	int damagePower;

	// The number of bullet shot by an enemy
	int number;

	void updateShape() const;

public:
	// power: For player shot.
	// number, target: For enemy shot.
	Shot(Point point, double speed, std::string movePattern, const IMG *image, Shape *shape, int power = 0, int number = 0, const Character *target = nullptr);

	Point getImageSize() const;
	const Shape *getShapePt() const;
	const Point *getPointPt() const;
	void moveX(double dx);
	void moveY(double dy);
	std::string getMovePattern() const;
	int getPower() const;
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
		shot->updateShape();
	}
};
// ---------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Character --------------------------------------------------------------
// base
class Character {
protected:
	Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotName, int HP);
	
	Point point;
	Shape *shape;
	const IMG *image;
	std::string shotName, shotPattern;
	double speed, shotSpeed;
	int counter, shotInterval, HP;

	void updateShape();

public:
	void setCoord(double x, double y);
	int damaged(int damageValue);

	Point getPoint() const;
	const Shape *getShapePt() const;
	const Point *getPointPt() const;
	const IMG *getImage() const;
	std::string getShotName() const;
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
private:
	const IMG *deathEffectImage;

public:
	Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotName, int maxLife, const IMG *deathEffectImage);

	void move(Direction dir);
	void setSpeed(double newSpeed);

	const IMG *getDeathEffectImage() const;
};

// enemy
class EnemyMover;
class Enemy: public Character {
friend EnemyMover;
private:
	// The number of bullets shot by this enemy
	int shotCnt;
	
	std::string movePattern;
	double moveAngle;

public:
	Enemy(double initPx, double initPy, std::string movePattern, double speed, double moveAngle, std::string shotPattern, double shotSpeed, int shotInterval, const IMG *image, Shape *shape, std::string shotImage, int HP);

	void incShotCnt();

	int getShotCnt() const;
};

class EnemyMover {
private:
	typedef void(EnemyMover:: *EFUNC)(Enemy *enemy);
	std::unordered_map<std::string, EFUNC> moveFuncTable;
	
	void straight(Enemy *enemy);
	
public:
	EnemyMover();

	void operator ()(Enemy *enemy) {
		(this->*moveFuncTable[enemy->movePattern])(enemy);
		enemy->updateShape();
	}
};
// ---------------------------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------- Effect ---------------------------------------------------------------
class Effect {
private:
	typedef std::tuple<Point, const IMG *, double, double> MoveEffectElement;
	enum EffectAccessor {
		EFAC_COORD, EFAC_IMG, EFAC_SPEED, EFAC_ANGLE
	};
	std::vector<MoveEffectElement> moveimages;
	
	typedef std::vector<const IMG *> AnimEffectElement;

	void applyNext();

public:
	void add(double initX, double initY, const IMG * image, double speed, double angle);
	void drawNextMove();

	bool areAllEffectsOutOfArea(double areaLeft, double areaTop, double areaRight, double areaBottom) const;
};
// ---------------------------------------------------------------------------------------------------------------------------------
