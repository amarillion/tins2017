#ifndef _CARDRENDERER_H_
#define _CARDRENDERER_H_

#include <allegro5/allegro.h>

/**
 * Drawing helper.
 *
 * Draw a rectangle with outline and fill.
 */
void drawOutlinedRect(int x1, int y1, int x2, int y2, ALLEGRO_COLOR outer, ALLEGRO_COLOR inner, float w);

/**
 * part of initialization.
 *
 * Render cards and add them to Resources.
 */
void renderCards();

#endif
