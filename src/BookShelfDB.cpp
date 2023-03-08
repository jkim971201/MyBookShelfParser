#include <iostream>
#include <vector>
#include <cassert>
#include <climits>
#include <cmath>
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

BsCell::BsCell(std::string name, int	width, int height, 
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
	rowWidth_ = numSites_ * siteSpacing_ + siteWidth_;
}

// BsDie //
BsDie::BsDie()
{
	
}

//	BsNet	//
BsNet::BsNet(int id)
{
	id_ = id;
}

// BsPin //
BsPin::BsPin(BsCell* cell, int netID, 
						 double offsetX, double offsetY,
						 char IO)
{
  cell_  = cell;
	net_   = nullptr;

	netID_ = netID;

	offsetX_ = offsetX;
	offsetY_ = offsetY;

	io_ = IO;
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
BookShelfDB::makeBsCell(std::string name, int	width, int height, 
												bool isTerminal, bool isTerminalNI)
{
	BsCell oneBsCell(name, 
					 width, 
					 height, 
					 isTerminal, isTerminalNI);
	cellInsts_.push_back(oneBsCell);
	// cellPtrs will be filled by buildBsCellMap()
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
BookShelfDB::makeBsRow(int idx,
                       int ly, 
                       int rowHeight, 
                       int siteWidth, 
                       int siteSpacing,
                       int offsetX,
                       int numSites)
{
	BsRow oneBsRow(idx, ly, 
                 rowHeight, 
                 siteWidth, 
                 siteSpacing, 
                 offsetX, 
                 numSites);
	rowInsts_.push_back(oneBsRow);
	// rowPtrs will be filled by buildBsRowMap()
}

void
BookShelfDB::makeBsNet(int netID)
{
	BsNet oneBsNet(netID);
	netInsts_.push_back(oneBsNet);
	// netPtrs will be filled by finishPinsAndNets()
}

void
BookShelfDB::makeBsPin(BsCell* cell, int netID, 
                       double offsetX, double offsetY,
                       char IO)
{
	double cell_half_w = static_cast<double>(cell->dx()) / 2;
	double cell_half_h = static_cast<double>(cell->dy()) / 2;

	if(std::abs(offsetX) > cell_half_w)
	{
		printf("[BookShelfDB] [Warning] Pin of Cell %s is out of Cell Boundary.\n", 
					                               cell->name().c_str());
		printf("[BookShelfDB] [Warning] Pin OffsetX %.1f is larger than Cell Half Width %.1f \n",
					                                      offsetX,                     cell_half_w);
	}

	if(std::abs(offsetY) > cell_half_h)
	{
		printf("[BookShelfDB] [Warning] Pin of Cell %s is out of Cell Boundary.\n", 
					                               cell->name().c_str());
		printf("[BookShelfDB] [Warning] Pin OffsetY %.1f is larger than Cell Half Height %.1f \n",
					                                      offsetY,                     cell_half_h);
	}

	BsPin oneBsPin(cell, netID, offsetX, offsetY, IO);

	pinInsts_.push_back(oneBsPin);
}

void
BookShelfDB::finishPinsAndNets()
{
	printf("[BookShelfDB] Building Net maps.\n");

  for(auto& net : netInsts_)
	{
		netPtrs_.push_back(&net); 
		netMap_[net.id()] = &net; 
	}

	printf("[BookShelfDB] Adding pins to cells and nets.\n");

  for(auto& pin : pinInsts_)
	{
		pinPtrs_.push_back(&pin);
	  pin.cell()->addNewPin(&pin);
	  if(!pin.net()) 
			pin.setNet(getBsNetByID(pin.netID()));
		else
		{
		  printf("[BookShelfDB] Unknown Error...\n");
			exit(0);
		}

	  if(pin.net()) 
			pin.net()->addNewPin(&pin);
		else
		{
		  printf("[BookShelfDB] Unknown Error...\n");
			exit(0);
		}
	}
}

void
BookShelfDB::buildBsRowMap()
{
	int maxX = 0;
	int maxY = 0;

	int minX = INT_MAX;
	int minY = INT_MAX;

	printf("[BookShelfDB] Building Row Map\n");
	for(BsRow& r : rowInsts_)
	{
	  if(r.ux() > maxX) maxX = r.ux();
	  if(r.lx() < minX) minX = r.lx();
	  if(r.uy() > maxY) maxY = r.uy();
	  if(r.ly() < minY) minY = r.ly();
		rowPtrs_.push_back(&r);
		rowMap_.emplace(r.id(), &r);
	}

	for(auto& c : cellPtrs_)
    if(c->uy() < minY) minY = c->uy();

	bsDie_.setUxUy(maxX, maxY);
	bsDie_.setLxLy(minX, minY);
	bsDiePtr_ = &bsDie_;

	printf("[BookShelfDB] Creating a Die (%d, %d) - (%d, %d) \n", 
                                   	maxX, maxY, minX, minY);
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
	for(int i = 0; i < cellPtrs_.size(); i++)
	{
		std::cout << "cell name: " << cellPtrs_[i]->name() << std::endl;
		std::cout << "cell width: " << cellPtrs_[i]->dx() << std::endl;
	}
}

} // namespace BookShelf
