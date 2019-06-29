#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "collision.h"
#include "STG.h"

constexpr size_t MAX_ENEMY_DISP = 20;
constexpr size_t MAX_SHOT_DISP = 300;
class Game {
private:
	Player *player;
	
	Stage stage;
	int leftX, rightX, topY, bottomY;		// for drawing area
	
	Direction keyDirection;
	
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

	StagePart getNextEnemyData();
	int getNextEnemyTiming();

	void playerKeyProcessing();
	void enemyProcessing();

	void enemyShotFlagProcessing();
	void playerShotFlagProcessing();

	void enemyShotMoving();
	void playerShotMoving();

	void collisionProcessing();

	void playerAndEnemyShotDrawing();
	void enemyDrawing();

public:
	Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages, const IMGDataBase &shotImages, int leftX, int topY, int rightX, int bottomY);
	~Game();

	void mainLoop();
};