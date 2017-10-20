#include "isomap.h"

void drawMap(const GraphicsContext &gc, IsoMap *map)
{
	for (size_t mx = 0; mx < map->getDimMX(); ++mx)
		for (size_t my = 0; my < map->getDimMY(); ++my)
		{
			Cell &c = map->get(mx, my);

			map->drawSurface (gc, mx, my, c);
			map->drawLeftWall (gc, mx, my, c);
			map->drawRightWall (gc, mx, my, c);
		}

}

void IsoMap::drawSurface(const GraphicsContext &gc, int mx, int my, Cell &c)
{
	al_set_target_bitmap (gc.buffer);

	ALLEGRO_VERTEX coord[6]; // hold memory for coordinates

	memset(&coord, 0, sizeof(ALLEGRO_VERTEX) * 6);

	// ALLEGRO_COLOR baseColor = al_map_rgb (192, 255, 192);
	ALLEGRO_COLOR baseColor = al_map_rgb (255, 255, 255);

	// TODO hard coded tile indices for use in krampushack for now...
	int tilei = (c.z + c.dzbot > 2 || c.z > 2 || c.z + c.dzright > 2 || c.z + c.dzleft > 2) ? 2 : 1;

	int ubase = tileu * tilei;
	int vbase = 0;

	canvasFromIso_f (tilex * mx,
					tiley * my,
					tilez * c.z,
					coord[0].x, coord[0].y);
	coord[0].u = ubase;
	coord[0].v = vbase;

	canvasFromIso_f (	tilex * (mx + 1),
					tiley * my,
					tilez * (c.z + c.dzright),
					coord[1].x, coord[1].y);
	coord[1].u = ubase + tileu;
	coord[1].v = vbase;

	canvasFromIso_f (	tilex * mx,
					tiley * (my + 1),
					tilez * (c.z + c.dzleft),
					coord[5].x, coord[5].y);
	coord[5].u = ubase;
	coord[5].v = vbase + tilev;

	canvasFromIso_f (	tilex * (mx + 1),
					tiley * (my + 1),
					tilez * (c.z + c.dzbot),
					coord[3].x, coord[3].y);

	coord[3].u = ubase + tileu;
	coord[3].v = vbase + tilev;

	ALLEGRO_COLOR color1, color2;

/*
*
*
*    y   A
*   /   / \   x
*  +   /   \   \
*     C     B   +
*      \   /
*       \ /
*        D
*
*
*   Coordinate array
*
*   0 1 2   3 4 5
*   A B -   D - C
* - A B D   D A C   ->  vertical split
* - A B C   D B C   ->  horizontal split
*/
	if (c.isVerticalSplit())
	{
		memcpy (&coord[4], &coord[0], sizeof(ALLEGRO_VERTEX));
		memcpy (&coord[2], &coord[3], sizeof(ALLEGRO_VERTEX));

		/*
		 *
		 *
		 *    y   A
		 *   /   /|\   x
		 *  +   / | \   \
		 *     C  |  B   +
		 *      \ | /
		 *       \|/
		 *        D
		 *
		*/
		// lighting for A-B-D
		color1 = litColor (baseColor,
					surfaceLighting (-1, 0, -c.dzright, 0, 1, c.dzbot - c.dzright) );
		// lighting for A-D-C
		color2 = litColor (baseColor,
						surfaceLighting (0, -1, -c.dzleft, 1, 0, c.dzbot - c.dzleft) );
	}
	else
	{
		/*
		 *
		 *
		 *    y   A
		 *   /   / \   x
		 *  +   /   \   \
		 *     C-----B   +
		 *      \   /
		 *       \ /
		 *        D
		 *
		*/

		memcpy (&coord[4], &coord[1], sizeof(ALLEGRO_VERTEX));
		memcpy (&coord[2], &coord[5], sizeof(ALLEGRO_VERTEX));
		// lighting for A-B-C
		color1 = litColor (baseColor,
						surfaceLighting (1, 0, c.dzright, 0, 1, c.dzleft) );

		// lighting for C-B-D
		color2 = litColor (baseColor,
					surfaceLighting (0, -1, c.dzright - c.dzbot, -1, 0, c.dzleft - c.dzbot) );
	}

	for (int i = 0; i < 6; ++i)
	{
		coord[i].x += gc.xofst;
		coord[i].y += gc.yofst;
	}


	for (int i = 0; i < 3; ++i)
	{
		coord[i].color = color1;
	}


	for (int i = 3; i < 6; ++i)
	{
		coord[i].color = color2;
	}

	al_draw_prim(coord, NULL, tiles, 0, 6, ALLEGRO_PRIM_TRIANGLE_LIST);

	/*
	// debugging help for interpolation
	for (int xx = 0; xx < 8; xx ++)
	{
		for (int yy = 0; yy < 8; yy ++)
		{
			float jx = (mx * tilex) + (xx * tilex / 8);
			float jy = (my * tiley) + (yy * tiley / 8);
			float jz = getSurfaceIsoz(jx, jy);
			float rx, ry;
			canvasFromIso_f(jx, jy, jz, rx, ry);

			al_draw_filled_circle(rx + gc.xofst, ry + gc.yofst, 2.0, MAGENTA);
		}
	}
	*/

}

// get the iso z coordinate in pixels, at a given isometric x, y coordinate
double IsoMap::getSurfaceIsoz(double ix, double iy)
{
	// cell
	int mx = ix / tilex;
	int remx = ix - (mx * tilex);

	int my = iy / tiley;
	int remy = iy - (my * tiley);

	if (!inBounds(mx, my)) return -std::numeric_limits<double>::infinity();

	Cell &c = get(mx, my);

	double result = 0;
	// interpolate

	if (c.isVerticalSplit())
	{
		// NOTE: this comparison assumes tilex == tiley
		if (remx < remy)
		{
			// left half
			result = c.z * tilez;
			result += remx * (- c.dzleft + c.dzbot) * tilez / tilex;
			result += remy * c.dzleft * tilez / tiley;
		}
		else
		{
			// right half
			result = c.z * tilez;
			result += remx * c.dzright * tilez / tilex;
			result += remy * (- c.dzright + c.dzbot) * tilez / tiley;
		}
	}
	else
	{
		// NOTE: this comparison assumes tilex == tiley
		if (remx + remy < tilex)
		{
			// top half
			result = c.z * tilez;
			result += remx * c.dzright * tilez / tilex;
			result += remy * c.dzleft * tilez / tiley;
		}
		else
		{
			// bottom half
			result = (c.z + c.dzbot) * tilez;
			result += (tilex - remx) * (- c.dzbot + c.dzleft) * tilez / tilex;
			result += (tiley - remy) * (- c.dzbot + c.dzright) * tilez / tiley;
		}
	}

	return result;
}

