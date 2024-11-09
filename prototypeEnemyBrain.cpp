#include "prototypeEnemyBrain.h"
#include <map>
#include <windows.system.h>
#include "main.h"

#define NORTH 1
#define NORTHEAST 2
#define EAST 3
#define SOUTHEAST 4
#define SOUTH 5
#define SOUTHWEST 6
#define WEST 7
#define NORTHWEST 8

ProtoEnemyBrain ProtoEnemyBrain::initBrain(basicMonster* me, Map* map){
	ProtoEnemyBrain newBrain;

	newBrain.me = me;

	CurrentTask newTask;
	newTask.subject = me;
	newTask.task = IDLE;
	newTask.taskState = INPROGRESS;
	newBrain.currentTask = newTask;

	newBrain.patienceStarting = newBrain.patienceCurrent = 100;
	newBrain.skiddishStarting = newBrain.skiddishCurrent = 15;
	newBrain.currentMap = map;

	return newBrain;

}

int ProtoEnemyBrain::brainLoop(){

	if (currentTask.task == IDLE)
	{

		if (enemyList.size() > 0){
			if (enemyList[0].dangerLevel > 10){
				CurrentTask newTask;
				newTask.task = FLEEING;
				newTask.taskState = STARTING;
				currentTask = newTask;
				currentTask.focus = &enemyList[0];
				currentTask.subject = me;
			}
		}

	}

	if (currentTask.task == FLEEING){
		if (currentTask.taskState == STARTING){
			mapSpaceID danger = *(currentMap->mapGrid[*currentTask.focus->xPointer / 20][*currentTask.focus->yPointer / 20]);
			mapSpaceID currentPos = *(currentMap->mapGrid[me->x / 20][me->y / 20]);
			vector<int> fleeingTo = lookForCover(danger, currentPos, *currentMap);
			if (currentPos.x == fleeingTo[0] && currentPos.y == fleeingTo[1]){ return 2; }
			currentTask.headingTo = fleeingTo;
			mapSpaceID goal = *currentMap->mapGrid[fleeingTo[0]][fleeingTo[1]];

			vector<mapSpaceID> pathResult = findPathF(currentPos, goal, *currentMap);
			vector<mapSpaceID> path;

			for (mapSpaceID pathPoint : pathResult){
				path.push_back(pathPoint);
			}

			addMovePath(path);
			currentTask.taskState = INPROGRESS;

		}

		if (currentTask.headingTo[0] * 20 == me->x && currentTask.headingTo[1] * 20 == me->y){
			currentTask.taskState = DONE;
			currentTask.task = IDLE;
		}

	}

	

	OutputDebugString(L"\nbrainLoop running");
	return 1;
}

void ProtoEnemyBrain::adjustDangerLevelOnExistingEnemy(int enemyID, int newLevel){

	enemyList[enemyID].dangerLevel = newLevel;

}

int ProtoEnemyBrain::realizeNewEnemy(int* x, int* y, int dangerLevel, int sizeX, int sizeY){

	EnemyOfAI newEnemy;
	newEnemy.xPointer = x;
	newEnemy.yPointer = y;
	newEnemy.sizeX = sizeX;
	newEnemy.sizeY = sizeY;
	newEnemy.dangerLevel = dangerLevel;

	

	enemyList.push_back(newEnemy);

	
	return enemyList.size() - 1;
}

mapSpaceID getSquareAtDistanceInDirection(float slope, mapSpaceID origin, int distance, int orientation, Map map){
	
	int x = std::sqrt(pow(distance, 2) / (1 + pow(slope, 2)));
	int y = slope * std::sqrt(pow(distance, 2) / (1 + pow(slope, 2)));

	//orientation should be 1 or negative 1. This will flip the X axis to be left or right of the origin
	//maybe handle the case where it is neither, here? throw an error
	x *= orientation;

	mapSpaceID returnSpace = *map.mapGrid[origin.x + x][origin.y + y];

	return returnSpace;
}

