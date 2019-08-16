#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "collision.h"
#include "STG.h"

#define WHITE GetColor(255, 255, 255)
#define RED GetColor(255, 0, 0)
#define GREEN GetColor(0, 255, 0)

typedef std::unordered_map<std::string, std::tuple<const IMG *, std::string, double, double, int>> CharDataBase;
enum CharDBAccessor {
	CHDB_IMG, CHDB_SHAPE, CHDB_SHAPE_DATA1, CHDB_SHAPE_DATA2, CHDB_HP_OR_POWER
};

constexpr size_t MAX_ENEMY_DISP = 20;
constexpr size_t MAX_SHOT_DISP = 300;
class Game {
private:
	Player *player;
	double playerOriginalSpeed;
	const CharDataBase &enemDB, &shotDB;
	
	Stage stage;
	const IMG *bgImg, *lifeImg;
	int bgY, scrollSpeed;
	int leftX, rightX, topY, bottomY;		// for drawing area
	
	Direction keyDirection;
	bool checkKeyPShotBt, checkKeyLowPlayer;
	int timeOfLastPShot;
	
	std::vector<Enemy *> enemyPool;
	std::vector<bool> enemyPoolFlags;
	std::vector <Shot *> shotPool;
	std::vector<bool> shotPoolFlags;
	std::vector<Shot *> playerShotPool;
	std::vector<bool> playerShotPoolFlags;
	ShotMover *smover;
	
	size_t enemCount;
	ShapeCollider *collider;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

	// For debug
	void drawShape(const Shape &shape);

	StagePart getNextEnemyData();
	int getNextEnemyTiming();
	void destroyEnemyPool(size_t index);
	void destroyEshotPool(size_t index);
	void destroyPshotPool(size_t index);

	void playerKeyProcessing();
	void enemyProcessing();

	void enemyShotFlagProcessing();
	void playerShotFlagProcessing();

	void enemyShotMoving();
	void playerShotMoving();

	void bgProcessing();
	void collisionProcessing();

	void bgDrawing();
	void systemDrawing();
	void playerAndEnemyShotDrawing();
	void enemyDrawing();
	void shapeDrawing();		// debug

public:
	Game(Player *player, const char *stagePath, const CharDataBase &enemyDB, const CharDataBase &shotDB, int leftX, int topY, int rightX, int bottomY, const IMG *lifeImg);
	~Game();

	void mainLoop();
};
