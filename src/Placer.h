#pragma once

#include <stdio.h>
#include <memory>
#include "BookShelfParser.h"
#include "Painter.h"
#include "PlacerDB.h"

namespace PlacerBase
{

using namespace BookShelf;
using namespace Painter;

class Placer
{
	public:
		Placer(BookShelfParser* bsParser);

		void drawFromPlacerDB();

	private:
		std::shared_ptr<PlacerDB>  db_;
		std::shared_ptr<PlPainter> plPainter_;
};

} // namespace PlacerBase