//this function just gets some particular squares to be considered based on the direction the AI is looking
vector<mapSpaceID> lookInDirection(int direction, int x, int y, Map map){

	vector<mapSpaceID> returnList;

	int startX;
	int startY;
	int endX;
	int endY;

	//giving these arbitrary values that wouldn't result from the below math
	int startX2 = -100;
	int startY2 = -100;
	int endX2 = -100;
	int endY2 = -100;

	switch (direction){
	case NORTH:
		startX = x - 10;
		startY = y - 10;
		endX = x + 10;
		endY = y;
		break;
	case NORTHEAST: 
		startX = x - 5;
		startY = y - 7;
		endX = x + 7;
		endY = y;
		startX2 = x;
		startY2 = y + 1;
		endX2 = x + 7;
		endY2 = y + 5;
		break;
	case EAST:
		startX = x;
		startY = y - 10;
		endX = x + 10;
		endY = y + 10;
		break;
	case SOUTHEAST:
		startX = x - 5;
		startY = y;
		endX = x + 7;
		endY = y + 7;
		startX2 = x;
		startY2 = y - 5;
		endX2 = x + 7;
		endY2 = y - 1;
		break;
	case SOUTH:
		startX = x - 10;
		startY = y;
		endX = x + 10;
		endY = y + 10;
		break;
	case SOUTHWEST: 
		startX = x - 7;
		startY = y;
		endX = x + 5;
		endY = y + 7;
		startX2 = x - 7;
		startY2 = y - 5;
		endX2 = x;
		endY2 = y -1;
		break;
	case WEST:
		startX = x - 10;
		startY = y - 10;
		endX = x;
		endY = y + 10;
		break;
	case NORTHWEST:
		startX = x - 7;
		startY = y - 7;
		endX = x + 5;
		endY = y;
		startX2 = x - 7;
		startY2 = y + 1;
		endX2 = x;
		endY2 = y + 5;
		break;
	}

	//add the grid squares
	for (int iX = startX; iX >= endX; iX++){
		for (int iY = startY; iY >= endY; iY++){
			if (iY < map.mapGrid[0].size() && iY >= 0 && iX < map.mapGrid.size() && iX >= 0){
				returnList.push_back(*map.mapGrid[iX][iY]);
			}
		}

	}

	//check if they're still the arbitrary values. If they are, we don't need to add those squares
	if (startX2 != -100){
		for (int iX = startX2; iX >= endX2; iX++){
			for (int iY = startY2; iY >= endY2; iY++){
				if (iY < map.mapGrid[0].size() && iY >= 0 && iX < map.mapGrid.size() && iX >= 0){
					returnList.push_back(*map.mapGrid[iX][iY]);
				}
			}

		}
	}

	return returnList;

}

//this will attempt to find a square on the other side of cover from a danger point
vector<int> lookForCover(mapSpaceID danger, mapSpaceID currentPos, Map map){
	float xDif = danger.x - currentPos.x;
	float yDif = danger.y - currentPos.y;

	float slopeForCover = 0;

	if (xDif == 0){ slopeForCover = 100; }
	else{ slopeForCover = -1 * (yDif / abs(xDif)); } //this is the slope we travel along to find our coverGuess


	int orientationToRun = -1 * ((danger.x - currentPos.x) / abs(danger.x - currentPos.x)); //this gives us 1 or negative 1

	

	
	mapSpaceID coverGuess = getSquareAtDistanceInDirection(slopeForCover, currentPos, 10, orientationToRun, map);

	vector<mapSpaceID> coverSet = generateSetFromStartAndGoal(currentPos, coverGuess, map);
	std::multimap<int, mapSpaceID> coverMap;

	for (mapSpaceID space : coverSet){
		int deltaX = space.x - currentPos.x;
		int deltaY = space.y - currentPos.y;

		int distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2));

		coverMap.insert({ distance, space });

	}
	
	bool foundWall = false;
	mapSpaceID theWall;

	while (foundWall == false){
		//bug here somewhere with coverMap running out of spaces, I think
		if ((*coverMap.begin()).second.filled == 1){
			theWall = (*coverMap.begin()).second;
			foundWall = true;
			break;
		}
		else{
			coverMap.erase(coverMap.begin());
		}
	}


	float xDifCover = danger.x - theWall.x;
	float yDifCover = danger.y - theWall.y;

	float slopeToFindCover = 0;

	int orientationCover = 1;

	if (xDifCover == 0 && yDifCover > 0){ slopeToFindCover = 100; }
	else if (xDifCover == 0 && yDifCover < 0){ slopeToFindCover = -100; }
	else{ slopeToFindCover = -1 * (yDifCover / abs(xDifCover)); orientationCover = -1 * (danger.x - theWall.x) / abs(danger.x - theWall.x); } //this is the slope we travel along to find our coverMark


	bool foundMark = false;
	mapSpaceID coverMark;

	for (int i = 1; i <= 10; i++){
		mapSpaceID checkSquare = getSquareAtDistanceInDirection(slopeToFindCover, theWall, i, orientationCover, map);
		if (checkSquare.filled == 0){
			coverMark = checkSquare;
			break;
		}
	}

	return vector < int > {coverMark.x, coverMark.y};

}



