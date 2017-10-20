#ifndef _ISOGRID_H_
#define _ISOGRID_H_

#include "color.h"
#include "component.h"
#include "isometric.h"

const int DEFAULT_TILEX = 32;
const int DEFAULT_TILEY = 32;
const int DEFAULT_TILEZ = 16;

// default tile size in texture space
const int DEFAULT_TILEU = 16;
const int DEFAULT_TILEV = 16;

// default maximum height of a map. Affects origin and clipping rectangle
const int DEFAULT_DIM_MZ = 20;

/*
 * implements coordinate system for a isometric grid system
 *
 * does not implement draw method, does not contain map.
 * but has handy transformation methods and drawing primitives
 */
class IsoGrid
{
protected:
	int dim_mx;
	int dim_my;
	int dim_mz;

	int tilex;
	int tiley;
	int tilez;

	int rx0; // location of origin
	int ry0;

	bool autoOrigin = true;

	/*
	 *
	 * coordinate systems:
	 *
	 *    rx, ry     = screen 2D coordinates
	 *    ox, oy     = rx, ry shifted to origin
	 *    ix, iy, iz = isometric pixel coordinates
	 *    cx, cy, cz / mx, my, mz = isometric grid cell coordinates
	 *    px, py, pz = isometric pixel delta against grid
	 *
	 *
	 *    xs, ys, zs = x-size, y-size, z-size
	 *
	 */

	void calculateOrigin()
	{
		if (autoOrigin)
		{
			rx0 = getw() / 2;
			ry0 = dim_mz * tilez;
		}
	}

public:
	IsoGrid(int sizex, int sizey, int sizez, int tilesizexy, int tilesizez) : dim_mx(sizex), dim_my(sizey), dim_mz(sizez), tilex(tilesizexy), tiley(tilesizexy), tilez(tilesizez)
	{
		calculateOrigin();
	}

	void setOrigin (int rx0Val, int ry0Val)
	{
		rx0 = rx0Val;
		ry0 = ry0Val;
		autoOrigin = false;
	}

	IsoGrid() : dim_mx(0), dim_my(0), dim_mz(0), tilex(DEFAULT_TILEX), tiley(DEFAULT_TILEY), tilez(DEFAULT_TILEZ)
	{
	}

	int getSize_ix() { return dim_mx * tilex; }
	int getSize_iy() { return dim_my * tiley; }
	int getSize_iz() { return dim_mz * tilez; }

	virtual ~IsoGrid() {}

	void isoDrawWireFrame (int rx, int ry, int ixs, int iys, int izs, ALLEGRO_COLOR color)
	{
		drawWireFrame (rx + rx0, ry + ry0, ixs, iys, izs, color);
	}

	/**
	 * given a certain screen coordinate and a isometric z-level,
	 * calculate the isometric x, y
	 *
	 * Because a given point on the screen corresponds to many possible x,y,z points in isometric space,
	 * the caller has to choose the z coordinate (usually, in a loop trying possibilities until a good fit comes up)
	 */
	void screenToIso (float iz, int rx, int ry, float &ix, float &iy)
	{
		int ox = rx - rx0;
		int oy = ry - ry0;

		ix = oy + (ox / 2) + (iz / 2);
		iy = oy - (ox / 2) + (iz / 2);
	}

	int isoToScreenX (float ix, float iy, float iz)
	{
	 	int ox = (int)(ix - iy);
	 	return ox + rx0;
	}

	int isoToScreenY (float ix, float iy, float iz)
	{
		int oy = (int)(ix * 0.5 + iy * 0.5 - iz);
		return oy + ry0;
	}

	/**
	 * Useful for z-ordering sprites
	 */
	int isoToScreenZ (float ix, float iy, float iz)
	{
		return (int)(ix + iy + iz);
	}

	/**
	 * Check that a given grid coordinate is within bounds
	 */
	bool cellInBounds(int cx, int cy, int cz)
	{
		return
				cx >= 0 && cx < dim_mx &&
				cy >= 0 && cy < dim_my &&
				cz >= 0 && cz < dim_mz;
	}


	int getw() const { return (dim_mx + tilex + dim_my * tiley) * 2; }
	int geth() const { return (dim_mx * tilex + dim_my * tiley) / 2 + dim_mz * tilez; }


	/** distance from the cell at 0,0 to the edge of the virtual screen */
	int getXorig () const { return rx0; }

	/** distance from the cell at 0,0 to the edge of the virtual screen */
	int getYorig () const { return ry0; }

	void canvasFromIso_f (float x, float y, float z, float &rx, float &ry) const
	{
		rx = rx0 + (x - y);
		ry = ry0 + (x * 0.5 + y * 0.5 - z);
	}

	void drawSurfaceWire (const GraphicsContext &gc, int mx, int my, int mz, int mxs, int mys, ALLEGRO_COLOR color);

	int getTilex() { return tilex; }
	int getTiley() { return tiley; }

	void setDimension(int mxVal, int myVal, int mzVal)
	{
		dim_mx = mxVal;
		dim_my = myVal;
		dim_mz = mzVal;
		calculateOrigin();
	}

	void setTileSize(int x, int y, int z)
	{
		tilex = x;
		tiley = y;
		tilez = z;
		calculateOrigin();
	}

	/** get dimension of map in isometric pixel units */
	int getDimIX() { return dim_my * tiley; }
	int getDimIY() { return dim_mx * tilex; }

	// assuming z = 0...
	void isoFromCanvas (double rx, double ry, double &x, double &y)
	{
		double ox = rx - rx0;
		double oy = ry - ry0;
		x = oy + ox / 2;
		y = oy - ox / 2;
	}

	int canvasFromMapX (int mx, int my) const
	{
		return rx0 + mx * 32 + my * -32;
	}

	int canvasFromMapY (int mx, int my) const
	{
		return ry0 + mx * 16 + my * 16;
	}

	int canvasFromMapX (float mx, float my) const
	{
		return (int)(rx0 + mx * 32.0 + my * -32.0);
	}

	int canvasFromMapY (float mx, float my) const
	{
		return (int)(ry0 + mx * 16.0 + my * 16.0);
	}

	int mapFromCanvasX (int x, int y) const
	{
		return ((x - rx0) / 2 + (y - ry0)) / 32;
	}

	int mapFromCanvasY (int x, int y) const
	{
		return  (y - ry0 - (x - rx0) / 2) / 32;
	}

};

#endif
