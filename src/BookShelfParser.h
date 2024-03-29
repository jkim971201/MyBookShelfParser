#pragma once

#include <memory>
#include <string>
#include "BookShelfDB.h"
#include "Painter.h"

#define MAX_FILE_NAME 256

namespace BookShelf
{

using namespace Painter;

class BookShelfDB;

class BookShelfParser
{
	public:
		BookShelfParser();
		BookShelfParser(const char* aux_name);

		void Parse();
		void drawFromBookShelfDB();

	  void read_aux();
	  void read_nodes();
	  void read_pl();
	  void read_scl();
	  void read_nets();

    std::shared_ptr<BookShelfDB> const getDB() { return bookShelfDB_; }

		int rowHeight() const { return maxRowHeight_; }

		const char* getBenchName() const { return benchName_; }

	private:
		int maxRowHeight_;

		std::shared_ptr<BookShelfDB> bookShelfDB_;
		std::shared_ptr<BsPainter> bsPainter_;

		char benchName_[MAX_FILE_NAME];

		// From .aux
		char dir_  [MAX_FILE_NAME];

		char aux_  [MAX_FILE_NAME];
		char nodes_[MAX_FILE_NAME];
		char nets_ [MAX_FILE_NAME];
		char pl_   [MAX_FILE_NAME];
		char scl_  [MAX_FILE_NAME];
};

} // namespace BookShelf
