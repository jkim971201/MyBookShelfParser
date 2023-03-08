#include "Placer.h"
#include <stdio.h>

namespace PlacerBase
{

using namespace BookShelf;

Placer::Placer(BookShelfParser* bsParser)
{
	printf("[Placer] Start Placer construction.\n");

	db_ = std::make_shared<PlacerDB>(bsParser->getBenchName(),
			                             bsParser->getDB());
	db_->bookShelfDBtoPlacerDB();

	printf("[Placer] Placer is constructed successfully.\n");
	db_->printInfo();
}

void
Placer::drawFromPlacerDB()
{
	printf("[Placer] Draw from PlacerDB.\n");
	plPainter_ = std::make_shared<PlPainter>(db_);
	plPainter_->drawChip();
}

} // namespace PlacerBase
