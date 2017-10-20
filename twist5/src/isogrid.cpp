#include "isogrid.h"
#include <allegro5/allegro_primitives.h>

void IsoGrid::drawSurfaceWire (const GraphicsContext &gc, int mx, int my, int mz, int mxs, int mys, ALLEGRO_COLOR color)
{
	float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;

	canvasFromIso_f (tilex * mx,
					tiley * my,
					tilez * mz,
					rx1, ry1);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my),
					tilez * mz,
					rx3, ry3);

	canvasFromIso_f (tilex * (mx),
					tiley * (my + mys),
					tilez * mz,
					rx4, ry4);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my + mys),
					tilez * mz,
					rx2, ry2);

	al_draw_line (rx1, ry1, 		rx3, ry3,	color, 1.0);
	al_draw_line (rx3, ry3, 		rx2, ry2,	color, 1.0);
	al_draw_line (rx2, ry2,			rx4, ry4,	color, 1.0);
	al_draw_line (rx4, ry4,			rx1, ry1,	color, 1.0);
}
