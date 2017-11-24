#include "sprite.h"
#include "engine.h"
#include "mainloop.h"
#include <math.h>

using namespace std;

/**
 * Linear movement
 *
 * f(x) = x
 */
double linear(double val) {
	return val;
}

/**
 *
 * Overshoot the goal, then return to it.
 * Based on quadratic curve
 *
 * f(x) = ax2 + bx + c
 * f(0) = 0
 * f(1) = 1
 * f(0.75) = 1.25
 *
 * c = 0
 * a + b = 1
 * b = 1 - a
 *
 * a * -0.1875 = 0.5
 *
 * a = -2.667
 * b = 3.667
 */
double overshoot(double val) {
	return (val * val * -2.667) + (val * 3.667);
}

/**
 * Sigmoid curve
 * Makes the object move slow-fast-slow
 *
 * Using logistic function:
 * f(x) = L / (1 + exp(-k * (val - x0))
 * L -> maximum  = 1
 * k -> steepness = 10.0
 * x0 -> x-value of mid-point = 0.5
 * f(0) = ~0.01;
 * f(1) = ~0.99
 * f(0.5) = 0.5
 *
 */
double sigmoid(double val) {
	return 1.0 / (1.0 + exp(-10.0 * (val - 0.5)));
}

Sprite::Sprite() : x(0), y(0), w(16), h(16), alive(true), visible(true), awake(true) {
	font = Engine::getResources()->getFont("builtin_font");
}

void Sprite::draw(const GraphicsContext &gc) {

	int msec = MainLoop::getMainLoop()->getMsecCounter();
	if (anim) {
		anim->drawFrame (state, dir, msec - animStart, x, y);
	}
	else if (sprite) {
		al_draw_bitmap(sprite, x + gc.xofst, y + gc.yofst, 0);
	}
};


void Sprite::setAnim(Anim *value) {
	anim = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
}

void Sprite::setBitmap(ALLEGRO_BITMAP *value) {
	sprite = value;
}

void Sprite::setState(int value)
{
	state = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
}

void Sprite::setDir(int value)
{
	dir = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
}

void SpriteGroup::move(const std::shared_ptr<Sprite> &target, float destx, float desty, int steps) {
	auto animator = make_shared <MoveAnimator<linear> >(target, destx, desty, steps);
	sprites.push_back(animator);
}
