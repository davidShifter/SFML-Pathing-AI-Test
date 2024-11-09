//#include <vector>
//#include <random>
//#include <chrono>
#include "map.h"
#include <windows.system.h>

#define BRANCHWIDTH 30

using std::vector;


void Map::generateGrid(int xSpaces, int ySpaces){
	int count = 0;
	height = ySpaces;
	width = xSpaces;


	for (int i = 0; i < (xSpaces * ySpaces); i++){
		mapSpaceID newSpaceID;
		newSpaceID.gridBlock = 0;
		newSpaceID.index = i;
		newSpaceID.filled = 0;
		mapSpaceIDList.push_back(newSpaceID);
		newSpaceID.gValue = 0;
		newSpaceID.fValue = 0;
		newSpaceID.inList = false;
		newSpaceID.closed = false;

	}

	for (int iX = 1; iX <= xSpaces; iX++){
		std::vector<mapSpaceID *> newColumn;

		for (int iY = 1; iY <= ySpaces; iY++){
			
			newColumn.push_back(&(mapSpaceIDList[count]));
			mapSpaceIDList[count].x = (iX - 1);
			mapSpaceIDList[count].y = (iY - 1);
		//	mapSpaceIDList[count].gridBlock = &(newColumn[iY - 1]);
			count++;
		}

		mapGrid.push_back(newColumn);
	}


}





int Map::addBlock(int fX, int fY, int bX, int bY){

	SquareBlock newBlock;
	newBlock.frontX = fX;
	newBlock.frontY = fY;
	newBlock.backX = bX;
	newBlock.backY = bY;


	squareBlocks.push_back(newBlock);

	return 1;
}



void Map::genRandomBlocks(int number){

	std::chrono::high_resolution_clock::duration dur = std::chrono::high_resolution_clock::now().time_since_epoch();
	unsigned dur2 = dur.count();
	dre.seed(dur2);

	for (int i = 0; i < number; i++)
	{
		std::uniform_int_distribution<int> randomSizeX(1, 8);
		std::uniform_int_distribution<int> randomSizeY(1, 3);

		int sizeX = randomSizeX(dre);
		int sizeY = randomSizeY(dre);

		std::uniform_int_distribution<int> randomX(0, mapGrid.size() - sizeX);
		std::uniform_int_distribution<int> randomY(0, mapGrid[0].size() - sizeY);
		
		int fX = randomX(dre);
		int fY = randomY(dre);

		int bX = fX + sizeX;
		int bY = fY + sizeY;

		bool spaceIsOpen = true;

		for (int iX = fX; iX < bX; iX++){
			for (int iY = fY; iY < bY; iY++){
				if (mapGrid[iX][iY]->filled == 1){ spaceIsOpen = false; break; }
			}
			if (spaceIsOpen == false){ break; }
		}

		if (spaceIsOpen == false){
			i -= 1;
			OutputDebugString(L"\nSpace was taken up!");
			continue;
		}

		if (spaceIsOpen == true){
			for (int iX = fX; iX < bX; iX++){
				for (int iY = fY; iY < bY; iY++){
					mapGrid[iX][iY]->filled = 1;
				}
			}

			addBlock(fX *20, fY *20, bX *20, bY *20);
			OutputDebugString(L"\nAdded a block!");
		}

		

		
	}

}

void Map::genRandomComplexBlocks(int number){

	std::chrono::high_resolution_clock::duration dur = std::chrono::high_resolution_clock::now().time_since_epoch();
	unsigned dur2 = dur.count();
	dre.seed(dur2);

	for (int i = 0; i < number; i++)
	{

		std::uniform_int_distribution<int> randomSizeX(5, 14);
		std::uniform_int_distribution<int> randomSizeY(1, 3);
		std::normal_distribution<double> randomDepth(0, 1.0);
		std::uniform_int_distribution<int> randomDirection(0, 1);

		int sizeX = randomSizeX(dre);
		int sizeY = randomSizeY(dre);

		std::uniform_int_distribution<int> branchRandomSizeY(1, sizeY);

		int branchSizeY = branchRandomSizeY(dre);

		std::uniform_int_distribution<int> randomX(0, mapGrid.size() - sizeX);
		std::uniform_int_distribution<int> randomY(branchSizeY, mapGrid[0].size() - sizeY - branchSizeY);

		int fX = randomX(dre);
		int fY = randomY(dre);

		int bX = fX + sizeX;
		int bY = fY + sizeY;

		bool spaceIsOpen = true;

		auto depthFloat = randomDepth(dre);

		int depthInt = ceil(depthFloat);
		int depth = abs(depthInt);
		int randomDir = randomDirection(dre);

		//std::uniform_int_distribution<int> randomBranchWidth(10, (bY - fY));
		//int branchWidth = randomBranchWidth(dre);

		std::uniform_int_distribution<int> branchRandomX(fX, bX - 1);
		

		int branchfY = 0;
		int branchfX = 0;
		int branchbX = 0;
		int branchbY = 0;
		
		if (randomDir == 0)
		{
			branchfY = bY;
			branchfX = branchRandomX(dre);
			branchbX = branchfX + 1;
			branchbY = branchfY + branchSizeY;
			OutputDebugString(L"\nRandomDir is 0");

		}
		else{

			branchfY = fY - branchSizeY;
			branchfX = branchRandomX(dre);
			branchbX = branchfX + 1;
			branchbY = fY;
			OutputDebugString(L"\nRandomDir is 1");
		}

		

		for (int iX = branchfX; iX < branchbX; iX++){
			for (int iY = branchfY; iY < branchbY; iY++){
				if (mapGrid[iX][iY]->filled == 1){ spaceIsOpen = false; break; }
			}
			if (spaceIsOpen == false){ break; }
		}
		
		if (spaceIsOpen == false){
			i -= 1;
			OutputDebugString(L"\nSpace was taken up!");
			continue;
		}

		for (int iX = fX; iX < bX; iX++){
			for (int iY = fY; iY < bY; iY++){
				if (mapGrid[iX][iY]->filled == 1){ spaceIsOpen = false; break; }
			}
			if (spaceIsOpen == false){ break; }
		}

		if (spaceIsOpen == false){
			i -= 1;
			OutputDebugString(L"\nSpace was taken up!");
			continue;
		}

		if (spaceIsOpen == true){
			for (int iX = fX; iX < bX; iX++){
				for (int iY = fY; iY < bY; iY++){
					mapGrid[iX][iY]->filled = 1;
				}
			}
			for (int iX = branchfX; iX < branchbX; iX++){
				for (int iY = branchfY; iY < branchbY; iY++){
					mapGrid[iX][iY]->filled = 1;
				}
			}
			addBlock(fX * 20, fY * 20, bX * 20, bY * 20);
			addBlock(branchfX * 20, branchfY * 20, branchbX * 20, branchbY * 20);
			OutputDebugString(L"\nAdded a complex block!");
		}


		//OutputDebugString(L"\nAdded a block!");
	}


}









