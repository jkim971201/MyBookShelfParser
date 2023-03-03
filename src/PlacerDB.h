#pragma once

#include <vector>
#include <memory>
#include "BookShelfDB.h"

namespace PlacerBase
{

using namespace BookShelf;

class Pin;
class Net;

class Cell
{
	public:
		Cell(BsCell* bsCell);

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

		int dx() const { return ux_ - lx_; }
		int dy() const { return uy_ - ly_; }

		int cx() const { return (ux_ + lx_)/2; }
		int cy() const { return (uy_ + ly_)/2; }

		bool isFixed() const { return isFixed_; }

		const std::vector<Pin*>& pins() const { return pins_; }

		void addNewPin(Pin* pin) { pins_.push_back(pin); }

	private:
		int lx_;
		int ly_;
		int ux_;
		int uy_;

		bool isFixed_;

		std::vector<Pin*> pins_;
};

class Net
{
	public:
		Net(BsNet* bsNet);

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

		double weight() const { return weight_; }

		const std::vector<Pin*>& pins() const { return pins_; }

		void addNewPin(Pin* pin) { pins_.push_back(pin); }

		void setWeight(double weight) { weight_ = weight; }

	private:

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
		Pin(BsPin* bsPin);

		char io() const { return io_; }

		int cx() const { return cx_; }
		int cy() const { return cy_; }
	
		BsNet*  net()  const { return net_;  }
		BsCell* cell() const { return cell_; }

	private:
		char io_;

		int cx_;
		int cy_;

		BsNet* net_;
		BsCell* cell_;
};

class Die
{
	public:
		Die();
		Die(BsDie* bsDie);

		int lx() const { return lx_; }
		int ly() const { return ly_; }
		int ux() const { return ux_; }
		int uy() const { return uy_; }

	private:
		int lx_;
		int ly_;
		int ux_;
		int uy_;
};

class PlacerDB
{
	public: 
		PlacerDB(std::shared_ptr<BookShelfDB> bookShelfDB);

		const std::vector<Cell*>& cells() const { return cellPtrs_; }
		const std::vector<Net*>&  nets()  const { return netPtrs_;  }
		const std::vector<Pin*>&  pins()  const { return pinPtrs_;  }

	private:
		void BookShelfDBToPlacerDB();

		bool bookShelfFlag_;
		bool lefdefFlag_;

		std::vector<Cell*> cellPtrs_; 
		std::vector<Cell>  cellInsts_; 

		std::vector<Net*>  netPtrs_; 
		std::vector<Net>   netInsts_; 

		std::vector<Pin*>  pinPtrs_; 
		std::vector<Pin>   pinInsts_; 

    Die  Die_;
    Die* DiePtr_;
};

} // namespace PlacerBase
