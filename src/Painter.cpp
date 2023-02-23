#include "Painter.h"
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
using namespace cimg_library;

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
  double tempY = static_cast<double>(dieHeight_ - dbY);
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

// BookShelf Painter //
BsPainter::BsPainter(std::shared_ptr<BookShelf::BookShelfDB> bsDB)
{
  bookShelfDB_ = bsDB;

  dieWidth_  = bookShelfDB_->getDieWidth();
  dieHeight_ = bookShelfDB_->getDieHeight();

  scaleX_ = double(MAX_H) / double(dieWidth_);
  scaleY_ = double(MAX_H) / double(dieHeight_);

  scale_  = std::min(scaleX_, scaleY_);

  printf("[GUI] Scaled Die Width  = %d * %f = %d.\n", dieWidth_ , scale_, int(dieWidth_  * scale_));
  printf("[GUI] Scaled Die Height = %d * %f = %d.\n", dieHeight_, scale_, int(dieHeight_ * scale_));
}

void
BsPainter::drawDie()
{
  printf("[GUI] Drawing a Die (%d, %d)-(%d, %d).\n", 
				          getX(0)        , getY(0)         , 
						  getX(dieWidth_), getY(dieHeight_) );
  drawRect(getX(bookShelfDB_->getDie()->lx()), getY(bookShelfDB_->getDie()->lx()), 
		   getX(bookShelfDB_->getDie()->ux()), getY(bookShelfDB_->getDie()->ux()), 
		   gray, black, DIE_LINE_THICKNESS);
}

void
BsPainter::drawRows()
{
  printf("[GUI] Drawing Rows.\n");
}

void
BsPainter::drawCell(BsCell* cell)
{
  int newLx = getX(cell->lx());
  int newLy = getY(cell->ly());
  int newUx = getX(cell->ux());
  int newUy = getY(cell->uy());
	
  if(cell->isTerminal())
  {
    drawRect(newLx, newLy, newUx, newUy, aqua, 1);
  }
  else
  {
    drawRect(newLx, newLy, newUx, newUy, red);
  }
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
  drawCells();
  show();
}

}
