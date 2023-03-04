#include <vector>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <climits> // For INT_MAX

#include "PlacerDB.h"
#include "BookShelfDB.h"

#define BS_DB 2

namespace PlacerBase
{

using namespace BookShelf;

// Cell // 
// height == criterion of macro
Cell::Cell()
	: lx_(0), ly_(0), 
	  ux_(0), uy_(0),
		dx_(0), dy_(0),
		isMacro_(false), isFixed_(false),
		bsCell_(nullptr)
{}

Cell::Cell(BsCell* bsCell, int height) : Cell()
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
Pin::Pin()
	: io_('I'), 
		cx_(0), cy_(0),
		offsetX_(0), offsetY_(0),
		net_(nullptr), cell_(nullptr)
{}

Pin::Pin(BsPin* bsPin) : Pin()
{
	io_ = bsPin->IO();
	bsPin_ = bsPin;
	offsetX_ = static_cast<int>(BS_DB * bsPin->offsetX());
	offsetY_ = static_cast<int>(BS_DB * bsPin->offsetY());
}

void
Pin::updatePinLocation(Cell* cell) 
{
	// if we use cell_ instead of given Cell*,
	// we have to check whether cell_ is not nullptr 
	// everytime we call this function
	cx_ = cell->cx() + offsetX_;
	cy_ = cell->cy() + offsetY_;
}

// Net //
Net::Net()
	: lx_(INT_MAX), ly_(INT_MAX), 
	  ux_(INT_MIN), uy_(INT_MIN),
		weight_(1.0),
		bsNet_(nullptr)
{}

Net::Net(BsNet* bsNet) : Net()
{
	weight_ = 1.0;
	bsNet_ = bsNet;
}

void
Net::updateBBox()
{
	// To detect an error,
	// We initilize them as INT_MAX
	// so that an un-initilized net will
	// make an invalid HPWL
	lx_ = ly_ = INT_MAX;
	ux_ = uy_ = INT_MIN;

	for(auto& p : pins_)
	{
		lx_ = std::min(p->cx(), lx_);
		ly_ = std::min(p->cy(), ly_);
		ux_ = std::max(p->cx(), ux_);
		uy_ = std::max(p->cy(), uy_);
	}
}

// Die //
Die::Die() 
	: lx_(0), ly_(0),
	  ux_(0), uy_(0)
{}

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
	{
		makeCell(c);
	}

	for(auto& c : cellInsts_)
	{
		cellMap_[c.bsCell()] = &c;
		cellPtrs_.push_back(&c);

		// For Information
		if(c.isFixed()) numFixed_++;
		else 				    numMovable_++;
		if(c.isMacro()) numMacro_++;
		else            numStdCells_++;
	}

	for(auto& n : bsDB_->netVector())
	{
		makeNet(n);
	}

	for(auto& n : netInsts_)
	{
		netMap_[n.bsNet()] = &n;
		netPtrs_.push_back(&n);
	}

	//std::cout << "NetMap Size: " << netMap_.size()  << std::endl;
	//std::cout << "CellMap Size: " << cellMap_.size()  << std::endl;

	for(auto& p : bsDB_->pinVector())
	{
		makePin(p);
	}

	for(auto& p : pinInsts_)
	{
		pinPtrs_.push_back(&p);
		if(netMap_.find(p.bsPin()->net()) != netMap_.end())
			netMap_[p.bsPin()->net()]->addNewPin(&p);
		else
		{
			printf("[PlacerDB] Net DB Construction Error.\n");
			exit(0);
		}

		if(cellMap_.find(p.bsPin()->cell()) != cellMap_.end())
			cellMap_[p.bsPin()->cell()]->addNewPin(&p);
		else
		{
			printf("[PlacerDB] Cell DB Construction Error.\n");
			exit(0);
		}
	}

	// Initialize Pin Location
	for(auto& c : cellPtrs_)
		for(auto& p : c->pins())
			p->updatePinLocation(c);

	initialHPWL_ = 0;

	// Initilize Net BBox
	for(auto& n : netPtrs_)
	{
		n->updateBBox();
		initialHPWL_ += n->hpwl();
	}


	printf("[PlacerDB] BookShelfDB is converted to PlacerDB successfully.\n");
}

PlacerDB::PlacerDB()
	: bsDB_(nullptr),
	  benchName_(nullptr),
		maxRowHeight_(0),
		numStdCells_(0), numMacro_(0),
		numFixed_(0), numMovable_(0),
		initialHPWL_(0),
		bookShelfFlag_(false), lefdefFlag_(false),
		diePtr_(nullptr)
{}

PlacerDB::PlacerDB(const char* benchName,
		               std::shared_ptr<BookShelfDB> bookShelfDB) : PlacerDB()
{
	bsDB_ = bookShelfDB;
	benchName_ = benchName;

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

void
PlacerDB::printInfo() const
{
	printf("  --------------------------------\n");
	printf("  |     Benchmark Statistics     |\n");
	printf("  --------------------------------\n");
	printf("  | Bench        | %10s    |\n", benchName_);
	printf("  | NumStd       | %10d    |\n", numStdCells_);
	printf("  | NumMacro     | %10d    |\n", numMacro_);
	printf("  | NumMovables  | %10d    |\n", numMovable_);
	printf("  | NumFixed     | %10d    |\n", numFixed_);
	printf("  | NumTotal     | %10d    |\n", numStdCells_ + numMacro_);
	if(initialHPWL_ > 0 && initialHPWL_ != INT_MAX) 
	printf("  | Initial HPWL | %10d    |\n", initialHPWL_/BS_DB);
	printf("  --------------------------------\n");
}

} // namespace PlacerBase
