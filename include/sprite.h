#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "graphicscontext.h"
#include <list>
#include <memory>
#include <functional>
#include "point.h"
#include "updateable.h"

struct ALLEGRO_BITMAP;
struct ALLEGRO_FONT;
class Anim;

class Sprite : public Updateable
{
protected:
	// will either draw anim, if defined, or else sprite, if defined.
	Anim *anim = nullptr;
	ALLEGRO_BITMAP *sprite = nullptr;
	int state = 0;
	int dir = 0;
	int animStart = 0;

	float x; // cell-x
	float y;

	int w;
	int h;

	bool visible;
	bool awake;

public:
	bool isVisible() { return visible; }
	void setAwake(bool value) { awake = value; }
	bool isAwake() { return awake; }
	void setVisible (bool value) { visible = value; }

	void setState(int value);
	void setDir(int value);
	void setAnim(Anim *value);
	void setBitmap(ALLEGRO_BITMAP *img);

	virtual void draw(const GraphicsContext &gc);

	virtual void update() override {};
	float getx() { return x; }
	float gety() { return y; }
	void setxy (int _x, int _y) { x = _x; y = _y; }
	void setx(int _x) { x = _x; }
	void sety(int _y) { y = _y; }
	Sprite();
	virtual ~Sprite() {}
	virtual void handleAnimationComplete() {};
};

/** grouping of sprites
 *
 * Note that sprites may always be members of multiple groups.
 *
 * SpriteGroups may be used for
 * - drawing
 * - updating
 * - z-ranking
 * collections of sprites
 *
 * But also for laying them out together, building a hierarchy of sprites,
 * or simply remembering which group of sprites belong together
 */
class SpriteGroup : public Sprite {
private:
	std::list<std::shared_ptr<Sprite>> sprites;

public:

	void push_back(const std::shared_ptr<Sprite> &val) {
		sprites.push_back(val);
	}

	void push_front(const std::shared_ptr<Sprite> &val) {
		sprites.push_front(val);
	}

	void killAll() {
		for (auto sp : sprites) {
			sp->kill();
		}
		sprites.clear();
	}

	virtual void update() {
		if (!awake) return;

		for (auto i : sprites)
		{
			if (i->isAlive() && i->isAwake()) i->update();
		}

		sprites.remove_if ( [](std::shared_ptr<Sprite> i) { return !(i->isAlive()); });
	}

	virtual void draw (const GraphicsContext &gc) {
		for (auto i : sprites)
		{
			if (i->isAlive() && i->isVisible()) i->draw(gc);
		}
	}
};

#endif
