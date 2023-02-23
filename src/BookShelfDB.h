#ifndef BOOKSHELF_DB_H
#define BOOKSHELF_DB_H

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

#define HASH_MAP std::unordered_map

namespace BookShelf
{

class BsDie
{
  public:
    BsDie();

	int ux() const { return ux_; }
	int uy() const { return uy_; }
	int lx() const { return lx_; }
	int ly() const { return ly_; }

	int dx() const { return ux_ - lx_; }
	int dy() const { return uy_ - ly_; }

	void setUxUy(int ux, int uy) 
	{
	  ux_ = ux;
	  uy_ = uy;
	};

    void setLxLy(int lx, int ly) 
	{
	  lx_ = lx;
	  ly_ = ly;
	};

  private:
    int ux_;
    int uy_;
    int lx_;
	int ly_;
};

class BsRow
{
  public:
    BsRow(int idx, int ly, int rowHeight, 
		  int siteWidth, int siteSpacing, int offsetX, int numSites);

    int id() const { return idx_;                 }

	int dx() const { return rowWidth_;            }
	int dy() const { return rowHeight_;           }

    int lx() const { return offsetX_;             }
	int ly() const { return ly_;                  }
	int ux() const { return offsetX_ + rowWidth_; }
	int uy() const { return ly_ + rowHeight_;     }

	int siteSpacing() const { return siteSpacing_; }
	int numSites()    const { return numSites_;    }

  private:
	int  idx_;

	// These are written in .scl file
    int  ly_;           // 1. Coordinate of ly
	int  rowHeight_;    // 2. Height
	int  siteWidth_;    // 3. Site Width
	int  siteSpacing_;  // 4. Site Spacing
    bool siteOrient_;   // 5. Siteorient
	bool siteSymmetry_; // 6. Sitesymmetry
	int  offsetX_;      // 7. SubrowOrigin
	int  numSites_;     // 8. NumSites

	// RowWidth = numSites * (siteSpacing + siteWidth) - siteSpacing 
	int  rowWidth_; 
};


class BsCell
{
  public:
	BsCell();
    BsCell(std::string name, int width, int height, bool Terminal, bool TerminalNI);

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

	void makeBsCell(std::string name, int lx, int ly, bool Terminal, bool TerminalNI);
	void makeBsRow(int idx, int ly, int rowHeight, 
				   int siteWidth, int siteSpacing, int offsetX, int numSites);

	const std::vector<BsCell*>& cellVector() const { return cellPtrs_; }
	const std::vector<BsRow*>&  rowVector() const { return rowPtrs_; }

	BsCell* getBsCellbyName(std::string name) { return cellMap_[name]; }
	BsRow*  getBsRowbyId(int id) { return rowMap_[id];    }

	const BsDie* getDie() { return bsDiePtr_; };

	void buildBsCellMap();
	void buildBsRowMap();

	void verifyMap();
	void verifyVec();
	void verifyPtrVec();

	int numRows() const  { return numRows_; }
	int numCells() const { return numBsCells_; }
	
	int getDieWidth() const  { return bsDiePtr_->dx(); }
	int getDieHeight() const { return bsDiePtr_->dy(); }

  private:

	int numBsCells_;
	int numStdBsCells_;
	int numMacros_;

	int numRows_;

	std::vector<BsRow*> rowPtrs_; 
	std::vector<BsRow>  rowInsts_; 

	HASH_MAP<int, BsRow*> rowMap_;
	
	std::vector<BsCell*> cellPtrs_; 
	std::vector<BsCell>  cellInsts_; 

	HASH_MAP<std::string, BsCell*> cellMap_;

	BsDie bsDie_;
	BsDie* bsDiePtr_;
};
	
} // namespace BookShelf

#endif
