#include <stdio.h>
#include <iostream>
#include <string>
#include "BookShelfParser.h"

using namespace BookShelf;

int main(int argc, char** argv)
{

  if(argc < 3)
  {
	std::cout << ".pl file is missing!" << std::endl;
    exit(0);
  }

  const char* nodesFileName = argv[1];
  const char* plFileName = argv[2];

  BookShelfParser Parser(nodesFileName);
  Parser.Parse(nodesFileName, plFileName);

  return 0;
}
