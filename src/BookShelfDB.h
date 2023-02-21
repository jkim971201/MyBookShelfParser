#ifndef BOOKSHELF_DB_H
#define BOOKSHELF_DB_H

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

#define HASH_MAP std::unordered_map

namespace BookShelf
{

class Cell
{
  public:
	Cell();
    Cell(std::string name, int width, int height, bool Terminal, bool TerminalNI);

	std::string name() const { return name_; }

    int lx() const { return lx_; }
	int ly() const { return ly_; }
	int ux() const { return ux_; }
	int uy() const { return uy_; }

	int dx() const { return dx_; }
	int dy() const { return dy_; }

	char orient()       const { return orient_;}

	bool isTerminal()   const { return isTerminal_;   }
	bool isTerminalNI() const { return isTerminalNI_; }

	bool isFixed()      const { return isFixed_;      }
	bool isFixedNI()    const { return isFixedNI_;    }

	void setXY(int x, int y);

	void setFixed();
	void setFixedNI();

	void setOrient(char orient);

  private:
	std::string name_;

	int lx_;
	int ly_;
	int ux_;
	int uy_;

	int dx_;
    int dy_;

	char orient_;

	bool isTerminal_;
	bool isTerminalNI_;

	bool isFixed_;
	bool isFixedNI_;
};

class BookShelfDB
{
  public:
    BookShelfDB(int numNodes, int numTerminals);

	void makeCell(std::string name, int lx, int ly, bool Terminal, bool TerminalNI);

	const std::vector<Cell*>& cellVector() const { return cellPtrs_; }

	Cell* getCellbyName(std::string name) { return cellMap_[name]; }

	void buildMap();

	void verifyMap();
	void verifyVec();
	void verifyPtrVec();
	
  private:

	int numCells_;
	int numStdCells_;
	int numMacros_;
	
	std::vector<Cell*> cellPtrs_; 
	std::vector<Cell>  cellInsts_; 

	HASH_MAP<std::string, Cell*> cellMap_;
};
	
} // namespace BookShelf

#endif
