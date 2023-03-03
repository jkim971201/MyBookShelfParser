#include "Placer.h"
#include <stdio.h>

namespace PlacerBase
{

using namespace BookShelf;

Placer::Placer(BookShelfParser* bsParser)
{
	printf("[Placer] Start Placer construction.\n");
	db_ = std::make_shared<PlacerDB>(bsParser->getDB());
	db_->bookShelfDBtoPlacerDB();
	printf("[Placer] Placer is constructed successfully.\n");
}

} // namespace PlacerBase