int getAdjacentIndex(mapSpaceID target, Map map, int direction){

	int index = target.index;
	
	
	//int columnLength = map.mapGrid[0].size;
	// If we ever change the map size, we need to change the numbers below to use columnLength

	switch (direction){
	case NORTH: index -= 1;
		break;
	case NORTHEAST: index += 39;
		break;
	case EAST: index += 40;
		break;
	case SOUTHEAST: index += 41;
		break;
	case SOUTH: index += 1;
		break;
	case SOUTHWEST: index -= 39;
		break;
	case WEST: index -= 40;
		break;
	case NORTHWEST: index -= 41;
		break;

	}

	if (index < 0 || index > (map.mapSpaceIDList.size() - 1)) { return -1; }

		//if (index > list.size)
		return index;

}


vector<mapSpaceID*> getAllAdjacentMapIDs(mapSpaceID target, Map map){
	OutputDebugString(L"\nRunning through getAllAdjacentMapIDs");
	vector<mapSpaceID *> returnList;

	int x = target.x;
	int y = target.y;
	

	/*
	if (map.mapGrid[x][y + 1]){ returnList.push_back(map.mapGrid[x][y + 1]); }
	if (map.mapGrid[x][y-1]){ returnList.push_back(map.mapGrid[x][y - 1]); }
	if (map.mapGrid[x + 1][y - 1]){ returnList.push_back(map.mapGrid[x + 1][y - 1]); }
	if (map.mapGrid[x + 1][y]){ returnList.push_back(map.mapGrid[x + 1][y]); }
	if (map.mapGrid[x + 1][y + 1]){ returnList.push_back(map.mapGrid[x + 1][y + 1]); }
	if (map.mapGrid[x - 1][y - 1]){ returnList.push_back(map.mapGrid[x - 1][y - 1]); }
	if (map.mapGrid[x - 1][y]){ returnList.push_back(map.mapGrid[x - 1][y]); }
	if (map.mapGrid[x - 1][y + 1]){ returnList.push_back(map.mapGrid[x - 1][y + 1]); }
	*/
	
	if (0 <= x && x < 60 && 0 <= (y + 1) && (y+1) < 40){ returnList.push_back(map.mapGrid[x][y+1]); }
	if (0 <= x && x < 60 && 0 <= (y - 1) && (y-1) < 40){ returnList.push_back(map.mapGrid[x][y - 1]); }
	if (0 <= (x+1) && (x + 1) < 60 && 0 <= (y - 1) && (y-1) < 40){ returnList.push_back(map.mapGrid[x + 1][y - 1]); }
	if (0 <= (x+1) && (x + 1) < 60 && 0 <= (y) && y < 40){ returnList.push_back(map.mapGrid[x + 1][y]); }
	if (0 <= (x+1) && (x + 1) < 60 && 0 <= (y + 1) && (y+1) < 40){ returnList.push_back(map.mapGrid[x + 1][y + 1]); }
	if (0 <= (x-1) && (x - 1) < 60 && 0 <= (y - 1) && (y-1) < 40){ returnList.push_back(map.mapGrid[x - 1][y - 1]); }
	if (0 <= (x-1) && (x - 1) < 60 && 0 <= (y) && y < 40){ returnList.push_back(map.mapGrid[x - 1][y]); }
	if (0 <= (x-1) && (x - 1) < 60 && 0 <= (y + 1) && (y+1) < 40){ returnList.push_back(map.mapGrid[x - 1][y + 1]); }
	
	
	return returnList;
}


