#include "Painter.h"
#include "PlacerDB.h"
#include "BookShelfDB.h"
#include <stdio.h>
#include "CImg.h"

#define MAX_W 2000
#define MAX_H 2000

#define DIE_OFFSET_X 20
#define DIE_OFFSET_Y 20

#define DIE_LINE_THICKNESS 3

namespace Painter
{

using namespace BookShelf;
using namespace PlacerBase;
using namespace cimg_library;

static const Color MACRO_COLOR    = aqua;
static const Color STD_CELL_COLOR = red;
static const Color STD_CELL_LINE_COLOR = black;

// Painter Interface //
Painter::Painter() 
{
	printf("[GUI] Constructing a Painter.\n");

	offsetX_ = DIE_OFFSET_X;
	offsetY_ = DIE_OFFSET_Y;

	int canvasX = MAX_H + 2 * DIE_OFFSET_X;
	int canvasY = MAX_H + 2 * DIE_OFFSET_Y;

	canvas_ = new CImg<unsigned char>(canvasX, canvasY, 1, 3, 255);
	canvas_->draw_rectangle(0, 0, canvasX, canvasY, white);

	img_ = new CImg<unsigned char>(*canvas_);
	window_ = new CImgDisplay(canvasX, canvasY, "Placement GUI");
}

int
Painter::getX(int dbX)
{
	double tempX = static_cast<double>(dbX);
	tempX = scale_ * tempX;
	return (static_cast<double>(tempX) + offsetX_);
}

int
Painter::getY(int dbY)
{
	double tempY = static_cast<double>(maxHeight_ - dbY);
	tempY = scale_ * tempY;
	return (static_cast<int>(tempY) + offsetY_);
}

void 
Painter::drawLine(int x1, int y1, int x2, int y2)
{
	img_->draw_line(x1, y1, x2, y2, black);
}

void 
Painter::drawLine(int x1, int y1, int x2, int y2, Color color)
{
	img_->draw_line(x1, y1, x2, y2, color);
}


void
Painter::drawRect(int lx, int ly, int ux, int uy, Color rect_c, int w)
{
	drawRect(lx, ly, ux, uy, rect_c, black, w);
}

void
Painter::drawRect(int lx, int ly, int ux, int uy, Color rect_c,
                  Color line_c, int w)
{
	img_->draw_rectangle(lx, ly, ux, uy, rect_c);
	drawLine(lx, ly, ux, ly, line_c);
	drawLine(ux, ly, ux, uy, line_c);
	drawLine(ux, uy, lx, uy, line_c);
	drawLine(lx, uy, lx, ly, line_c);

	int xd = (ux > lx) ? 1 : -1; 
	int yd = (uy > ly) ? 1 : -1; 

	if(w > 0)
	{
		for(int i = 1; i < w+1; i++)
		{
			drawLine(lx + xd * i, ly + yd * i, 
			         ux - xd * i, ly + yd * i, line_c);

			drawLine(ux - xd * i, ly + yd * i, 
			         ux - xd * i, uy - yd * i , line_c);

			drawLine(ux - xd * i, uy - yd * i, 
			         lx + xd * i, uy - yd * i, line_c);

			drawLine(lx + xd * i, uy - yd * i, 
			         lx + xd * i, ly + yd * i, line_c);

			drawLine(lx - xd * i, ly - yd * i, 
			         ux + xd * i, ly - yd * i, line_c);

			drawLine(ux + xd * i, ly - yd * i, 
			         ux + xd * i, uy + yd * i , line_c);

			drawLine(ux + xd * i, uy + yd * i, 
			         lx - xd * i, uy + yd * i, line_c);

			drawLine(lx - xd * i, uy + yd * i, 
			         lx - xd * i, ly - yd * i, line_c);
		}
	}
}

void 
Painter::show()
{
	img_->display(*window_);
	while(!window_->is_closed() && !window_->is_keyESC())
			wait(window_);
	exit(0);
}


// PlacerDB Painter //
PlPainter::PlPainter(std::shared_ptr<PlacerBase::PlacerDB> plDB)
{
	plDB_ = plDB;

	maxWidth_ = 0;
	maxHeight_ = 0;
	for(auto c : plDB_->cells())
	{
		if(c->ux() > maxWidth_)
			maxWidth_ = c->ux();
		if(c->uy() > maxHeight_)
			maxHeight_ = c->uy();
	}

	double scaleX = double(MAX_H) / double(maxWidth_);
	double scaleY = double(MAX_H) / double(maxHeight_);

	scale_	= std::min(scaleX, scaleY);
}

void
PlPainter::drawDie()
{
	drawRect(getX(plDB_->die()->lx()), getY(plDB_->die()->ly()), 
	         getX(plDB_->die()->ux()), getY(plDB_->die()->uy()), 
	         gray, black, DIE_LINE_THICKNESS);
}

void
PlPainter::drawRows()
{
	// Not Implemented Yet
}

void
PlPainter::drawNet(Net* net)
{
	int newLx = getX(net->lx());
	int newLy = getY(net->ly());
	int newUx = getX(net->ux());
	int newUy = getY(net->uy());

	drawLine(newLx, newLy, newUx, newLy, black);
	drawLine(newUx, newLy, newUx, newUy, black);
	drawLine(newUx, newUy, newLx, newUy, black);
	drawLine(newLx, newUy, newLx, newLy, black);
}

void
PlPainter::drawNets()
{
	printf("[GUI] Drawing Nets.\n");
	for(auto &c : plDB_->nets())
		drawNet(c);
}

void
PlPainter::drawCell(Cell* cell)
{
	int newLx = getX(cell->lx());
	int newLy = getY(cell->ly());
	int newUx = getX(cell->ux());
	int newUy = getY(cell->uy());

	if(cell->isMacro())
		drawRect(newLx, newLy, newUx, newUy, MACRO_COLOR, 1);
	else
		drawRect(newLx, newLy, newUx, newUy, STD_CELL_COLOR, STD_CELL_LINE_COLOR, 0);
}

void
PlPainter::drawCells()
{
	printf("[GUI] Drawing Cells.\n");
	for(auto &c : plDB_->cells())
		drawCell(c);
}

void
PlPainter::drawChip()
{
	drawDie();
	drawCells();
	//drawNets();
	show();
}


// BookShelf Painter //
BsPainter::BsPainter(std::shared_ptr<BookShelf::BookShelfDB> bsDB)
{
	bookShelfDB_ = bsDB;

	maxWidth_ = 0;
	maxHeight_ = 0;
	for(auto c : bookShelfDB_->cellVector())
	{
		if(c->ux() > maxWidth_)
			maxWidth_ = c->ux();
		if(c->uy() > maxHeight_)
			maxHeight_ = c->uy();
	}

	double scaleX = double(MAX_H) / double(maxWidth_);
	double scaleY = double(MAX_H) / double(maxHeight_);

	scale_	= std::min(scaleX, scaleY);
}

void
BsPainter::drawDie()
{
	drawRect(getX(bookShelfDB_->getDie()->lx()), getY(bookShelfDB_->getDie()->ly()), 
	         getX(bookShelfDB_->getDie()->ux()), getY(bookShelfDB_->getDie()->uy()), 
	         gray, black, DIE_LINE_THICKNESS);
}

void
BsPainter::drawRow(BsRow* row)
{
	int newLx = getX(row->lx());
	int newLy = getY(row->ly());
	int newUx = getX(row->ux());
	int newUy = getY(row->uy());

	drawRect(newLx, newLy, newUx, newUy, gray, 1);
}

void
BsPainter::drawRows()
{
	printf("[GUI] Drawing Rows.\n");
	for(auto &r : bookShelfDB_->rowVector())
		drawRow(r);
}

void
BsPainter::drawCell(BsCell* cell)
{
	int newLx = getX(cell->lx());
	int newLy = getY(cell->ly());
	int newUx = getX(cell->ux());
	int newUy = getY(cell->uy());

	if(cell->isTerminal())
		drawRect(newLx, newLy, newUx, newUy, MACRO_COLOR, 1);
	else
		drawRect(newLx, newLy, newUx, newUy, STD_CELL_COLOR, black, 0);
}

void
BsPainter::drawCells()
{
	printf("[GUI] Drawing Cells.\n");
	for(auto &c : bookShelfDB_->cellVector())
		drawCell(c);
}

void
BsPainter::drawChip()
{
	drawDie();
	//drawRows();
	drawCells();
	show();
}

} // namespace Painter
