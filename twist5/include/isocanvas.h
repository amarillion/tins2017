#ifndef _ISOCANVAS_H_
#define _ISOCANVAS_H_

//#include <MASkinG.h>
#include "isogrid.h"
#include "dialog.h"
#include "color.h"
#include "scrollbox.h"
#include "mainloop.h"

template <class T> class IsoCanvas : public Scrollable
{
protected:
	T *map;
	IsoGrid *grid;

	// std::shared_ptr<Timer> scrollTimer;
	int cursor_x;
	int cursor_y;
	bool fullRedraw;
	Rect cursorRect; // store to mark dirty later
public:
	void setFullRedraw (bool value) { fullRedraw = value; }
	bool getFullRedraw () { return fullRedraw; }

	IsoCanvas() : map(NULL), cursor_x(-1), cursor_y(-1), fullRedraw (true)
	{
	}

	virtual ~IsoCanvas() {}

	void setMap (T *value, IsoGrid *gridVal)
	{
		grid = gridVal;
		map = value;
		if (map)
		{
//			setwh (map->getw() + 64, map->geth()); //TODO: old code used different scale
			setDimension(2048, 1024);
			// TODO: send to parent? Center();
			center();
		}
	}

	void drawCursor (const GraphicsContext &gc)
	{
		if (!map) return;
		if (cursor_x >= 0 && cursor_y >= 0 &&
			cursor_x < map->getDimMX() && cursor_y < map->getDimMY())
		{
			int x = grid->canvasFromMapX (cursor_x, cursor_y) + gc.xofst;
			int y = grid->canvasFromMapY (cursor_x, cursor_y) + gc.yofst;

			al_draw_line (x + 31, y, 		x + 63, y + 15,	YELLOW, 1.0);
			al_draw_line (x + 63, y + 15, 	x + 31, y + 31,	YELLOW, 1.0);
			al_draw_line (x + 31, y + 31,	x, y + 15,		YELLOW, 1.0);
			al_draw_line (x, y + 15,		x + 31, y, 		YELLOW, 1.0);

			cursorRect = Rect (x, y, 66, 34); // store for marking dirty later
		}
	}

	virtual void MsgMousemove(const Point &d) override
	{
		int qx = d.x();
		int qy = d.y();

		cursor_x = grid->mapFromCanvasX (qx, qy);
		cursor_y = grid->mapFromCanvasY (qx, qy);

		/*
		 TODO: marking dirty probably not necessary
		if (fullRedraw)
		{
			Redraw();
		}
		else
		{
			Redraw (cursorRect); // remove old rect
			Redraw (Rect (d.x() - 64, d.y() - 32, 128, 64));
		}
		 */
	}

};

#endif /* ISOCANVAS_H_ */
