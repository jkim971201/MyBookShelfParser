#include <vector>
#include <memory>
#include <stdio.h>

#include "PlacerDB.h"
#include "BookShelfDB.h"

#define BS_DB 2

namespace PlacerBase
{

using namespace BookShelf;

// Cell // 
Cell::Cell(BsCell* bsCell)
{
	lx_ = BS_DB * bsCell->lx();
	ly_ = BS_DB * bsCell->ly();
	ux_ = BS_DB * bsCell->ux();
	uy_ = BS_DB * bsCell->uy();

	isFixed_ = bsCell->isFixed();
}

// Pin // 
Pin::Pin(BsPin* bsPin)
{
	io_ = bsPin->IO();
}

// Net //
Net::Net(BsNet* bsNet)
{
	weight_ = 1.0;
}

// Die //
Die::Die() {}
Die::Die(BsDie* bsDie)
{
	lx_ = BS_DB * bsDie->lx();
	ly_ = BS_DB * bsDie->ly();
	ux_ = BS_DB * bsDie->ux();
	uy_ = BS_DB * bsDie->uy();
}

// PlacerDB // 
void
PlacerDB::BookShelfDBToPlacerDB()
{
	printf("[PlacerDB] BookShelfDB is converted to PlacerDB.\n");
}

PlacerDB::PlacerDB(std::shared_ptr<BookShelfDB> bookShelfDB)
{
	bookShelfFlag_ = true;
	lefdefFlag_    = false;

	printf("[PlacerDB] PlacerDB is constructed successfully!\n");
}

} // namespace PlacerBase