vector<mapSpaceID*> getAllAdjacentMapIDsWithinSet(mapSpaceID target, Map map, vector<mapSpaceID> *set){
	OutputDebugString(L"\nRunning through getAllAdjacentMapIDsWithinSet");
	vector<mapSpaceID *> possibleReturns;
	vector<mapSpaceID*> returnList;
	int x = target.x;
	int y = target.y;


	/*
	if (map.mapGrid[x][y + 1]){ returnList.push_back(map.mapGrid[x][y + 1]); }
	if (map.mapGrid[x][y-1]){ returnList.push_back(map.mapGrid[x][y - 1]); }
	if (map.mapGrid[x + 1][y - 1]){ returnList.push_back(map.mapGrid[x + 1][y - 1]); }
	if (map.mapGrid[x + 1][y]){ returnList.push_back(map.mapGrid[x + 1][y]); }
	if (map.mapGrid[x + 1][y + 1]){ returnList.push_back(map.mapGrid[x + 1][y + 1]); }
	if (map.mapGrid[x - 1][y - 1]){ returnList.push_back(map.mapGrid[x - 1][y - 1]); }
	if (map.mapGrid[x - 1][y]){ returnList.push_back(map.mapGrid[x - 1][y]); }
	if (map.mapGrid[x - 1][y + 1]){ returnList.push_back(map.mapGrid[x - 1][y + 1]); }
	*/

	if (0 <= x && x < 60 && 0 <= (y + 1) && (y + 1) < 40){ possibleReturns.push_back(map.mapGrid[x][y + 1]); }
	if (0 <= x && x < 60 && 0 <= (y - 1) && (y - 1) < 40){ possibleReturns.push_back(map.mapGrid[x][y - 1]); }
	if (0 <= (x + 1) && (x + 1) < 60 && 0 <= (y - 1) && (y - 1) < 40){ possibleReturns.push_back(map.mapGrid[x + 1][y - 1]); }
	if (0 <= (x + 1) && (x + 1) < 60 && 0 <= (y) && y < 40){ possibleReturns.push_back(map.mapGrid[x + 1][y]); }
	if (0 <= (x + 1) && (x + 1) < 60 && 0 <= (y + 1) && (y + 1) < 40){ possibleReturns.push_back(map.mapGrid[x + 1][y + 1]); }
	if (0 <= (x - 1) && (x - 1) < 60 && 0 <= (y - 1) && (y - 1) < 40){ possibleReturns.push_back(map.mapGrid[x - 1][y - 1]); }
	if (0 <= (x - 1) && (x - 1) < 60 && 0 <= (y) && y < 40){ possibleReturns.push_back(map.mapGrid[x - 1][y]); }
	if (0 <= (x - 1) && (x - 1) < 60 && 0 <= (y + 1) && (y + 1) < 40){ possibleReturns.push_back(map.mapGrid[x - 1][y + 1]); }

	
	for (mapSpaceID* returnSquare : possibleReturns){
		for (mapSpaceID& setSquare : *set){
			if (returnSquare->x == setSquare.x && returnSquare->y == setSquare.y){
				returnList.push_back(&setSquare);
				break;
			}
		}
	}

	return returnList;
}

vector<mapSpaceID> generateSetFromStartAndGoal(mapSpaceID start, mapSpaceID goal, Map map){

	vector<mapSpaceID> returnList;
	
	int xBegin = fmin(start.x, goal.x) - 2;
	int xEnd = fmax(start.x, goal.x) + 2;
	int yBegin = fmin(start.y, goal.y) - 2;
	int yEnd = fmax(start.y, goal.y) + 2;

	for (int i = xBegin; i <= xEnd; i++){
		for (int i2 = yBegin; i2 <= yEnd; i2++){
			if (i < 0 || i >= map.mapGrid.size() || i2 < 0 || i2 >= map.mapGrid[0].size()){
				continue;
			}
			else if (start.x == i && start.y == i2){
				continue;
			}
			else 
			{
				returnList.push_back(*map.mapGrid[i][i2]);
			}
		}

	}

	return returnList;
}

