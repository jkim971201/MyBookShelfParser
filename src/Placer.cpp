#include "Placer.h"
#include <stdio.h>

namespace PlacerBase
{

using namespace BookShelf;

Placer::Placer(BookShelfParser* bsParser)
{
	db_ = std::make_shared<PlacerDB>(bsParser->getDB());
	printf("[Placer] Placer is constructed successfully.\n");
}

} // namespace PlacerBase
