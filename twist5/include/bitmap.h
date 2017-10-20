/*
 * bitmap.h
 */

#ifndef BITMAP_H
#define BITMAP_H

#include <memory>

struct ALLEGRO_BITMAP;

std::shared_ptr<ALLEGRO_BITMAP> make_shared_bitmap (int w, int h);

#endif /* SRC_BITMAP_H_ */
