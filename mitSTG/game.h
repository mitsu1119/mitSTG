#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <cmath>
#include "collision.h"
#include "STG.h"
#include "util.h"

#define WHITE GetColor(255, 255, 255)
#define RED GetColor(255, 0, 0)
#define GREEN GetColor(0, 255, 0)

class Scene {
protected:
	int counter;


public:
	Scene();
	const IMG *backGround;
	virtual int update() = 0;
	virtual void draw() = 0;
};

typedef std::unordered_map<std::string, std::tuple<std::vector<const IMG *>, unsigned long, std::string, double, double, int>> CharDataBase;
enum CharDBAccessor {
	CHDB_IMG, CHDB_ANIM_COUNT, CHDB_SHAPE, CHDB_SHAPE_DATA1, CHDB_SHAPE_DATA2, CHDB_HP_OR_POWER
};

typedef std::unordered_map<std::string, std::vector<std::tuple<std::vector<const IMG *>, unsigned long, int, int, std::string, std::string, double, int>>> OptionDataBase;
enum OptionDBAccessor {
	OPDB_IMG, OPDB_ANIM_COUNT, OPDB_X, OPDB_Y, OPDB_SHOTNAME, OPDB_SHOTTYPE, OPDB_SHOTSPEED, OPDB_SHOTINTERVAL
};

typedef std::unordered_map<std::string, std::tuple<std::vector<const IMG *>, unsigned long>> EffectDataBase;
enum EffectDBAccessor {
	EFDB_IMG, EFDB_ANIM_COUNT
};

constexpr size_t MAX_EFFECT_DISP = 50;
constexpr size_t MAX_ENEMY_DISP = 20;
constexpr size_t MAX_SHOT_DISP = 500;
class Game : public Scene {
private:
	Player *player;
	int playerInvincibleFlag;			// Invincible mode start time. If player is not invinceble, this value is -1.
	double playerOriginalSpeed;
	bool playerNonDrawFlag;

	const CharDataBase &enemDB, &shotDB;
	const EffectDataBase &effectDB;
	const OptionDataBase &optionDB;
	
	Stage stage;
	const IMG *bgImg, *lifeImg;
	int bgY, scrollSpeed;
	int leftX, rightX, topY, bottomY;		// for drawing area
	
	Direction keyDirection;
	bool checkKeyPShotBt, checkKeyLowPlayer;
	int timeOfLastPShot;
	
	std::vector<Effect *> effectPool;
	std::vector<bool> effectPoolFlags;
	std::vector<Enemy *> enemyPool;
	std::vector<bool> enemyPoolFlags;
	std::vector <Shot *> shotPool;
	std::vector<bool> shotPoolFlags;
	std::vector<Shot *> playerShotPool;
	std::vector<bool> playerShotPoolFlags;
	EnemyMover *emover;
	ShotMover *smover;
	
	size_t enemCount;
	ShapeCollider *collider;

	// store the key direction entered in keyDirection
	void checkKey();

	// For debug
	void drawShape(const Shape &shape);

	// If addable effect pool index was not found, return 0.
	size_t searchAddableEffectPool() const;

	StagePart getNextEnemyData();
	int getNextEnemyTiming();
	void destroyEffectPool(size_t index);
	void destroyEnemyPool(size_t index);
	void destroyEshotPool(size_t index);
	void destroyPshotPool(size_t index);

	void playerKeyProcessing();
	void playerProcessing();
	void enemyProcessing();

	void enemyShotFlagProcessing();
	void playerShotFlagProcessing();

	void enemyMoving();
	void enemyShotMoving();
	void playerShotMoving();

	void bgProcessing();
	void collisionProcessing();
	void animationProcessing();

	void bgDrawing();
	void effectDrawing();
	void systemDrawing();
	void playerAndEnemyShotDrawing();
	void enemyDrawing();
	void playerDrawing();
	void shapeDrawing();		// debug

public:
	Game(Player *player, const char *stagePath, const CharDataBase &enemyDB, const CharDataBase &shotDB, const EffectDataBase &effectDB, const OptionDataBase &optionDB, int leftX, int topY, int rightX, int bottomY, const IMG *lifeImg);
	~Game();

	virtual int update();
	virtual void draw();
};

class TitleScene: public Scene {
private:
	bool drawPressxkeyFlag;
	std::vector<const IMG *> pressXKey;
	bool isKeyInputableFlag;
	
public:
	TitleScene();
	~TitleScene();

	void animation();
	virtual int update();
	virtual void draw();
};

class GameOverScene: public Scene {
private:
	bool isKeyInputableFlag;

public:
	GameOverScene();
	~GameOverScene();

	virtual int update();
	virtual void draw();
};

class MitSTG  {
private:
	enum SceneType {
		SCENE_TITLE, SCENE_GAME_1, SCENE_GAMEOVER
	};

	// Meta datas.
	RECT wndArea;
	Scene *nowScene;
	SceneType nowSceneType;
	CharDataBase players;
	std::unordered_map<std::string, std::vector<const IMG *>> playerLeftImages;
	std::unordered_map<std::string, std::vector<const IMG *>> playerRightImages;
	CharDataBase enemys, shots;
	EffectDataBase effects;
	OptionDataBase options;

	// Player datas.
	Player *player;
	std::vector<Option *> playerOptions;
	Shape *pShape;
	double initPx, initPy;		// There data are the player's initial position when each stage begins.
	int initPlayerLifeNum;

	// Other datas.
	const IMG *lifeImg;

	// Loading databases.
	int loading();

	// Scene change function.
	void changeScene(SceneType scene);

public:
	MitSTG();
	~MitSTG();

	void update();
	void draw();
};
