#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <windows.system.h>
//#include <chrono>
#include <future>
#include <thread>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "map.h"
#include "prototypeEnemyBrain.h"
#include "main.h"
#include <algorithm>



#define CIRCLESIZE 10.f
#define CIRCLEMAXX (MAXX - (CIRCLESIZE * 2))
#define CIRCLEMAXY (MAXY - (CIRCLESIZE * 2))
#define NORTH 1
#define EAST 2
#define SOUTH 3
#define WEST 4
#define posBackX circleRect.top + CIRCLESIZE
#define posBackY circleRect.left + CIRCLESIZE
#define NEXTX move->pathList[move->nextNode].x
#define NEXTY move->pathList[move->nextNode].y
#define LASTX move->pathList[move->finalNode].x
#define LASTY move->pathList[move->finalNode].y

extern float posX = 0;
extern float posY = 0;
int keepGoing = 0;
int doOnce = 0;



struct activeMove{
public:
	vector<mapSpaceID> pathList;
//	mapSpaceID* lastNode;
//	mapSpaceID* nextNode;
//	vector<mapSpaceID>::iterator nextNode;
//	vector<mapSpaceID>::iterator lastNode;
	int nextNode;
	int lastNode;
	int finalNode;
	int subject;
};


sf::RenderWindow window(sf::VideoMode(MAXX, MAXY), "SFML works!");
sf::CircleShape shape(CIRCLESIZE);
sf::CircleShape enemyShape(CIRCLESIZE);
basicMonster enemy1;
Map map1;
vector<sf::RectangleShape> rectShapes;
sf::IntRect circleRect;
vector<sf::RectangleShape> pathShapes;
vector<activeMove> activeMovePaths;
vector<activeMove> waitingMovePaths;
vector<activeMove*> toDeleteMovePaths;
vector<ProtoEnemyBrain*> activeBrains;
vector<EnemyOfAI*> enemiesWeGaveBrain;
vector<int> givenEnemyIDs;



auto gameStart = std::chrono::high_resolution_clock::now();
auto lastDraw = gameStart;
auto nextDraw = lastDraw;
auto lastLoop = gameStart;
auto nextLoop = lastLoop;





int addMovePath(vector<mapSpaceID> pathList){

	activeMove newMove;
	newMove.pathList = pathList;
	newMove.subject = 0;
	newMove.nextNode = 1;
	newMove.lastNode = 0;
	newMove.finalNode = newMove.pathList.size() - 1;
	activeMovePaths.push_back(newMove);
	/*
	newMove.nextNode = newMove.pathList.begin();
	newMove.nextNode++;
	newMove.lastNode = newMove.pathList.begin();
	*/
	/*
	activeMovePaths[activeMovePaths.size()-1].nextNode = activeMovePaths[activeMovePaths.size()-1].pathList.begin();
	activeMovePaths[activeMovePaths.size()-1].nextNode++;
	activeMovePaths[activeMovePaths.size()-1].lastNode = activeMovePaths[activeMovePaths.size()-1].pathList.begin();
	*/

	return 1;
}


int addWaitingMovePath(vector<mapSpaceID> pathList){

	activeMove newMove;
	newMove.pathList = pathList;
	newMove.subject = 0;
	newMove.nextNode = 1;
	newMove.lastNode = 0;
	newMove.finalNode = newMove.pathList.size() - 1;
	waitingMovePaths.push_back(newMove);
	/*
	newMove.nextNode = newMove.pathList.begin();
	newMove.nextNode++;
	newMove.lastNode = newMove.pathList.begin();
	*/
	/*
	activeMovePaths[activeMovePaths.size()-1].nextNode = activeMovePaths[activeMovePaths.size()-1].pathList.begin();
	activeMovePaths[activeMovePaths.size()-1].nextNode++;
	activeMovePaths[activeMovePaths.size()-1].lastNode = activeMovePaths[activeMovePaths.size()-1].pathList.begin();
	*/

	return 1;
}


int stepActiveMove(activeMove* move){

	if ((LASTX * 20 == enemy1.x) && (LASTY * 20 == enemy1.y)){
		toDeleteMovePaths.push_back(move);
		return 2;
	}

	if ((NEXTX * 20 == enemy1.x) && (NEXTY * 20 == enemy1.y)){ move->nextNode++; }
	

	int xStep = 0;
	int yStep = 0;

	if (NEXTX * 20 - enemy1.x == 0){ xStep = 0; }
	else{ xStep = ((NEXTX * 20 - enemy1.x) / abs((NEXTX * 20 - enemy1.x))) * 2; }

	if (NEXTY * 20 - enemy1.y == 0){ yStep = 0; }
	else{ yStep = ((NEXTY * 20 - enemy1.y) / abs((NEXTY * 20 - enemy1.y))) * 2; }

	enemy1.x += xStep;
	enemy1.y += yStep;

	return 1;

}

