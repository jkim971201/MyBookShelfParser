#include <iostream>
#include <cassert> // For Debug-Mode
#include <cstring>
#include <stdio.h>

#include "BookShelfDB.h"
#include "BookShelfParser.h"

#define BUF_SIZE 256


namespace BookShelf
{

inline char* getNextWord(const char* delimiter)
{
  // strtok() has "static char *olds" inside
  // if strtok() gets NULL, then it starts from the "olds"
  // so you can get the next word
  // you must use strtok(buf, ~) to capture the first word
  // otherwise, you will get segmentation fault
  // (this is implemented in goNextLine)
  return strtok(NULL, delimiter);
}

// Go Next Line and Get First Word
inline char* goNextLine(char* buf, const char* delimiter, FILE* fp)
{
  fgets(buf, BUF_SIZE-1, fp);
  return strtok(buf, delimiter);
}

BookShelfParser::BookShelfParser(const char* FileName) 
		: bookShelfDB_(nullptr)
{
  nodesFileName_ = std::string(FileName);
}

void
BookShelfParser::Parse(const char* fileName1, const char* fileName2)
{
  printf("[Parser] Start Parse!\n");
  read_nodes(fileName1);
  read_pl(fileName2);
}

void
BookShelfParser::read_nodes(const char* fileName)
{
  printf("[Parser] Reading %s...\n", fileName);

  FILE *fp = fopen(fileName, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  // Skip Headlines
  while(!token || !strcmp(token, "UCLA") || token[0] == '#')
    token = goNextLine(buf, " \t\n", fp);

  // Read NumNodes (at this moment, buf == "NumNodes")
  token = getNextWord(" \t\n");
  token = getNextWord(" \t\n");
  int numNodes = atoi(token);
  token = goNextLine(buf, " \t\n", fp);
  
  // Read NumTerminals (at this moment, buf == "NumTerminals")
  token = getNextWord(" \t\n");
  token = getNextWord(" \t\n");
  int numTerminals = atoi(token);
  token = getNextWord(" \t\n");
  printf("[Parser] Total Nodes: %d\n", numNodes);
  printf("[Parser] Total Terms: %d\n", numTerminals);

  // Go to Next Line untill there are no blank lines anymore
  while(!token)
    token = goNextLine(buf, " \t\n", fp);

  bookShelfDB_ = std::make_shared<BookShelfDB>(numNodes, numTerminals);

  int numLines = 0;

  while(!feof(fp))
  {
	int width  = 0;
	int height = 0;
	bool isTerminal   = false;
	bool isTerminalNI = false;

	char* cellName = token;

	// Get Width
    token = getNextWord(" \t");
	width = atoi(token);

	// Get Height
    token = getNextWord(" \t");
	height = atoi(token);

	// Check Terminal
    token = getNextWord(" \t");
	if(token && !strcmp(token, "terminal\n")) 
	  isTerminal   = true;
	else if(token && !strcmp(token, "terminal_NI\n"))
	  isTerminalNI = true;

    token = goNextLine(buf, " \t\n", fp);

	// Make a Cell (==Node)
	bookShelfDB_->makeCell(cellName, width, height, isTerminal, isTerminalNI);
	numLines++;

	if(numLines % 100000 == 0)
      printf("[Parser] Completed %d lines\n", numLines);
  }

  // For Debug
  assert(numNodes == bookShelfDB_->cellVector().size());

  printf("[Parser] Successfully Finished %s!\n", fileName);
}

void
BookShelfParser::read_pl(const char* fileName)
{
  printf("[Parser] Reading %s...\n", fileName);

  FILE *fp = fopen(fileName, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  // Skip Headlines
  while(!token || !strcmp(token, "UCLA") || token[0] == '#')
    token = goNextLine(buf, " \t\n", fp);

  // Go to Next Line untill there are no blank lines anymore
  while(!token)
    token = goNextLine(buf, " \t\n", fp);

  int numLines = 0;

  while(!feof(fp))
  {
	int lx = 0;
	int ly = 0;

    char orient;

	bool isFixed   = false;
	bool isFixedNI = false;

	char* cellName = token;

	// Get X Coordinate
    token = getNextWord(" \t");
	lx = atoi(token);

	// Get Y Coordinate
    token = getNextWord(" \t");
	ly = atoi(token);

	// Get Orient
    token = getNextWord(" \t:");
	orient = atoi(token);

	// Get Move-Type
    token = getNextWord(" \t");
	if(token && !strcmp(token, "/FIXED\n")) 
	  isFixed   = true;
	else if(token && !strcmp(token, "/FIXED_NI\n"))
	  isFixedNI = true;

    token = goNextLine(buf, " \t\n", fp);
  }
}

} // namespace BookShelf
