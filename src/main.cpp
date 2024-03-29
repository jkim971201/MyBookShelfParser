#include <stdio.h>
#include <iostream>
#include <string>
#include "BookShelfParser.h"
#include "Placer.h"

using namespace BookShelf;
using namespace PlacerBase;

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf(".aux file is missing!\n");
		exit(0);
	}

	const char* aux_file = argv[1];

	BookShelfParser Parser(aux_file);
	Parser.Parse();
	//Parser.drawFromBookShelfDB();
	
	Placer Placer(&Parser);
	Placer.drawFromPlacerDB();

	return 0;
}