void draw()
{
	/*
	if (GetKeyState('D') == true){ posX++; }
	if (GetKeyState('A') == true){ posX--; }
	*/
	/*
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){ posX+= 5; OutputDebugString(L"\nLefting!"); }
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){ posX-= 5; OutputDebugString(L"\nRighting!"); }
	*/
	shape.setPosition(circleRect.left, circleRect.top);
	enemyShape.setPosition(enemy1.x, enemy1.y);
	window.clear();
	window.draw(shape);
	for (sf::RectangleShape rect : rectShapes)
	{
		window.draw(rect);
	}
	for (sf::RectangleShape rect : pathShapes)
	{
		window.draw(rect);
	}

	window.draw(enemyShape);
	window.display();
	//window.setTitle("TITLE RESET");
	//OutputDebugString(L"\nDrawing!");
}

int checkOutsideBounds(int direction)
{
	switch (direction)
	{
	case WEST:
		if (circleRect.left <= 0){ return WEST; }
		break;
	case NORTH:
		if (circleRect.top <= 0){ return NORTH; }
		break;
	case EAST:
		if (circleRect.left >= CIRCLEMAXX){ return EAST; }
		break;
	case SOUTH: 
		if (circleRect.top >= CIRCLEMAXY){ return SOUTH; }
		break;
	}

	return 0;
}

bool checkObstacleHits(int direction)
{
	
	//bool hit = false;
	sf::IntRect tempRect = circleRect;

	switch (direction)
	{
	case WEST:
		tempRect.left -= 5;
		break;
	case NORTH:
		tempRect.top -= 5;
		break;
	case EAST:
		tempRect.left += 5;
		break;
	case SOUTH:
		tempRect.top += 5;
		break;
	}

	for (SquareBlock block : map1.squareBlocks)
	{
		sf::IntRect blockRect(block.frontX, block.frontY, (block.backX - block.frontX), (block.backY - block.frontY));

		if (blockRect.intersects(tempRect))
		{
		//	hit = true;
			return true;
		}


	}


		/*
		if (block.frontX - circleRect.left - CIRCLESIZE > 0 || circleRect.left - block.backX > 0){ continue; }
		if (block.frontY - circleRect.top - CIRCLESIZE > 0 || circleRect.top - block.backY > 0){ continue; }

		if (block.frontX - circleRect.left - CIRCLESIZE <= 0 || circleRect.left - block.backX > 0){ hit = true; }
		*/
	

	return false;

}

void logicLoop(void (*fp)())
{
	window.setActive(true);

	while (keepGoing == 1)
	//while (window.isOpen())
	{
		//sf::Event event;

		//std::time_t timeString = std::chrono::high_resolution_clock::to_time_t( std::chrono::high_resolution_clock::now());
		//OutputDebugString(L"\nLooping!");
		//OutputDebugString(L"FUCKING TRASH");

		/*while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}*/
		/*
		sf::Event event2;
		window.pollEvent(event2);
		if (event2.type == sf::Event::Closed)
			window.close();

		if (window.isOpen()) 
			*/
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			if (checkOutsideBounds(WEST)==WEST){ circleRect.left = 0; }
			if (checkObstacleHits(WEST) == false && checkOutsideBounds(WEST)!=WEST)
			{ circleRect.left -= 5; OutputDebugString(L"\nLefting!"); }
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){ 
			if (checkOutsideBounds(EAST) == EAST){ circleRect.left = CIRCLEMAXX; }
			if (checkObstacleHits(EAST) == false && checkOutsideBounds(EAST) != EAST)
			{ circleRect.left += 5; OutputDebugString(L"\nRighting!"); }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			if (checkOutsideBounds(SOUTH) == SOUTH){ circleRect.top = CIRCLEMAXY; }
			if (checkObstacleHits(SOUTH) == false && checkOutsideBounds(SOUTH) != SOUTH)
			{ circleRect.top += 5; OutputDebugString(L"\nDowning!"); }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			if (checkOutsideBounds(NORTH) == NORTH){ circleRect.top = 0; }
			if (checkObstacleHits(NORTH) == false && checkOutsideBounds(NORTH) != NORTH)
			{ circleRect.top -= 5; OutputDebugString(L"\nUpping!"); }
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
			activeBrains[0]->adjustDangerLevelOnExistingEnemy(givenEnemyIDs[0],100);
			 OutputDebugString(L"\nSpooking the monster.");
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)){
			activeBrains[0]->adjustDangerLevelOnExistingEnemy(givenEnemyIDs[0], 0);
			OutputDebugString(L"\nCalming the monster.");
		}
		/*
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
			if (doOnce == 0){
				activeMove newMove = waitingMovePaths[0];
				activeMovePaths.push_back(newMove);
				//doOnce = 1;
			}
		}
		*/
		/*
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			if (posX <= 0){ posX = 0; }
			else{posX -= 5; OutputDebugString(L"\nLefting!");}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){ 
			if (posX >= CIRCLEMAXX){ posX = CIRCLEMAXX; }
			else{posX += 5; OutputDebugString(L"\nRighting!");}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			if (posY >= CIRCLEMAXY){ posY = CIRCLEMAXY;  }
			else{posY += 5; OutputDebugString(L"\nDowning!");}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			if (posY <= 0){ posY = 0; }
			else{posY -= 5; OutputDebugString(L"\nUpping!");}
		}
		*/

		//You're erasing the element being iterated upon
		for (auto&& move : activeMovePaths){
			activeMove *movePtr = &move;
			stepActiveMove(movePtr);
		}

		if (toDeleteMovePaths.size() > 0){
			vector<activeMove>::iterator pos;
			for (activeMove* path : toDeleteMovePaths){
				for (pos = activeMovePaths.begin(); pos != activeMovePaths.end(); pos++){
					if (path == &(*pos)){
						break;
					}
					break;
				}
				activeMovePaths.erase(pos);
			}

			toDeleteMovePaths.clear();
		}

		

		fp();



		lastDraw = std::chrono::high_resolution_clock::now();
		nextDraw += std::chrono::milliseconds(20);
		
		std::this_thread::sleep_until(nextDraw);

	}
	window.setActive(false);
	return;
	//std::chrono::high_resolution_clock
}


