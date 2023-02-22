#include <iostream>
#include <cassert> // For Debug-Mode
#include <cstring>
#include <stdio.h>
#include <string.h>

#include "BookShelfDB.h"
#include "BookShelfParser.h"

#define BUF_SIZE 256
#define MAX_CELL_NAME 12


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

inline void getDir(char* file, char* dir)
{
  int next_to_last_slash = 0;
  int len = strlen(file);

  strcpy(dir, file);

  for(int i = 0; i < len; i++) 
  {
    if(file[i] == '/') 
	  next_to_last_slash = i + 1;
  }

  dir[next_to_last_slash] = '\0';
  // \0 == NULL
}

// What a stupid way....
inline void catDirName(char* dir, char* name)
{
  char temp[MAX_FILE_NAME];
  strcpy(temp, dir);
  strcat(temp, name);
  strcpy(name, temp);
}

inline void getSuffix(const char* token, char* sfx)
{
  int last_dot = 0;
  int len = strlen(token);

  for(int i = 0; i < len; i++) 
  {
    if(token[i] == '.') 
	  last_dot = i;
  }
  strcpy(sfx, &token[last_dot + 1]);
}

BookShelfParser::BookShelfParser()
{
  bookShelfDB_ = nullptr;
}

BookShelfParser::BookShelfParser(const char* aux_name) 
		: BookShelfParser()
{
  char suf[4];
  getSuffix(aux_name, suf);

  if(!strcmp(suf, "aux"))
  {
    strcpy(aux_, aux_name);
    getDir(aux_, dir_);
  }
  else
  {
    printf("[Parser] Make sure you specify .aux file\n");
	exit(0);
  }
}

void
BookShelfParser::Parse()
{
  printf("[Parser] Start Parse!\n");

  // Start from .aux file
  read_aux();
  read_nodes();
  read_pl();
  read_scl();
}

