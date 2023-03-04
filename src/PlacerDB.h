#pragma once

#include <vector>
#include <memory>
#include <string>
#include "BookShelfDB.h"

#define HASH_MAP std::unordered_map

namespace PlacerBase
{

using namespace BookShelf;

class Pin;
class Net;

class Cell
{
	public:
		Cell();
		Cell(BsCell* BsCell, int height);
		// height == criterion of macro

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

		int dx() const { return dx_; }
		int dy() const { return dy_; }

		int cx() const { return (ux_ + lx_)/2; }
		int cy() const { return (uy_ + ly_)/2; }

		bool isFixed() const { return isFixed_; }
		bool isMacro() const { return isMacro_; }

		const std::vector<Pin*>& pins() const { return pins_; }

		void addNewPin(Pin* pin) { pins_.push_back(pin); }

		BsCell* bsCell() const { return bsCell_; }

	private:
		BsCell* bsCell_;

		int lx_;
		int ly_;
		int ux_;
		int uy_;

		int dx_;
		int dy_;

		bool isMacro_;
		bool isFixed_;

		std::vector<Pin*> pins_;
};

class Net
{
	public:
		Net();
		Net(BsNet* BsNet);

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

		int hpwl() const { return (ux_ - lx_ + uy_ - ly_); } 

		double weight() const { return weight_; }

		const std::vector<Pin*>& pins() const { return pins_; }

		void addNewPin(Pin* pin) { pins_.push_back(pin); }

		void setWeight(double weight) { weight_ = weight; }

		void updateBBox();

		BsNet* bsNet() const { return bsNet_; }

	private:

		BsNet* bsNet_;

		int lx_; // Lx of BBox
		int ly_; // Ly of BBox
		int ux_; // Ux of BBox
		int uy_; // Uy of BBox

		double weight_;

		std::vector<Pin*> pins_;
};

class Pin
{
	public:
		Pin();
		Pin(BsPin* BsPin);

		char io() const { return io_; }

		int cx() const { return cx_; }
		int cy() const { return cy_; }
	
		Net*  net()  const { return net_;  }
		Cell* cell() const { return cell_; }

		BsPin* bsPin() const { return bsPin_; }

		void updatePinLocation(Cell* cell);

	private:
		BsPin* bsPin_;

		char io_;

		int cx_;
		int cy_;

		int offsetX_;
		int offsetY_;

		Net* net_;
		Cell* cell_;
};

class Die
{
	public:
		Die();

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

		void init(BsDie* bsDie);

	private:
		int lx_;
		int ly_;
		int ux_;
		int uy_;
};

class PlacerDB
{
	public: 
		PlacerDB();
		PlacerDB(const char* benchName,
				     std::shared_ptr<BookShelfDB> bookShelfDB);

		const std::vector<Cell*>& cells() const { return cellPtrs_; }
		const std::vector<Net*>&  nets()  const { return netPtrs_;  }
		const std::vector<Pin*>&  pins()  const { return pinPtrs_;  }

		Die* die() const { return diePtr_; }

		bool ifBookShelf() const { return bookShelfFlag_; }
		bool ifLEFDEF()    const { return lefdefFlag_;    }

		void bookShelfDBtoPlacerDB();

		void printInfo() const;

	private:
		std::shared_ptr<BookShelfDB> bsDB_;
		const char* benchName_;

		int maxRowHeight_;

		int numStdCells_;
		int numMacro_;
		int numFixed_;
		int numMovable_;

		int initialHPWL_;

		bool bookShelfFlag_;
		bool lefdefFlag_;

		std::vector<Cell*> cellPtrs_; 
		std::vector<Cell>  cellInsts_; 

		std::vector<Net*>  netPtrs_; 
		std::vector<Net>   netInsts_; 

		std::vector<Pin*>  pinPtrs_; 
		std::vector<Pin>   pinInsts_; 

		HASH_MAP<BsCell*, Cell*> cellMap_;
		HASH_MAP<BsNet* ,  Net*>  netMap_;

    Die  die_;
    Die* diePtr_;

		void makeCell(BsCell* cell);
		void makeNet(BsNet* net);
		void makePin(BsPin* pin);
};

} // namespace PlacerBase