void runLoop(void(*fp)(), int loopRate)
{
	lastLoop = std::chrono::high_resolution_clock::now();
	nextLoop = lastLoop;

	while (keepGoing == 1){

		fp();

		lastLoop = std::chrono::high_resolution_clock::now();
		nextLoop += std::chrono::milliseconds(loopRate);

		std::this_thread::sleep_until(nextLoop);

	}
	return;	
}


void runLoopForBrain(ProtoEnemyBrain* brain, int loopRate)
{
	auto lastLoop = std::chrono::high_resolution_clock::now();
	auto nextLoop = lastLoop;

	while (keepGoing == 1){

		
		brain->brainLoop();

		lastLoop = std::chrono::high_resolution_clock::now();
		nextLoop += std::chrono::milliseconds(loopRate);

		std::this_thread::sleep_until(nextLoop);

	}
	return;
}


int main()
{
	

	shape.setFillColor(sf::Color::Green);
	enemyShape.setFillColor(sf::Color::Red);

	enemy1.x = 25 * 20;
	enemy1.y = 20 * 20;

	circleRect = sf::IntRect(posX, posY, CIRCLESIZE * 2, CIRCLESIZE * 2);
	
	map1.generateGrid(60, 40);
	//map1.genRandomBlocks(0);

	map1.genRandomComplexBlocks(15);


	

	/*
	map1.addBlock(50, 50, 60, 60);
	map1.addBlock(70, 70, 90, 90);
	map1.addBlock(100, 100, 140, 140);
	*/

	
	for (SquareBlock block : map1.squareBlocks)
	{
		sf::RectangleShape newRect;
		newRect.setPosition(block.frontX, block.frontY);

		int sizeX = block.backX - block.frontX;
		int sizeY = block.backY - block.frontY;

		newRect.setSize(sf::Vector2f(sizeX, sizeY));

		rectShapes.push_back(newRect);
	}
	
	mapSpaceID* start = map1.mapGrid[25][0];
	mapSpaceID* goal = map1.mapGrid[40][39];

	vector<mapSpaceID> setList = generateSetFromStartAndGoal(*start, *goal, map1);
	//vector<mapSpaceID *>  pathResults = findPath(start, goal, map1);
	
	//vector<mapSpaceID> pathResults = findSimplePathWithinSet(*start, *goal, map1, setList);

	vector<mapSpaceID> pathList;
	/*
	for (mapSpaceID square : pathResults){
		sf::RectangleShape newRect;

		newRect.setPosition(square.x * 20, square.y * 20);
		newRect.setFillColor(sf::Color::Blue);
		int sizeX = 20;
		int sizeY = 20;

		newRect.setSize(sf::Vector2f(sizeX, sizeY));

		pathShapes.push_back(newRect);
		pathList.push_back(square);
	}
	
	//addMovePath(pathList);
	addWaitingMovePath(pathList);
	*/

	ProtoEnemyBrain* brain1 = &ProtoEnemyBrain::initBrain(&enemy1, &map1);
	activeBrains.push_back(brain1);

	int newGivenEnemy = activeBrains[0]->realizeNewEnemy(&circleRect.left, &circleRect.top, 0, 20, 20);
	//enemiesWeGaveBrain.push_back(newGivenEnemy);

	givenEnemyIDs.push_back(newGivenEnemy);

	keepGoing = 1;
	void (*drawp)();
	drawp = draw;

	int (ProtoEnemyBrain::*fpointer)();
	fpointer = &ProtoEnemyBrain::brainLoop;

	window.setActive(false);

	std::thread t1(logicLoop,drawp);
	
	std::thread t2(runLoopForBrain, activeBrains[0], 500);

	while (window.isOpen())
	{
		sf::Event event;
		//OutputDebugString(L"\nGotHere!");

		
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed){

				keepGoing = 0;
				t1.join();
				window.close();
			}
		}
		
		
		

		//OutputDebugString(L"\GotHere!");
		//draw(shape, window, posX, posY);
		/*
		shape.setPosition(posX, posY);
		window.clear();
		window.draw(shape);
		window.display();
		*/

		//draw();
		//window.draw(shape);
		//window.display();

	}


	return 0;
}


//void draw(sf::CircleShape shape1, sf::RenderWindow window1, float posX, float posY)