//we'll hand some squares to this and see if we can find a simple move
//Couple things TODO:
//Logic for returning the case where a simple path can't be found
//Change the getSet function to just use a simple X and Y instead of handing off the whole damn set
// ^ no longer sure if this code really has perf issues that warrant this
vector<mapSpaceID> findSimplePathWithinSet(mapSpaceID start, mapSpaceID goal, Map map, vector<mapSpaceID> set){

	//std::multimap<int, mapSpaceID*> gOpenList;
	std::multimap<int, mapSpaceID*> fOpenList;
	std::multimap<int, mapSpaceID*> closedList; 

	vector<mapSpaceID> spaceList = set;


	//we got all the squares around the start, so we close the start
	start.closed = true;
	start.child = &start;
	closedList.insert({ 0, &start });

	//put the first squares into the f-sorted open list, fOpenList
	mapSpaceID* focus = &start;

	
	bool goalFound = false;
	while (goalFound == false){

		vector<mapSpaceID*> nextList = getAllAdjacentMapIDsWithinSet(*focus, map, &set);

		

		for (mapSpaceID* spacePointer : nextList)
		{
			if (spacePointer->x == goal.x && spacePointer->y == goal.y)
			{
				goal.child = focus; goalFound = true; OutputDebugString(L"\nBreak command activated in nextList iteration"); break;
			}

			if (spacePointer->closed == true || spacePointer->filled == 1){ continue; }

			if (spacePointer->inList == true)
			{
				int newG;
				if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
				else{ newG = 14; }
				if (focus->gValue + newG < spacePointer->gValue){
					spacePointer->child = focus;
					spacePointer->fValue -= spacePointer->gValue;
					spacePointer->gValue = focus->gValue + newG;
					spacePointer->fValue += spacePointer->gValue;
				}
				continue;
			}


			int newG;
			if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
			else{ newG = 14; }

			if (newG == 14){

				if (map.mapGrid[spacePointer->x][focus->y]->filled == 1 || map.mapGrid[focus->x][spacePointer->y]->filled == 1){
					continue;
				}

			}

			spacePointer->child = focus;
			spacePointer->closed = false;


			int gValue = newG + focus->gValue;

			int hValue = (abs(spacePointer->x - goal.x) * 10 + abs(spacePointer->y - goal.y) * 10);

			int fValue = gValue + hValue;

			spacePointer->fValue = fValue;
			spacePointer->gValue = gValue;

			fOpenList.insert({ fValue, spacePointer });
			spacePointer->inList = true;

		}

		focus = (*fOpenList.begin()).second;
		focus->closed = true;
		closedList.insert({ focus->fValue, focus });
		fOpenList.erase(fOpenList.begin());
		focus->inList = false;
	}

	//Here, if everything worked correctly, we have found our goal & path

	vector<mapSpaceID> pathList;
	mapSpaceID* nextSpace = &goal;
	bool done = false;

	while (done == false){
		pathList.insert(pathList.begin(), *nextSpace);
		if (nextSpace == &start){ done = true; break; }
		nextSpace = nextSpace->child;
	}


	return pathList;
	//YAY
}


vector<mapSpaceID> findPathF(mapSpaceID start, mapSpaceID goal, Map mapSource){
	//map is being retained. What the fucking hell?

	//std::multimap<int, mapSpaceID*> gOpenList;
	std::multimap<int, mapSpaceID*> fOpenList;
	std::multimap<int, mapSpaceID*> closedList;

	Map map(mapSource);

	start = *(map.mapGrid[start.x][start.y]);
	goal = *(map.mapGrid[goal.x][goal.y]);

	//we got all the squares around the start, so we close the start
	start.closed = true;
	start.child = &start;
	closedList.insert({ 0, &start });

	//put the first squares into the f-sorted open list, fOpenList
	mapSpaceID* focus = &start;


	bool goalFound = false;
	while (goalFound == false){

		vector<mapSpaceID*> nextList = getAllAdjacentMapIDs(*focus, map);



		for (mapSpaceID* spacePointer : nextList)
		{
			if (spacePointer->x == goal.x && spacePointer->y == goal.y)
			{
				goal.child = focus; goalFound = true; OutputDebugString(L"\nBreak command activated in nextList iteration"); break;
			}

			if (spacePointer->closed == true || spacePointer->filled == 1){ continue; }

			if (spacePointer->inList == true)
			{
				int newG;
				if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
				else{ newG = 14; }
				if (focus->gValue + newG < spacePointer->gValue){
					spacePointer->child = focus;
					spacePointer->fValue -= spacePointer->gValue;
					spacePointer->gValue = focus->gValue + newG;
					spacePointer->fValue += spacePointer->gValue;
				}
				continue;
			}


			int newG;
			if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
			else{ newG = 14; }

			if (newG == 14){

				if (map.mapGrid[spacePointer->x][focus->y]->filled == 1 || map.mapGrid[focus->x][spacePointer->y]->filled == 1){
					continue;
				}

			}

			spacePointer->child = focus;
			spacePointer->closed = false;


			int gValue = newG + focus->gValue;

			int hValue = (abs(spacePointer->x - goal.x) * 10 + abs(spacePointer->y - goal.y) * 10);

			int fValue = gValue + hValue;

			spacePointer->fValue = fValue;
			spacePointer->gValue = gValue;

			fOpenList.insert({ fValue, spacePointer });
			spacePointer->inList = true;

		}

		if (goalFound == true){ break; }
		focus = (*fOpenList.begin()).second;
		focus->closed = true;
		closedList.insert({ focus->fValue, focus });
		fOpenList.erase(fOpenList.begin());
		focus->inList = false;
	}

	//Here, if everything worked correctly, we have found our goal & path

	vector<mapSpaceID> pathList;
	mapSpaceID* nextSpace = &goal;
	bool done = false;

	while (done == false){
		pathList.insert(pathList.begin(), *nextSpace);
		if (nextSpace == &start){ done = true; break; }
		nextSpace = nextSpace->child;
	}


	return pathList;
}




