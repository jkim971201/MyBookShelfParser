#include <iostream>
#include <vector>
#include <cassert>
#include <climits>
#include <stdio.h>

#include "BookShelfDB.h"

namespace BookShelf
{

// BsCell //
BsCell::BsCell()
{
  lx_ = ly_ = ux_ = uy_ = 0;
  orient_ = 'N';
  isFixed_ = isFixedNI_ = false;
}

BsCell::BsCell(std::string name, int  width, int height, 
	    	   bool isTerminal, bool isTerminalNI) 
    : BsCell()
{
  name_ = std::string(name);

  dx_ = width;
  dy_ = height;

  isTerminal_   = isTerminal;
  isTerminalNI_ = isTerminalNI;
}

void 
BsCell::setFixed()   
{ 
  isFixed_   = true; 
  isFixedNI_ = false; 
}

void 
BsCell::setFixedNI() 
{ 
  // isFixed is also true for FixedNI
  isFixed_   = true; 
  isFixedNI_ = true; 
}

void 
BsCell::setOrient(char orient) 
{ 
  orient_ = orient;
}

void 
BsCell::setXY(int x, int y)
{
  lx_ = x;
  ly_ = y;
  ux_ = lx_ + dx_;
  uy_ = ly_ + dy_;
}

// BsRow //
BsRow::BsRow(int idx        ,        
			 int ly         , 
			 int rowHeight  , 
			 int siteWidth  , 
			 int siteSpacing,
			 int offsetX    ,
			 int numSites)
{
  idx_          = idx;

  // Explicit Values from .scl file
  ly_           = ly;
  rowHeight_    = rowHeight;
  siteWidth_    = siteWidth;
  siteSpacing_  = siteSpacing;
  offsetX_      = offsetX;
  numSites_     = numSites;
  siteOrient_   = true;
  siteSymmetry_ = true;

  // Implicit Value
  rowWidth_ = numSites_ * siteSpacing_;
}

//  BsDie //
BsDie::BsDie()
{
  
}

// BookShelfDB //
BookShelfDB::BookShelfDB(int numNodes, int numTerminals)
{
  numBsCells_ = numNodes;
  numStdBsCells_ = numNodes - numTerminals;
  numMacros_ = numTerminals;

  cellPtrs_.reserve(numNodes);
  cellInsts_.reserve(numNodes);
}

void
BookShelfDB::makeBsCell(std::string name, int  width, int height, 
				        bool isTerminal, bool isTerminalNI)
{
  BsCell oneBsCell(name, width, height, isTerminal, isTerminalNI);
  cellInsts_.push_back(oneBsCell);
}

void
BookShelfDB::buildBsCellMap()
{
  printf("[BookShelfDB] Building Node Map\n");
  for(BsCell& c : cellInsts_)
  {
    cellPtrs_.push_back(&c);
    cellMap_.emplace(c.name(), &c);
  }
}

void
BookShelfDB::makeBsRow(int idx        ,        
				       int ly         , 
				       int rowHeight  , 
				       int siteWidth  , 
					   int siteSpacing,
					   int offsetX    ,
					   int numSites)
{
  BsRow oneBsRow(idx, ly, rowHeight, siteWidth, siteSpacing, offsetX, numSites);
  rowInsts_.push_back(oneBsRow);
}

void
BookShelfDB::buildBsRowMap()
{
  int maxX = 0;
  int maxY = 0;

  int minX = INT_MAX;

  printf("[BookShelfDB] Building Row Map\n");
  for(BsRow& r : rowInsts_)
  {
	if(r.ux() > maxX) maxX = r.ux();
	if(r.lx() < minX) minX = r.lx();
	if(r.uy() > maxY) maxY = r.uy();

    rowPtrs_.push_back(&r);
    rowMap_.emplace(r.id(), &r);
  }

  bsDie_.setUxUy(maxX, maxY);
  bsDie_.setLxLy(minX,    0);
  bsDiePtr_ = &bsDie_;

  printf("[BookShelfDB] Creating a Die(%d, %d)\n", maxX, maxY);

  numRows_ = rowPtrs_.size();
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
