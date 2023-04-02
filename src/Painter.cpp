#include "Painter.h"
#include "PlacerDB.h"
#include "BookShelfDB.h"
#include <stdio.h>
#include "CImg.h"

// Not a real size
// MAX_W, MAX_H is just a imaginary size
#define MAX_W 8000
#define MAX_H 8000

#define WINDOW_W 1600
#define WINDOW_H 1600

#define ZOOM_SPEED 300
#define MOVE_SPEED 300

#define DIE_OFFSET_X 20
#define DIE_OFFSET_Y 20

#define DIE_OPACITY      1.0
#define MACRO_OPACITY    0.7
#define STD_CELL_OPACITY 0.7

#define DIE_LINE_THICKNESS      1
#define MACRO_LINE_THICKNESS    1
#define STD_CELL_LINE_THICKNESS 0

namespace Painter
{

using namespace BookShelf;
using namespace PlacerBase;
using namespace cimg_library;

static const Color DIE_COLOR             = gray;
static const Color MACRO_COLOR           = aqua;
static const Color STD_CELL_COLOR        = red;

static const Color NET_LINE_COLOR        = blue;
static const Color DIE_LINE_COLOR        = black;
static const Color MACRO_LINE_COLOR      = black;
static const Color STD_CELL_LINE_COLOR   = black;

// Painter Interface //
Painter::Painter() 
{
	printf("[GUI] Constructing a Painter.\n");

	offsetX_ = DIE_OFFSET_X;
	offsetY_ = DIE_OFFSET_Y;

	canvasX_ = MAX_W + 2 * DIE_OFFSET_X;
	canvasY_ = MAX_H + 2 * DIE_OFFSET_Y;

	// canvas is just a background image for placement visualization
	canvas_ = new CImg<unsigned char>(canvasX_, canvasY_, 1, 3, 255);
	canvas_->draw_rectangle(0, 0, canvasX_, canvasY_, white);

	// img_ := Original image which represents the whole placement
	// any 'zoomed' image will use a crop of this img_
	img_    = new CImg<unsigned char>(*canvas_);
	//window_ = new CImgDisplay(canvasX_, canvasY_, "Placement GUI");
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
	drawRect(lx, ly, ux, uy, rect_c, black, w, 1.0);
}

void
Painter::drawRect(int lx, int ly, int ux, int uy, Color rect_c,
                  Color line_c, int w, float opacity)
{
	img_->draw_rectangle(lx, ly, ux, uy, rect_c, opacity);
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
			         ux + xd * i, uy + yd * i, line_c);

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
	int viewX = 0;
	int viewY = 0;

	int ZoomBoxW  = MAX_H; 
	int ZoomBoxH  = MAX_W; 

	bool redraw = false;

	int lx = 0;
	int ly = 0;

	CImg<unsigned char> ZoomBox 
		= img_->get_crop(lx, ly, lx + canvasX_, ly + canvasY_);

	window_ = new CImgDisplay(WINDOW_W, WINDOW_H, "Placement GUI");

	// Interactive Mode //
	while(!window_->is_closed() && !window_->is_keyESC()
			                        && !window_->is_keyQ())
	{
		if(redraw)
		{
			ZoomBox 
				= img_->get_crop(lx, ly, lx + ZoomBoxW, ly + ZoomBoxH);
			ZoomBox.resize(*window_);
			redraw = false;
		}

		//ZoomBox.resize(ZoomBoxW, ZoomBoxH);
		ZoomBox.display(*window_);

		if(window_->key())
		{
			switch(window_->key())
			{
				case cimg::keyARROWUP:
					if(check_inside(lx, ly - MOVE_SPEED, ZoomBoxW, ZoomBoxH))
					{
						ly -= MOVE_SPEED;
						redraw = true;
					}
					break;
				case cimg::keyARROWDOWN:
					if(check_inside(lx, ly + MOVE_SPEED, ZoomBoxW, ZoomBoxH))
					{
						ly += MOVE_SPEED;
						redraw = true;
					}
					break;
				case cimg::keyARROWLEFT:
					if(check_inside(lx - MOVE_SPEED, ly, ZoomBoxW, ZoomBoxH))
					{
						lx -= MOVE_SPEED;
						redraw = true;
					}
					break;
				case cimg::keyARROWRIGHT:
					if(check_inside(lx + MOVE_SPEED, ly, ZoomBoxW, ZoomBoxH))
					{
						lx += MOVE_SPEED;
						redraw = true;
					}
					break;

				case cimg::keyZ:
					if(ZoomBoxW > ZOOM_SPEED 
					&& ZoomBoxH > ZOOM_SPEED)
					{
						redraw = true;
						ZoomBoxW -= ZOOM_SPEED;
						ZoomBoxH -= ZOOM_SPEED;
					}
					break;
				case cimg::keyX:
					if(ZoomBoxW <= canvasX_ - ZOOM_SPEED 
				  && ZoomBoxH <= canvasY_ - ZOOM_SPEED)
					{
						redraw = true;
						ZoomBoxW += ZOOM_SPEED;
						ZoomBoxH += ZOOM_SPEED;
					}
					break;
			}
			window_->set_key(); // Flush all key events...
		}

		window_->wait();
	}
	exit(0);
}

bool 
Painter::check_inside(int lx, int ly, int w, int h)
{
	if(lx < 0)            return false;
	if(ly < 0)            return false;
	if(lx + w > canvasX_) return false;
	if(ly + h > canvasY_) return false;
	return true;
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
	         gray, DIE_LINE_COLOR, DIE_LINE_THICKNESS, DIE_OPACITY);
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

	drawLine(newLx, newLy, newUx, newLy, NET_LINE_COLOR);
	drawLine(newUx, newLy, newUx, newUy, NET_LINE_COLOR);
	drawLine(newUx, newUy, newLx, newUy, NET_LINE_COLOR);
	drawLine(newLx, newUy, newLx, newLy, NET_LINE_COLOR);
}

void
PlPainter::drawNets()
{
	printf("[GUI] Drawing Nets.\n");
	int i = 0;

	int cellH = 0;
	for(auto &c : plDB_->cells())
	{
		if(!c->isMacro())
		{
			cellH = c->dy();
			break;
		}
	}

	for(auto &n : plDB_->nets())
	{
		// For readability
		// Too Large BBoxs make it difficult to
		// recognize other BBoxs
		if(n->dy() > 5 * cellH) continue;
		if(n->dx() > 5 * cellH) continue;
		if(i++ % 3 == 0) drawNet(n);
	}
}

void
PlPainter::drawCell(Cell* cell)
{
	int newLx = getX(cell->lx());
	int newLy = getY(cell->ly());
	int newUx = getX(cell->ux());
	int newUy = getY(cell->uy());

	if(cell->isMacro())
	{
		drawRect(newLx, newLy, newUx, newUy, MACRO_COLOR, 
		                                     MACRO_LINE_COLOR,
		                                     MACRO_LINE_THICKNESS, 
																				 MACRO_OPACITY);
	}
	else
	{
		drawRect(newLx, newLy, newUx, newUy, STD_CELL_COLOR, 
				                                 STD_CELL_LINE_COLOR, 
				                                 STD_CELL_LINE_THICKNESS,
																				 STD_CELL_OPACITY);
	}
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
	         DIE_COLOR, DIE_LINE_COLOR, DIE_LINE_THICKNESS, DIE_OPACITY);
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
		drawRect(newLx, newLy, newUx, newUy, STD_CELL_COLOR, 0);
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
