#ifndef BOOKSHELF_PARSER_H
#define BOOKSHELF_PARSER_H

#include <memory>
#include <string>
#include "BookShelfDB.h"

namespace BookShelf
{

class BookShelfDB;

class BookShelfParser
{
  public:
    BookShelfParser(const char* fileName);

    void Parse(const char* fileName1, const char* fileName2);

	void read_nodes(const char* fileName);
	void read_pl(const char* fileName);

  private:
	std::shared_ptr<BookShelfDB> bookShelfDB_;

	// From .aux
	std::string nodesFileName_;
	//char* nodesFileName_;
	//char* netsFileName_;
	//char* plFileName_;
	//char* sclFileName_;
};


} // namespace BookShelf

#endif
