#ifndef GRAPHICSCONTEXT_H
#define GRAPHICSCONTEXT_H

struct ALLEGRO_BITMAP;

struct GraphicsContext
{
	ALLEGRO_BITMAP *buffer;
	int xofst;
	int yofst;
};

#endif