void
BookShelfParser::read_aux()
{
  printf("[Parser] Reading %s...\n", aux_);
  FILE *fp = fopen(aux_, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  char sfx[6]; // the longest is "nodes" (5 letters)

  if(fp == NULL)
  {
    printf("[Parser] Failed to open %s...\n", aux_);
    exit(0);
  }
  
  token = goNextLine(buf, " :", fp);

  if(strcmp(token, "RowBasedPlacement"))
  {
    printf("[Parser] Unknown Placement Type: %s\n", token);
    exit(0);
  }

  while(true)
  {
    token = getNextWord(" :\n");
    if(!token) break;	

	getSuffix(token, sfx);

    if(!strcmp(sfx, "nodes"))
	{
	  printf("[Parser] .nodes file detected.\n");
	  strcpy(nodes_, token);
	  catDirName(dir_, nodes_);
	}
	else if(!strcmp(sfx, "pl"))
	{
	  printf("[Parser] .pl file detected.\n");
	  strcpy(pl_, token);
	  catDirName(dir_, pl_);
	}
	else if(!strcmp(sfx, "scl"))
	{
	  printf("[Parser] .scl file detected.\n");
	  strcpy(scl_, token);
	  catDirName(dir_, scl_);
	}
	else
	{
	  printf("[Parser] not supported yet...\n");
	}
  }
}


void
BookShelfParser::read_nodes()
{
  printf("[Parser] Reading %s...\n", nodes_);

  FILE *fp = fopen(nodes_, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  if(fp == NULL)
  {
    printf("[Parser] Failed to open %s...\n", nodes_);
    exit(0);
  }

  // Skip Headlines
  while(!token || !strcmp(token, "UCLA") || token[0] == '#')
    token = goNextLine(buf, " \t\n", fp);

  // Read NumNodes (at this moment, buf == "NumNodes")
  assert(!strcmp(buf, "NumNodes"));
  token = getNextWord(" \t\n");
  token = getNextWord(" \t\n");
  int numNodes = atoi(token);
  token = goNextLine(buf, " \t\n", fp);
  
  // Read NumTerminals (at this moment, buf == "NumTerminals")
  assert(!strcmp(buf, "NumTerminals"));
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

  int width  = 0;
  int height = 0;
  bool isTerminal   = false;
  bool isTerminalNI = false;


  while(!feof(fp))
  {
    //char cellName[MAX_CELL_NAME];
	//strcpy(cellName, token);
	std::string cellName = std::string(token);

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

	// Make a BsCell (==Node)
	bookShelfDB_->makeBsCell(cellName, width, height, isTerminal, isTerminalNI);
	numLines++;

	if(numLines % 100000 == 0)
      printf("[Parser] Completed %d lines\n", numLines);
  }

  // For Debug
  bookShelfDB_->buildBsCellMap();
  assert(numNodes == bookShelfDB_->cellVector().size());

  printf("[Parser] Successfully Finished %s!\n", nodes_);

  //bookShelfDB_->verifyVec();
  //bookShelfDB_->verifyPtrVec();
  //bookShelfDB_->verifyMap();
}

void
BookShelfParser::read_pl()
{
  printf("[Parser] Reading %s...\n", pl_);

  FILE *fp = fopen(pl_, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  if(fp == NULL)
  {
    printf("[Parser] Failed to open %s...\n", pl_);
    exit(0);
  }

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

    std::string cellName = std::string(token);

	BsCell* myBsCell = bookShelfDB_->getBsCellbyName(cellName);
	//std::cout << token << std::endl;
	//std::cout << "=" << myBsCell->name() << std::endl;
	assert(cellName == myBsCell->name());

	// Get X Coordinate
    token = getNextWord(" \t");
	lx = atoi(token);

	// Get Y Coordinate
    token = getNextWord(" \t");
	ly = atoi(token);

	myBsCell->setXY(lx, ly);

	// Get Orient
    token = getNextWord(" \t:");
	orient = atoi(token);

	if(orient != 'N') 
	  myBsCell->setOrient(orient);

	// Get Move-Type
    token = getNextWord(" \t");
	if(token && !strcmp(token, "/FIXED\n")) 
	  myBsCell->setFixed();
	else if(token && !strcmp(token, "/FIXED_NI\n"))
	  myBsCell->setFixedNI();

	//printf("[Parser] %s: %d %d\n", cellName.c_str(), lx, ly);

    token = goNextLine(buf, " \t\n", fp);

	numLines++;

	if(numLines % 100000 == 0)
      printf("[Parser] Completed %d lines\n", numLines);
  }

  printf("[Parser] Successfully Finished %s!\n", pl_);
}


void
BookShelfParser::read_scl()
{
  printf("[Parser] Reading %s...\n", scl_);

  FILE *fp = fopen(scl_, "r");
  char *token = NULL;
  char buf[BUF_SIZE-1];

  if(fp == NULL)
  {
    printf("[Parser] Failed to open %s...\n", scl_);
    exit(0);
  }

  // Skip Headlines
  while(!token || !strcmp(token, "UCLA") || token[0] == '#')
    token = goNextLine(buf, " \t\n", fp);

  // Read NumNodes (at this moment, buf == "NumNodes")
  assert(!strcmp(buf, "NumRows"));
  token = getNextWord(" \t\n");
  token = getNextWord(" \t\n");
  int numRows = atoi(token);
  token = getNextWord(" \t\n");

  // Go to Next Line untill there are no blank lines anymore
  while(!token)
    token = goNextLine(buf, " \t\n", fp);

  int rowsRead = 0;

  while(!feof(fp))
  {
    int  ly;           
	int  rowHeight;    
	int  siteWidth;    
	int  siteSpacing;  
    char siteOrient;   
	char siteSymmetry;
	int  offsetX;     
	int  numSites;     
   
	// Read a Row (at this moment, token == "CoreRow")
    assert(!strcmp(token, "CoreRow"));

	while(true)
	{
      token = goNextLine(buf, " \t\n:", fp); 
	  // in the initial step of this loop 
	  // token == "Coordinate" at this moment

   	  if(!strcmp(token, "Coordinate"))
	  {
        token = getNextWord(" \t\n:");
        ly = atoi(token);
	  }
	  else if(!strcmp(token, "Height"))
      {
        token = getNextWord(" \t\n:");
        rowHeight = atoi(token);
	  }
	  else if(!strcmp(token, "Sitewidth"))
	  {
        token = getNextWord(" \t\n:");
        siteWidth = atoi(token);
	  }
	  else if(!strcmp(token, "Sitespacing"))
	  {
        token = getNextWord(" \t\n:");
        siteSpacing = atoi(token);
	  }
	  else if(!strcmp(token, "Siteorient"))
	  {
        token = getNextWord(" \t\n:");
        siteOrient = token[0];
	  }
	  else if(!strcmp(token, "Sitesymmetry"))
	  {
        token = getNextWord(" \t\n:");
        siteOrient = token[0];
	  }
	  else if(!strcmp(token, "SubrowOrigin"))
	  {
        token = getNextWord(" \t\n:");
        offsetX = atoi(token);
	    token = getNextWord(" \t\n:");
	    if(!strcmp(token, "NumSites"))
	    {
	      token = getNextWord(" \t\n:");
		  numSites = atoi(token);
	    }
	    else
	    {
	      printf("[Parser] Wrong BookShelf Syntax\n");
		  exit(0);
	    }
	  }
	  else if(!strcmp(token, "End"))
	  {
		bookShelfDB_->makeBsRow(rowsRead,  // idx
						        ly, rowHeight, 
								siteWidth, siteSpacing, 
								offsetX, numSites);
	    rowsRead++;
        token = goNextLine(buf, " \t\n:", fp); 
	    break;
	  }
	  else
	  {
	    printf("[Parser] Wrong BookShelf Syntax\n");
	    exit(0);
	  }
	}

	if(rowsRead % 500 == 0)
      printf("[Parser] Completed %d rows\n", rowsRead);

    if(rowsRead > numRows) 
	{
      printf("[Parser] Extra Rows more than %d will be ignored...\n", numRows);
	  break;
	}
  }

  bookShelfDB_->buildBsRowMap();
  assert(numRows == bookShelfDB_->rowVector().size());
  printf("[Parser] Successfully Finished %s!\n", scl_);
}

void
BookShelfParser::drawFromBookShelfDB()
{
  printf("[Parser] Draw from BookShelfDB\n");
  bsPainter_ = std::make_shared<BsPainter>(bookShelfDB_);
  bsPainter_->drawChip();
}


} // namespace BookShelf
