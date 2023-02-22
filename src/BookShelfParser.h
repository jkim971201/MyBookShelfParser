#ifndef BOOKSHELF_PARSER_H
#define BOOKSHELF_PARSER_H

#include <memory>
#include <string>
#include "BookShelfDB.h"

#define MAX_FILE_NAME 256

namespace BookShelf
{

class BookShelfDB;

class BookShelfParser
{
  public:
    BookShelfParser();
    BookShelfParser(const char* aux_name);

    void Parse();

	void read_aux();
	void read_nodes();
	void read_pl();
	void read_scl();

  private:
	std::shared_ptr<BookShelfDB> bookShelfDB_;

	// From .aux
	char   dir_[MAX_FILE_NAME];

	char   aux_[MAX_FILE_NAME];
	char nodes_[MAX_FILE_NAME];
	char  nets_[MAX_FILE_NAME];
	char    pl_[MAX_FILE_NAME];
	char   scl_[MAX_FILE_NAME];
};


} // namespace BookShelf

#endif
