#pragma once

#include <stdio.h>
#include <memory>
#include "BookShelfParser.h"
#include "PlacerDB.h"

namespace PlacerBase
{

using namespace BookShelf;

class Placer
{
	public:
		Placer(BookShelfParser* bsParser);

	private:
		std::shared_ptr<PlacerDB> db_;
};

} // namespace PlacerBase
