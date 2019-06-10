#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "STG.h"

constexpr size_t MAX_ENEMY_DISP = 10;
constexpr size_t MAX_SHOT_DISP = 10;
class Game {
private:
	Player *player;
	Stage stage;
	Direction keyDirection;
	std::vector<Enemy *> enemyPool;
	std::vector<bool> enemyPoolFlags;
	std::vector <Shot *> shotPool;
	std::vector<bool> shotPoolFlags;
	ShotMover smover;
	size_t enemCount;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

	StagePart getNextEnemyData();
	int getNextEnemyTiming();

	void playerKeyProcessing();
	void enemyShotProcessing();
	void enemyProcessing();
	void enemyShotDrawing();
	void enemyDrawing();

public:
	Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages, const IMGDataBase &shotImages);
	~Game();

	void mainLoop();
};