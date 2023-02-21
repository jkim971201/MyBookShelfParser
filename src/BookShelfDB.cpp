#include <iostream>
#include <vector>
#include <cassert>
#include <stdio.h>

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

Cell::Cell(std::string name, int  width, int height, 
		   bool isTerminal, bool isTerminalNI) 
    : Cell()
{
  name_ = std::string(name);

  dx_ = width;
  dy_ = height;

  isTerminal_   = isTerminal;
  isTerminalNI_ = isTerminalNI;
}

void 
Cell::setFixed()   
{ 
  isFixed_   = true; 
  isFixedNI_ = false; 
}

void 
Cell::setFixedNI() 
{ 
  // isFixed is also true for FixedNI
  isFixed_   = true; 
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

  cellPtrs_.reserve(numNodes);
  cellInsts_.reserve(numNodes);
}

void
BookShelfDB::makeCell(std::string name, int  width, int height, 
				      bool isTerminal, bool isTerminalNI)
{
  Cell oneCell(name, width, height, isTerminal, isTerminalNI);
  cellInsts_.push_back(oneCell);
}

void
BookShelfDB::buildMap()
{
  printf("[BookShelfDB] Building Node Map\n");
  for(Cell& c : cellInsts_)
  {
    cellPtrs_.push_back(&c);
    cellMap_.emplace(c.name(), &c);
  }
}

void
BookShelfDB::verifyMap()
{
  std::cout << "Start Verifying Map Vector" << std::endl;
  for(auto kv : cellMap_)
  {
    std::cout << "key name: " << kv.first << std::endl;
    std::cout << "ptr width: " << kv.second->dx() << std::endl;
  }
}

void
BookShelfDB::verifyVec()
{
  std::cout << "Start Verifying Instance Vector" << std::endl;
  for(auto c : cellInsts_)
  {
    std::cout << "cell name: " << c.name() << std::endl;
    std::cout << "cell width: " << c.dx() << std::endl;
  }
}

void
BookShelfDB::verifyPtrVec()
{
  std::cout << "Start Verifying Pointer Vector" << std::endl;
//  for(auto c : cellPtrs_)
//  {
//    std::cout << "cell name: " << c->name() << std::endl;
//    std::cout << "cell width: " << c->dx() << std::endl;
//  }
  for(int i = 0; i < cellPtrs_.size(); i++)
  {
    std::cout << "cell name: " << cellPtrs_[i]->name() << std::endl;
    std::cout << "cell width: " << cellPtrs_[i]->dx() << std::endl;
  }
}

} // namespace BookShelf
