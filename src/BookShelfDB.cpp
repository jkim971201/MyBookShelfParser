#include <iostream>
#include <vector>

#include "BookShelfDB.h"

namespace BookShelf
{

// Cell //
Cell::Cell()
{
  lx_ = ly_ = ux_ = uy_ = 0;
  orient_ = 'N';
  isFixed_ = isFixedNI_ = false;
}

Cell::Cell(char* name, int  width, int height, 
		   bool isTerminal, bool isTerminalNI) 
    : Cell()
{
  name_ = name;

  dx_ = width;
  dy_ = height;

  isTerminal_   = isTerminal;
  isTerminalNI_ = isTerminalNI;
}

void 
Cell::setFixed()   
{ 
  isFixed_   = true; 
}

void 
Cell::setFixedNI() 
{ 
  isFixedNI_ = true; 
}

void 
Cell::setOrient(char orient) 
{ 
  orient_ = orient;
}

void 
Cell::setXY(int x, int y)
{
  lx_ = x;
  ly_ = y;
}

// BookShelfDB //
BookShelfDB::BookShelfDB(int numNodes, int numTerminals)
{
  numCells_ = numNodes;
  numStdCells_ = numNodes - numTerminals;
  numMacros_ = numTerminals;
}

void
BookShelfDB::makeCell(char* name, int  width, int height, 
				      bool isTerminal, bool isTerminalNI)
{
  Cell oneCell(name, width, height, isTerminal, isTerminalNI);

  cellInsts_.push_back(oneCell);
  cellPtrs_.push_back(&oneCell);

  cellMap_[name] = &oneCell;
}

} // namespace BookShelf