vector<mapSpaceID *> findPath(mapSpaceID* start, mapSpaceID* goal, Map map){
	
	//std::multimap<int, mapSpaceID*> gOpenList;
	std::multimap<int, mapSpaceID*> fOpenList;
	std::multimap<int, mapSpaceID*> closedList; 

	vector<mapSpaceID*> startingList = getAllAdjacentMapIDs(*start, map);

	//!!! rewrite this so we're not adjusting variables of mapSpaceID's. We should have
	// a vector that we generate to trade these values. It can be correspond with trackingVector[space->index]
	// then we just deallocate the trackingVector when we have our path

	//we got all the squares around the start, so we close the start
	start->closed = true;
	start->child = start;
	closedList.insert({ 0, start });

	//put the first squares into the f-sorted open list, fOpenList
	mapSpaceID* focus = start;

	for (mapSpaceID* spacePointer : startingList)
	{
		if (spacePointer == goal){ break; }

		if (spacePointer->closed == true || spacePointer->filled == 1){ continue; }

		if (spacePointer->inList == true)
		{
			int newG;
			if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
			else{ newG = 14; }
			if (focus->gValue + newG < spacePointer->gValue){
				spacePointer->child = focus;
				spacePointer->fValue -= spacePointer->gValue;
				spacePointer->gValue = focus->gValue + newG;
				spacePointer->fValue += spacePointer->gValue;
			}
			continue;
		}

		int newG;
		if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
		else{ newG = 14; }

		if (newG == 14){

			if (map.mapGrid[spacePointer->x][focus->y]->filled == 1 || map.mapGrid[focus->x][spacePointer->y]->filled == 1){
				continue;
			}

		}

		spacePointer->child = focus;
		spacePointer->closed = false;

		

		int gValue = newG;

		int hValue = (abs(spacePointer->x - goal->x) + abs(spacePointer->y - goal->y));

		int fValue = gValue + hValue;

		spacePointer->fValue = fValue;
		spacePointer->gValue = gValue;
		spacePointer->inList = true;
		fOpenList.insert({ fValue, spacePointer });
		

	}

	//keep going until we find the goal, in a nearly-identical loop. Why? I don't know why.
	bool goalFound = false;
	while (goalFound == false){

		mapSpaceID* focus = (*(fOpenList.begin())).second;
		vector<mapSpaceID*> nextList = getAllAdjacentMapIDs(*focus, map);

		focus->closed = true;
		closedList.insert({ focus->fValue, focus });
		fOpenList.erase(fOpenList.begin());
		focus->inList = false;

		for (mapSpaceID* spacePointer : nextList)
		{
			if (spacePointer->x == goal->x && spacePointer->y == goal->y){ goal->child = focus; goalFound = true; break; }

			if (spacePointer->closed == true || spacePointer->filled == 1){ continue; }

			if (spacePointer->inList == true)
			{
				int newG;
				if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
				else{ newG = 14; }
				if (focus->gValue + newG < spacePointer->gValue){
					spacePointer->child = focus;
					spacePointer->fValue -= spacePointer->gValue;
					spacePointer->gValue = focus->gValue + newG;
					spacePointer->fValue += spacePointer->gValue;
				}
				continue;
			}


			int newG;
			if ((spacePointer->x - focus->x) == 0 || (spacePointer->y - focus->y) == 0){ newG = 10; }
			else{ newG = 14; }

			if (newG == 14){

				if (map.mapGrid[spacePointer->x][focus->y]->filled == 1 || map.mapGrid[focus->x][spacePointer->y]->filled == 1){
					continue;
				}

			}

			spacePointer->child = focus;
			spacePointer->closed = false;


			int gValue = newG + focus->gValue;

			int hValue = (abs(spacePointer->x - goal->x) * 10 + abs(spacePointer->y - goal->y) * 10);

			int fValue = gValue + hValue;

			spacePointer->fValue = fValue;
			spacePointer->gValue = gValue;

			fOpenList.insert({ fValue, spacePointer });
			spacePointer->inList = true;

		}

	}

	//Here, if everything worked correctly, we have found our goal & path

	vector<mapSpaceID *> pathList;
	mapSpaceID* nextSpace = goal;
	bool done = false;

	while (done == false){
		pathList.insert(pathList.begin(), nextSpace);
		if (nextSpace == start){ done = true; break; }
		nextSpace = nextSpace->child;
	}
	

	return pathList;
	//YAY
}



