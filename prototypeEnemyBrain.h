#pragma once

#include <vector>
#include <random>
#include <chrono>
#include "map.h"
#include "main.h"

vector<mapSpaceID*> findPath(mapSpaceID* start, mapSpaceID* goal, Map map);
vector<mapSpaceID> generateSetFromStartAndGoal(mapSpaceID start, mapSpaceID goal, Map map);
vector<mapSpaceID> findSimplePathWithinSet(mapSpaceID start, mapSpaceID goal, Map map, vector<mapSpaceID> set);
vector<int> lookForCover(mapSpaceID danger, mapSpaceID currentPos, Map map);
vector<mapSpaceID> findPathF(mapSpaceID start, mapSpaceID goal, Map map);

struct mapSpaceWrapper{
public:
	mapSpaceID* mapSpace;
	int closed;
	int gValue;
	int fValue;
	mapSpaceID* child;
};


struct EnemyOfAI{
public:
	int x;
	int y;
	int sizeX;
	int sizeY;
	int dangerLevel;
	int* xPointer;
	int* yPointer;
};


#define IDLE 31
#define ATTACKING 32
#define FLEEING 33

#define STARTING 41
#define INPROGRESS 42
#define DONE 43

struct CurrentTask{
public:
	int task;
	int taskState;
	EnemyOfAI* focus;
	basicMonster* subject;
	vector<int> headingTo;


};

class ProtoEnemyBrain{
	
public:
	vector<EnemyOfAI> enemyList;
	int patienceStarting;
	int patienceCurrent;
	int skiddishStarting;
	int skiddishCurrent;
	CurrentTask currentTask;
	basicMonster* me;
	Map* currentMap;

	static ProtoEnemyBrain initBrain(basicMonster* me, Map* map);
	int brainLoop();
	int realizeNewEnemy(int* x, int* y, int dangerLevel, int sizeX, int sizeY);
	void adjustDangerLevelOnExistingEnemy(int enemyID, int newLevel);
};