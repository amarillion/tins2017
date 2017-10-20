#ifndef _ISOMAP_H
#define _ISOMAP_H

#include "isogrid.h"
#include <allegro5/allegro_primitives.h>
#include "map2d.h"
#include "isogrid.h"

/**
 * Draw a surface.
 *
 * z is the height, in tile units, of the top corner.
 *
 * dzleft, dzright and dzbot are the z-delta, in tile units, of the left,
 * right and bottom corners
 */
struct Cell
{
	int z;
	short dzleft;
	short dzright;
	short dzbot;
	Cell() { z = 0; dzleft= 0; dzright = 0; dzbot = 0; }


	bool isVerticalSplit()
	{
		return dzbot == 0;
	}

	// lift corner of a single tile
	void liftCorner(int delta, int side)
	{
		switch (side)
		{
		case 0:
			z += delta;
			dzleft -= delta;
			dzright -= delta;
			dzbot -= delta;
			break;
		case 1:
			dzright += delta;
			break;
		case 2:
			dzbot += delta;
			break;
		case 3:
			dzleft += delta;
			break;
		default:
			assert (false);
			break;
		}
	}

	void setCorner(int value, int side)
	{
		int delta = value - z;
		switch (side)
		{
		case 0:
			z += delta;
			dzleft -= delta;
			dzright -= delta;
			dzbot -= delta;
			break;
		case 1:
			dzright = delta;
			break;
		case 2:
			dzbot = delta;
			break;
		case 3:
			dzleft = delta;
			break;
		default:
			assert (false);
			break;
		}
	}

	// return the z of the highest corner.
	int getMaxHeight()
	{
		return std::max(z, std::max (z + dzbot, std::max (z + dzleft, z + dzright)));
	}

	// return the z of the lowest corner
	int getMinHeight()
	{
		return std::min(z, std::min (z + dzbot, std::min (z + dzleft, z + dzright)));
	}

	bool isFlat()
	{
		return (dzbot == 0 && dzleft == 0 && dzright == 0);
	}

};

class IsoMap : public Map2D<Cell>, public IsoGrid
{
protected:

	// maximum height
	int sizez = 20;

	// dimensions of a texture tile
	int tileu = DEFAULT_TILEU;
	int tilev = DEFAULT_TILEV;
public:

	IsoMap() : Map2D<Cell>(), IsoGrid(0, 0, 0, DEFAULT_TILEX, DEFAULT_TILEZ) {}
	IsoMap(int w, int h, int tilexVal = DEFAULT_TILEX, int tileyVal = DEFAULT_TILEY, int tilezVal = DEFAULT_TILEZ) :
		Map2D<Cell>(w, h), IsoGrid(w, h, DEFAULT_DIM_MZ, tilexVal, tilezVal) {}

	void setTileTextureSize(int u, int v)
	{
		tileu = u;
		tilev = v;
	}

	ALLEGRO_BITMAP *tiles = NULL;

	void drawSurface(const GraphicsContext &gc, int mx, int my, Cell &c);

	void setPointAtLeast(int mx, int my, int value)
	{
		if (!inBounds(mx, my)) return;

		if (get(mx, my).z >= value) return;
		setPoint (mx, my, value);
	}

	void setPoint(int mx, int my, int value)
	{
		if (!inBounds(mx, my)) return;

		get(mx    ,     my).setCorner(value, 0);

		if (mx >= 1)
		{
			get(mx - 1,     my).setCorner(value, 1);
		}

		if (my >= 1 && mx >= 1)
		{
			get(mx - 1, my - 1).setCorner(value, 2);
		}

		if (my >= 1)
		{
			get(mx    , my - 1).setCorner(value, 3);
		}
	}

	// recursive version of setPointAtLeast
	void setHillAtLeast(int mx, int my, int value)
	{
		if (!inBounds(mx, my)) return;

		if (get(mx, my).z >= value) return;
		setPointAtLeast (mx, my, value);

		if (value > 0)
		{
			setHillAtLeast (mx + 1, my, value - 1);
			setHillAtLeast (mx, my + 1, value - 1);
			setHillAtLeast (mx - 1, my, value - 1);
			setHillAtLeast (mx, my - 1, value - 1);
		}
	}


	// lift all four corners around a point
	void raisePoint(int mx, int my, int delta)
	{
		get(mx    ,     my).liftCorner(delta, 0);
		get(mx - 1,     my).liftCorner(delta, 1);
		get(mx - 1, my - 1).liftCorner(delta, 2);
		get(mx    , my - 1).liftCorner(delta, 3);
	}

	void drawLeftWall(const GraphicsContext &gc, int mx, int my, Cell &c)
	{
		int x[4];
		int y[4];
		int z[4];

		x[0] = tilex * (mx + 1);
		y[0] = tiley * (my + 1);
		z[0] = 0;

		x[1] = tilex * mx;
		y[1] = tiley * (my + 1);
		z[1] = 0;

		x[2] = tilex * mx;
		y[2] = tiley * (my + 1);
		z[2] = tilez * (c.z + c.dzleft);

		x[3] = tilex * (mx + 1);
		y[3] = tiley * (my + 1);
		z[3] = tilez * (c.z + c.dzbot);

		ALLEGRO_COLOR color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 1, 0, 0, 0, 1 ));

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawRightWall(const GraphicsContext &gc, int mx, int my, Cell &c)
	{
		int x[4];
		int y[4];
		int z[4];

		x[0] = tilex * (mx + 1);
		y[0] = tiley * my;
		z[0] = 0;

		x[1] = tilex * (mx + 1);
		y[1] = tiley * (my + 1);
		z[1] = 0;

		x[2] = tilex * (mx + 1);
		y[2] = tiley * (my + 1);
		z[2] = tilez * (c.z + c.dzbot);

		x[3] = tilex * (mx + 1);
		y[3] = tiley * my;
		z[3] = tilez * (c.z + c.dzright);

		ALLEGRO_COLOR color = litColor (al_map_rgb (192, 192, 192),
					surfaceLighting (0, 0, 1, -1, 0, 0) );

		drawIsoPoly(gc, 4, x, y, z, color);
	}

	void drawIsoPoly (const GraphicsContext &gc, int num, int x[], int y[], int z[], ALLEGRO_COLOR color)
	{
		const int BUF_SIZE = 20; // max 20 points
		assert (num <= BUF_SIZE);

		ALLEGRO_VERTEX coord[BUF_SIZE]; // hold actual objects
		ALLEGRO_VERTEX *pcoord[BUF_SIZE]; // hold array of pointers

		// initialize pointers to point to objects
		for (int i = 0; i < BUF_SIZE; ++i) { pcoord[i] = &coord[i]; }

		for (int i = 0; i < num; ++i)
		{
			canvasFromIso_f (x[i], y[i], z[i], coord[i].x, coord[i].y);
			coord[i].x += gc.xofst;
			coord[i].y += gc.yofst;
			coord[i].color = color;
		}

		al_set_target_bitmap (gc.buffer);
		al_draw_prim (coord, NULL, NULL, 0, num, ALLEGRO_PRIM_TRIANGLE_FAN);
	}

	double getSurfaceIsoz(double ix, double iy);

};


void drawMap(const GraphicsContext &gc, IsoMap *map);

#endif
