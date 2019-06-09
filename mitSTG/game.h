#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "STG.h"

constexpr size_t MAX_ENEMY_DISP = 10;
class Game {
private:
	Player *player;
	Stage stage;
	Direction keyDirection;
	std::vector<const Enemy *> enemyPool;
	std::vector<bool> enemyPoolFlags;
	size_t enemCount;
	int counter;

	// store the key direction entered in keyDirection
	void checkKey();

	StagePart getNextEnemyData();
	int getNextEnemyTiming();

	void playerKeyProcessing();
	void enemyProcessing();
	void enemyDrawing();

public:
	Game(Player *player, const char *stagePath, const IMGDataBase &enemyImages);

	void mainLoop();
};