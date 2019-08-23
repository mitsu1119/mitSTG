#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <deque>
#include <tuple>
#include <random>
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
	std::vector<const IMG *> image;
	size_t animationNum;
	unsigned long animationCount;

	bool lazerFlag;
	int lazerState;		// 0: not existing, 1: existing, 2: existing only tracing
	size_t tracingSize;
	std::deque<Point> tracing;

	std::string movePattern;
	double speed, angle;
	int counter;

	Character *target;

	// Damage value to the enemy when the owner is player.
	int damagePower;

	// The number of bullet shot by an enemy
	int number;

	void updateShape() const;

public:
	// power: For player shot.
	// number, target: For enemy shot.
	Shot(Point point, double speed, std::string movePattern, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, int power = 0, bool isLazer = false, int number = 0, Character *target = nullptr);

	void setNullTarget();
	void incLazerState();

	Point getImageSize() const;
	const Shape *getShapePt() const;
	const Point *getPointPt() const;
	void moveX(double dx);
	void moveY(double dy);
	std::string getMovePattern() const;
	Character *getTarget() const;
	int getPower() const;
	bool isLazer() const;
	bool lazerDestroyableFlag() const;
	void draw();
};
// ---------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Shot Mover ------------------------------------------------------------
class Player;
class ShotMover {
private:
	std::mt19937 mt;
	std::uniform_real_distribution<> realRand;

	typedef void(ShotMover:: *SFUNC)(Shot *shot);
	std::unordered_map<std::string, SFUNC> moveFuncTable;
	const Player *player;

	void player1(Shot *shot);
	void option1(Shot *shot);
	void option2(Shot *shot);
	void target(Shot *shot);
	void swirl(Shot *shot);
	void evdir(Shot *shot);
	void random(Shot *shot);

public:
	ShotMover(const Player *player);

	static int getWayNum(std::string shotPattern);

	void operator ()(Shot *shot) {
		(this->*moveFuncTable[shot->getMovePattern()])(shot);
		shot->updateShape();
	}
};
// ---------------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------- Option --------------------------------------------------------------
class Character;
class Option {
private:
	// point: distance to player, coord: the option's coordinate.
	Point point, coord;
	std::vector<const IMG *> image;
	size_t animationNum;
	unsigned long animationCount;

	std::string shotName, shotPattern;
	double shotSpeed;
	int shotInterval, shotCnt;
	
	int counter;

public:
	Option(Point p, std::vector<const IMG *> image, unsigned long animationCount, std::string shotName, std::string shotPattern, double shotSpeed, int shotInterval);

	const Point *getCoordPt() const;
	std::string getShotName() const;
	std::string getShotPattern() const;
	double getShotSpeed() const;
	int getShotInterval() const;
	int getShotCnt() const;

	void update(double ownerX, double ownerY);
	void incShotCnt();
	void draw() const;
};

// --------------------------------------------------------- Character --------------------------------------------------------------
// base
class Character {
protected:
	Character(Point p, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, std::string shotName, int HP, std::vector<Option *> options);
	
	Point point;
	Shape *shape;
	std::vector<const IMG *> image;
	std::vector<Option *> options;
	size_t animationNum;
	unsigned long animationCount;

	std::string shotName, shotPattern;
	double speed, shotSpeed;
	int counter, shotInterval, HP;

	void updateShape();

public:
	void setCoord(double x, double y);
	int damaged(int damageValue);
	void optionUpdate();
	virtual void draw() = 0;

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
	const std::vector<Option *> *getOptionsPt() const;
	void info() const;		// for debug
	void optionsDraw() const;
};

// player
class Player: public Character {
private:
	std::vector<const IMG *> centerImage, leftImage, rightImage;

	void changeImage(Direction dir);

public:
	Player(double initPx, double initPy, double speed, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, std::vector<const IMG *> leftImage, std::vector<const IMG *> rightImage, unsigned long animationCount, Shape *shape, std::string shotName, int maxLife, std::vector<Option *> options);

	void move(Direction dir);
	void setSpeed(double newSpeed);

	virtual void draw();
};

// enemy
class EnemyMover;
class Enemy: public Character {
friend EnemyMover;
private:
	// The number of bullets shot by this enemy
	int shotCnt;

	bool shotFlag;
	
	std::string movePattern;
	double moveAngle;

public:
	Enemy(double initPx, double initPy, std::string movePattern, double speed, double moveAngle, std::string shotPattern, double shotSpeed, int shotInterval, std::vector<const IMG *> image, unsigned long animationCount, Shape *shape, std::string shotImage, int HP, std::vector<Option *> options);

	void incShotCnt();
	virtual void draw();

	int getShotCnt() const;
	bool getShotFlag() const;
};

class EnemyMover {
private:
	typedef void(EnemyMover:: *EFUNC)(Enemy *enemy);
	std::unordered_map<std::string, EFUNC> moveFuncTable;

	Enemy *bossBuf;
	
	void straight(Enemy *enemy);
	void straight_down(Enemy *enemy);
	void straight_stop(Enemy *enemy);
	void straight_back(Enemy *enemy);
	void left_to_right_revu(Enemy *enemy);
	void right_to_left_revu(Enemy *enemy);
	void fuji(Enemy *enemy);
	void fuji_left(Enemy *enemy);
	void fuji_right(Enemy *enemy);
	void fuji_back_right(Enemy *enemy);
	void fuji_back_left(Enemy *enemy);

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
	enum MoveEffectAccessor {
		MEFAC_COORD, MEFAC_IMG, MEFAC_SPEED, MEFAC_ANGLE
	};
	std::vector<MoveEffectElement> moveimages;
	
	typedef std::tuple<Point, std::vector<const IMG *>, unsigned long> AnimEffectElement;
	enum AnimEffectAccessor {
		AEFAC_COORD, AEFAC_IMG, AEFAC_ANIM_COUNT
	};
	std::vector<AnimEffectElement> animimages;

	std::vector<std::pair<AnimEffectElement, const Character *>> dependimages;
	
	int counter;

	void applyNext();

public:
	Effect();

	void add(double initX, double initY, const IMG * image, double speed, double angle);
	void add(Point point, std::vector<const IMG *> image, unsigned long animationCount);
	void add(Point point, std::vector<const IMG *> image, unsigned long animationCount, const Character *owner);
	void drawNextMove();

	bool areAllEffectsOutOfArea(double areaLeft, double areaTop, double areaRight, double areaBottom) const;
	int minAnimationLoopNum();
};
// ---------------------------------------------------------------------------------------------------------------------------------
