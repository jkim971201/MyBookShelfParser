#include "Painter.h"
#include "BookShelfDB.h"
#include <stdio.h>
#include "CImg.h"

#define MAX_W 3000
#define MAX_H 1800

#define DIE_OFFSET_X 20
#define DIE_OFFSET_Y 20

#define DIE_LINE_THICKNESS 4

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

  int canvasX = MAX_W + 2 * DIE_OFFSET_X;
  int canvasY = MAX_H + 2 * DIE_OFFSET_Y;

  canvas_ = new CImg<unsigned char>(canvasX, canvasY, 1, 3, 255);
  canvas_->draw_rectangle(0, 0, canvasX, canvasY, white);

  img_ = new CImg<unsigned char>(*canvas_);
  window_ = new CImgDisplay(canvasX, canvasY, "Placement GUI");
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
Painter::drawLine(int x1, int y1, int x2, int y2, Color color, int w)
{
  img_->draw_line(x1, y1, x2, y2, color);
  if(x1 == x2)
  {
    for(int i = 1; i < w+1; i++)
    {
      img_->draw_line(x1+i, y1, x2+i, y2, color);
      img_->draw_line(x1-i, y1, x2-i, y2, color);
    }
  }
  else if(y1 == y2)
  {
    for(int i = 1; i < w+1; i++)
    {
      img_->draw_line(x1, y1+i, x2, y2+i, color);
      img_->draw_line(x1, y1-i, x2, y2-i, color);
    }
  }
}

void
Painter::drawRect(int lx, int ly, int ux, int uy, Color rect_c,
				  Color line_c, int w)
{
  img_->draw_rectangle(lx, ly, ux-lx, uy-ly, rect_c);
  drawLine(lx, ly, ux, ly, line_c, w);
  drawLine(ux, ly, ux, uy, line_c, w);
  drawLine(ux, uy, lx, uy, line_c, w);
  drawLine(lx, uy, lx, ly, line_c, w);
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

  scaleX_ = double(MAX_W) / double(dieWidth_);
  scaleY_ = double(MAX_H) / double(dieHeight_);

  scale_  = std::min(scaleX_, scaleY_);

  printf("[GUI] Scaled Die Width  = %d * %f = %d.\n", dieWidth_ , scale_, int(dieWidth_  * scale_));
  printf("[GUI] Scaled Die Height = %d * %f = %d.\n", dieHeight_, scale_, int(dieHeight_ * scale_));
}

void
BsPainter::drawDie()
{
  printf("[GUI] Drawing a Die.\n");
  drawRect(0, 0, dieWidth_ * scale_, dieHeight_ * scale_, white, black, DIE_LINE_THICKNESS);
}

void
BsPainter::drawRows()
{
  printf("[GUI] Drawing Rows.\n");
}

void
BsPainter::drawCell(BsCell* cell)
{
  if(cell->isTerminal())
  {
    drawRect(cell->lx() * scale_, cell->ly() * scale_,
	  	     cell->ux() * scale_, cell->uy() * scale_, blue);
  }
  else
  {
    drawRect(cell->lx() * scale_, cell->ly() * scale_,
	  	     cell->ux() * scale_, cell->uy() * scale_, red);
  }
}

void
BsPainter::drawCells()
{
  printf("[GUI] Drawing Cells.\n");
  for(auto &c : bookShelfDB_->cellVector())
  {
    drawCell(c);
  }
}

void
BsPainter::drawChip()
{
  drawDie();
  drawRows();
  drawCells();
  show();
}

}
