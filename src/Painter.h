#pragma once

#include <memory>
#include "CImg.h"
#include "PlacerDB.h"
#include "BookShelfDB.h"

namespace Painter
{

static const unsigned char white[]  = {255, 255, 255},
                           black[]  = {  0,   0,   0},
                           red[]    = {255,   0,   0},
                           blue[]   = {120, 200, 255},
                           green[]  = {0,   255,   0},
                           purple[] = {255, 100, 255},
                           orange[] = {255, 165,   0},
                           yellow[] = {255, 255,   0},
                           gray[]   = {204, 204, 204},
                           aqua[]   = {204, 204, 255};

using namespace BookShelf;
using namespace PlacerBase;
using namespace cimg_library;

typedef const unsigned char* Color;

class Painter
{
	public:
		Painter();
		void show();

	protected:
		int getX(int dbX);
		int getY(int dbY);

		void drawLine(int x1, int y1, int x2, int y2);
		void drawLine(int x1, int y1, int x2, int y2, Color c);


		void drawRect(int lx, int ly, int ux, int uy, Color rect_c, int w);
		void drawRect(int lx, int ly, int ux, int uy, Color rect_c, 
		              Color line_c = black, int w = 0);
		              // line_c: border-line color
		              // w : Thicnkness of border-line

		// Pure virtual functions
		// These make Painter as an abstract class
		virtual void drawDie() = 0;
		virtual void drawRows() = 0;
		virtual void drawCells() = 0;
		virtual void drawChip() = 0;

		int offsetX_;
		int offsetY_;

		double scale_; 
		// Scaling Factor to fit the window size

		int maxWidth_;
		int maxHeight_;
	
		//CImg library
		CImg<unsigned char>* canvas_;
		CImg<unsigned char>* img_;
		CImgDisplay* window_;
};

class BsPainter : public Painter
{
	public:
		BsPainter(std::shared_ptr<BookShelf::BookShelfDB> bsDB);

		void drawDie() override;
		void drawRows() override;
		void drawCells() override;
		void drawChip() override;

		void drawRow(BookShelf::BsRow* bsRow);

	private:
		void drawCell(BookShelf::BsCell* bsCell);

		std::shared_ptr<BookShelf::BookShelfDB> bookShelfDB_;
};

class PlPainter : public Painter
{
	public:
		PlPainter(std::shared_ptr<PlacerBase::PlacerDB> plDB);

		void drawDie() override;
		void drawRows() override;
		void drawCells() override;
		void drawChip() override;
		void drawNets();

		void drawNet(PlacerBase::Net* net);

	private:
		void drawCell(PlacerBase::Cell* cell);

		std::shared_ptr<PlacerBase::PlacerDB> plDB_;
};


} // namespace Painter
