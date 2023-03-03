#include <vector>
#include <memory>
#include <stdio.h>
#include <iostream>

#include "PlacerDB.h"
#include "BookShelfDB.h"

#define BS_DB 2

namespace PlacerBase
{

using namespace BookShelf;

// Cell // 
// height == criterion of macro
Cell::Cell(BsCell* bsCell, int height)
{
	bsCell_ = bsCell;

	lx_ = BS_DB * bsCell->lx();
	ly_ = BS_DB * bsCell->ly();
	ux_ = BS_DB * bsCell->ux();
	uy_ = BS_DB * bsCell->uy();

	dx_ = BS_DB * bsCell->dx();
	dy_ = BS_DB * bsCell->dy();

	isFixed_ = bsCell->isFixed();
	if(bsCell->dy() > height) isMacro_ = true;
	else                      isMacro_ = false;
}

// Pin // 
Pin::Pin(BsPin* bsPin)
{
	io_ = bsPin->IO();
	bsPin_ = bsPin;
}

// Net //
Net::Net(BsNet* bsNet)
{
	weight_ = 1.0;
	bsNet_ = bsNet;
}

// Die //
Die::Die() {}

void
Die::init(BsDie* bsDie)
{
	lx_ = BS_DB * bsDie->lx();
	ly_ = BS_DB * bsDie->ly();
	ux_ = BS_DB * bsDie->ux();
	uy_ = BS_DB * bsDie->uy();

	printf("[PlacerDB] Initialize Die (%d, %d) - (%d, %d)\n",
	                                  lx_, ly_,   ux_, uy_);
}

// PlacerDB // 
void
PlacerDB::makeCell(BsCell* cell)
{
	Cell myCell(cell, maxRowHeight_);
	cellInsts_.push_back(myCell);
}

void
PlacerDB::makeNet(BsNet* net)
{
	Net myNet(net);
	netInsts_.push_back(myNet);
}

void
PlacerDB::makePin(BsPin* pin)
{
	Pin myPin(pin);
	pinInsts_.push_back(myPin);
}

void
PlacerDB::bookShelfDBtoPlacerDB()
{
	printf("[PlacerDB] Start BookShelfDB -> PlacerDB Conversion\n");

	for(auto& c : bsDB_->cellVector())
		makeCell(c);

	for(auto& c : cellInsts_)
	{
		cellMap_[c.bsCell()] = &c;
		cellPtrs_.push_back(&c);
	}

	for(auto& n : bsDB_->netVector())
		makeNet(n);

	for(auto& n : netInsts_)
	{
		netMap_[n.bsNet()] = &n;
		netPtrs_.push_back(&n);
	}

	//std::cout << "NetMap Size: " << netMap_.size()  << std::endl;
	//std::cout << "CellMap Size: " << cellMap_.size()  << std::endl;

	for(auto& p : bsDB_->pinVector())
		makePin(p);

	for(auto& p : pinInsts_)
	{
		pinPtrs_.push_back(&p);
		//if(netMap_.find(p.bsPin()->net()) == netMap_.end()) std::cout << "End!" << std::endl;
		netMap_[p.bsPin()->net()]->addNewPin(&p);
	 	cellMap_[p.bsPin()->cell()]->addNewPin(&p);
	}

	printf("[PlacerDB] BookShelfDB is converted to PlacerDB successfully.\n");
}

PlacerDB::PlacerDB(std::shared_ptr<BookShelfDB> bookShelfDB)
{
	bsDB_ = bookShelfDB;

	maxRowHeight_ = bsDB_->rowHeight();
	if(maxRowHeight_ < 0)
	{
		printf("[PlacerDB] Row Height must be larger than 0!\n");
		exit(0);
	}

	bookShelfFlag_ = true;
	lefdefFlag_    = false;

	die_.init(bsDB_->getDie());
	diePtr_ = &die_;
}

} // namespace PlacerBase
