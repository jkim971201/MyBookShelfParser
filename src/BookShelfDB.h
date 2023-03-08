#ifndef BOOKSHELF_DB_H
#define BOOKSHELF_DB_H

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

#define HASH_MAP std::unordered_map

// By Jaekyung Im 
// 02. 28. 2023
// BookShelfDB does not have ability to do anything itself
// it's just a container of raw values from original
// input file ( .nodes .pl .scl .nets ) 
// You don't have to worry about some APIs are missing
// because these are enough for building a PlacerDB
// ** stupid implementations are existing...

namespace BookShelf
{

class BsPin;
class BsNet;
class BsCell;

class BsDie // should it be named as BsCore?
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

		int id() const { return idx_;                   }

		int dx() const { return rowWidth_;              }
		int dy() const { return rowHeight_;             }

		int lx() const { return offsetX_;               }
		int ly() const { return ly_;                    }
		int ux() const { return offsetX_ + rowWidth_;   }
		int uy() const { return ly_ + rowHeight_;       }

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

		// RowWidth = numSites * siteWidth - siteSpacing 
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

		void addNewPin(BsPin* pin) { pins_.push_back(pin); }

		const std::vector<BsPin*>& pins() const { return pins_; }

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

		std::vector<BsPin*> pins_;
};

class BsPin
{
	public:
		BsPin(BsCell* cell, int netID, double offsetX_, double offsetY_, char IO);

		int netID() const { return netID_; }

		BsCell* cell() const { return cell_; }
		BsNet*  net()  const { return net_;  }

		double offsetX() const { return offsetX_; }
		double offsetY() const { return offsetY_; }

		char IO() const { return io_; }

		void setNet(BsNet* net) { net_ = net; }

	private:
		int netID_;

		BsCell* cell_;
		BsNet* net_;
 
		double offsetX_;
		double offsetY_;

		// if Sink    == I
		// if Driver  == O
		// if Both(?) == B
		char io_; 
};

class BsNet
{
	public:
		BsNet(int id);

		int id() const { return id_; }
		int getDegree() const { return pins_.size(); }

		void addNewPin(BsPin* pin) { pins_.push_back(pin); }

		const std::vector<BsPin*>& pins() const { return pins_; }

	private:
		int id_;
		std::vector<BsPin*> pins_;
};

class BookShelfDB
{
	public:
		BookShelfDB(int numNodes, int numTerminals);

		void makeBsCell(std::string name, int lx, int ly, bool Terminal, bool TerminalNI);
		void makeBsNet(int netID);
		void makeBsPin(BsCell* cell, int netID, double offX, double offY, char IO);

		void makeBsRow(int idx, int ly, int rowHeight, 
		               int siteWidth, int siteSpacing, int offsetX, int numSites);

		const std::vector<BsCell*>& cellVector() const { return cellPtrs_; }
		const std::vector<BsRow*>&   rowVector() const { return rowPtrs_;  }
		const std::vector<BsNet*>&   netVector() const { return netPtrs_;  }
		const std::vector<BsPin*>&   pinVector() const { return pinPtrs_;  }

		BsNet*  getBsNetByID(int id)              { return netMap_[id];    }
		BsCell* getBsCellByName(std::string name) { return cellMap_[name]; }
		BsRow*  getBsRowbyID(int id)              { return rowMap_[id];    }

		BsDie* getDie() const { return bsDiePtr_; };

		void buildBsCellMap();
		void buildBsRowMap();
		void finishPinsAndNets();

		void verifyMap();
		void verifyVec();
		void verifyPtrVec();

		int numRows() const  { return numRows_; }
		int numCells() const { return numBsCells_; }

		int getDieWidth() const  { return bsDiePtr_->dx(); }
		int getDieHeight() const { return bsDiePtr_->dy(); }

		int rowHeight() const { return rowHeight_; }
		void setHeight(int rowHeight) { rowHeight_ = rowHeight; }

	private:
		int numRows_;
		int numBsCells_;
		int numStdBsCells_;
		int numMacros_;

		int rowHeight_;

		std::vector<BsRow*> rowPtrs_; 
		std::vector<BsRow>  rowInsts_; 

		HASH_MAP<int, BsRow*> rowMap_;

		std::vector<BsPin*> pinPtrs_; 
		std::vector<BsPin>  pinInsts_; 

		std::vector<BsNet*> netPtrs_; 
		std::vector<BsNet>  netInsts_; 

		HASH_MAP<int, BsNet*> netMap_;

		std::vector<BsCell*> cellPtrs_; 
		std::vector<BsCell>  cellInsts_; 

		HASH_MAP<std::string, BsCell*> cellMap_;

		BsDie bsDie_;
		BsDie* bsDiePtr_;
};

} // namespace BookShelf

#endif
