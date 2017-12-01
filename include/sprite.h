#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "graphicscontext.h"
#include <list>
#include <memory>
#include <functional>

struct ALLEGRO_BITMAP;
struct ALLEGRO_FONT;
class Anim;

class Sprite
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

	bool alive;
	bool visible;
	bool awake;

public:
	bool isAlive() { return alive; }
	void kill() { alive = false; /* scheduled to be removed at next update from any list that is updated */ }
	bool isVisible() { return visible; }
	void setAwake(bool value) { awake = value; }
	bool isAwake() { return awake; }
	void setVisible (bool value) { visible = value; }

	void setState(int value);
	void setDir(int value);
	void setAnim(Anim *value);
	void setBitmap(ALLEGRO_BITMAP *img);

	virtual void draw(const GraphicsContext &gc);

	virtual void update() {};
	float getx() { return x; }
	float gety() { return y; }
	void setxy (int _x, int _y) { x = _x; y = _y; }
	void setx(int _x) { x = _x; }
	void sety(int _y) { y = _y; }
	Sprite();
	virtual ~Sprite() {}
	virtual void handleAnimationComplete() {};
};

/*
 * Easing func: translates time component between 0 (start) and 1 (end) to a
 * value between 0 (start) and 1 (end). The result may overshoot and undershoot,
 * i.e. generate values outside the range 0 and 1, as long as the end points
 * are fixed, i.e. func(0) -> 0 and func(1) -> 1.
 */
typedef double (*EasingFunc)(double);

double linear(double val);
double overshoot(double val);
double sigmoid(double val);

template <EasingFunc F>
class MoveAnimator : public Sprite {

	float destX, destY;
	float srcX, srcY;
	std::shared_ptr<Sprite> target;
	int totalSteps;
	int currentStep;
public:
	MoveAnimator (const std::shared_ptr<Sprite> &target, float destX, float destY, int steps) :
		destX(destX), destY(destY), target(target), totalSteps(steps), currentStep(0) {
		srcX = target->getx();
		srcY = target->gety();
		visible = false;
	}

	virtual void update() {
		if (!target) return;

		// interpolate...
		double delta = (double)currentStep / (double)totalSteps;

		// apply easing func
		double ease = F(delta);
		double newx = srcX + ease * (destX - srcX);
		double newy = srcY + ease * (destY - srcY);
		target->setxy(newx, newy);

		currentStep++;

		if (currentStep > totalSteps) {
			target->handleAnimationComplete();
			kill();
			target = nullptr;
		}
	}
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

	void move(const std::shared_ptr<Sprite> &target, float destx, float desty, int steps);

};

#endif
