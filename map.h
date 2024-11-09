#pragma once


#include <vector>
#include <random>
#include <chrono>


#define MAXX 1200
#define MAXY 800


using std::vector;

struct SquareBlock {
public:
	int frontX;
	int frontY;
	int backX;
	int backY;
	
};

struct mapSpaceID {
public:
	int filled;
	mapSpaceID **gridBlock;
	int x;
	int y;
	int index;
	int gValue;
	int fValue;
	bool inList;
	bool closed;
	mapSpaceID *child;


};

class Map {

public:
	vector<SquareBlock> squareBlocks;
	vector<vector<mapSpaceID *>> mapGrid;
	vector<mapSpaceID> mapSpaceIDList;
	std::default_random_engine dre;
	int addBlock(int fX, int fY, int bX, int bY);
	void genRandomBlocks(int number);
	void genRandomComplexBlocks(int number);
	void generateGrid(int xSpaces, int ySpaces);
	int width;
	int height;
};