/*
mapSpaceID getAdjacentMapID(vector<mapSpaceID> list, mapSpaceID start, int direction){

	int index = start.index;
	int columnLength = 40;

	switch (direction){
	case NORTH: index -= 1;
		break;
	case NORTHEAST: index += 39;
		break;
	case EAST: index += 40;
		break;
	case SOUTHEAST: index += 41;
		break;
	case SOUTH: index += 1;
		break;
	case SOUTHWEST: index -= 39;
		break;
	case WEST: index -= 40;
		break;
	case NORTHWEST: index -= 41;
		break;

	}

	//if (index > list.size)
	return list[index];

}
*/

int firstSquareToCheck(mapSpaceID start, mapSpaceID goal){

	if (goal.x == start.x){
		if (goal.y > start.y){ return SOUTH; }
		if (goal.y < start.y){ return NORTH; }
	}
	else{ return 0; }

	int direction = 0;
	if (goal.x > start.x){ direction = 1; }
	if (goal.x < start.x){ direction = -1; }

	


	if (direction == 1){
		float slope = (start.y - goal.y) / (goal.x - start.x);

		if (slope > 2){
			return NORTH;

		}
		else if (slope > 0.5){
			return NORTHEAST;
		}
		else if (slope > -0.5){
			return EAST;
		}
		else if (slope > -2){
			return SOUTHEAST;
		}
		else if (slope <= -2){
			return SOUTH;
		}
		else{ return 0; }
	}


	if (direction == -1){
		float slope = (start.y - goal.y) / (start.x - goal.x);

		if (slope > 2){
			return NORTH;

		}
		else if (slope > 0.5){
			return NORTHWEST;
		}
		else if (slope > -0.5){
			return WEST;
		}
		else if (slope > -2){
			return SOUTHWEST;
		}
		else if (slope <= -2){
			return SOUTH;
		}
		else{ return 0; }
	}

	return 0;
}

//This was half-written code for a method of grabbing a set of squares to pathfind within. It tries to pick X squares and gets their adjacents, attempting to grab a swath of squares from Start to Goal no matter the direction. My math is broken, somehow.
/*while (done == false){

		if (currentSquare->y - goal.y < 2)

		vector<mapSpaceID*> newGroup = getAllAdjacentMapIDs(*currentSquare, map);
		for (mapSpaceID* space : newGroup){
			returnList.push_back(*space);
		}
		

		if (&*currentSquare == &goal){ returnList.push_back(goal); done = true; break; }
		

		#define DISTANCE 3
		float slope = (goal.y - start.y) / (goal.x - start.x);
		int x = std::sqrt(pow(DISTANCE, 2) / (1 + slope));
		int y = slope * x;

		currentSquare = map.mapGrid[x][y];
		}*/