/*int Map::addBlock(int fX, int fY, int bX, int bY){

	SquareBlock newBlock;
	newBlock.frontX = fX;
	newBlock.frontY = fY;
	newBlock.backX = bX;
	newBlock.backY = bY;

	squareBlocks.push_back(newBlock);

	return 1;
}



void Map::genRandomBlocks(int number){

	std::chrono::high_resolution_clock::duration dur = std::chrono::high_resolution_clock::now().time_since_epoch();
	unsigned dur2 = dur.count();
	dre.seed(dur2);

	for (int i = 0; i < number; i++)
	{

		std::uniform_int_distribution<int> randomX(0, MAXX);
		std::uniform_int_distribution<int> randomY(0, MAXY);
		std::uniform_int_distribution<int> randomSizeX(5, 80);
		std::uniform_int_distribution<int> randomSizeY(10, 30);

		int fX = randomX(dre);
		int fY = randomY(dre);
		int bX = fX + randomSizeX(dre);
		int bY = fY + randomSizeY(dre);

		addBlock(fX, fY, bX, bY);

		OutputDebugString(L"\nAdded a block!");
	}

}

void Map::genRandomComplexBlocks(int number){

	std::chrono::high_resolution_clock::duration dur = std::chrono::high_resolution_clock::now().time_since_epoch();
	unsigned dur2 = dur.count();
	dre.seed(dur2);

	for (int i = 0; i < number; i++)
	{


		std::uniform_int_distribution<int> randomSizeX(90, 160);
		std::uniform_int_distribution<int> randomSizeY(15, 30);
		std::normal_distribution<double> randomDepth(0, 1.0);
		std::uniform_int_distribution<int> randomDirection(0, 1);

		int sizeX = randomSizeX(dre);
		int sizeY = randomSizeY(dre);

		std::uniform_int_distribution<int> randomX(0, MAXX - sizeX);
		std::uniform_int_distribution<int> randomY(0, MAXY - sizeY);

		int fX = randomX(dre);
		int fY = randomY(dre);

		int bX = fX + sizeX;
		int bY = fY + sizeY;

		auto depthFloat = randomDepth(dre);

		int depthInt = ceil(depthFloat);
		int depth = abs(depthInt);
		int randomDir = randomDirection(dre);

		std::uniform_int_distribution<int> randomBranchWidth(10, (bY - fY));

		int branchWidth = randomBranchWidth(dre);

		std::uniform_int_distribution<int> branchRandomX(fX + branchWidth, bX);
		std::uniform_int_distribution<int> branchRandomSizeY(20, ((bX-fX)/3 - 10));

		
		
		
		

		if (randomDir == 0)
		{
			int branchfY = bY;
			int branchbX = branchRandomX(dre);
			int branchbY = fY + branchRandomSizeY(dre);
			int branchfX = branchbX - branchWidth;
			OutputDebugString(L"\nRandomDir is 0");
			addBlock(branchfX, branchfY, branchbX, branchbY);
		}
		else{

			int branchfY = fY - branchRandomSizeY(dre);
			int branchbX = branchRandomX(dre);
			int branchbY = fY;
			int branchfX = branchbX - branchWidth;
			addBlock(branchfX, branchfY, branchbX, branchbY);
		}

		addBlock(fX, fY, bX, bY);
		

		OutputDebugString(L"\nAdded a block!");
	}


}*/

/*
class Map {
public:
	std::default_random_engine dre;
	vector<SquareBlock> squareBlocks;


	int addBlock(int fX, int fY, int bX, int bY)
	{

		SquareBlock newBlock;
		newBlock.frontX = fX;
		newBlock.frontY = fY;
		newBlock.backX = bX;
		newBlock.backY = bY;

		squareBlocks.push_back(newBlock);
	}

	void genRandomBlocks(int number){

		dre.seed(std::chrono::high_resolution_clock::now());
		std::uniform_int_distribution<int> randomX(0, MAXX);
		std::uniform_int_distribution<int> randomY(0, MAXY);
		std::uniform_int_distribution<int> randomSizeX(0, 10);
		std::uniform_int_distribution<int> randomSizeY(0, 10);

		int fX = randomX(dre);
		int fY = randomY(dre);
		int bX = fX + randomSizeX(dre);
		int bY = fY + randomSizeY(dre);

		addBlock(fX, fY, bX, bY);


	}
	


};
*/
