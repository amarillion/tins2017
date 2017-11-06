#include "sprite.h"
#include "engine.h"
#include "mainloop.h"

using namespace std;

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

void Sprite::setState(int value)
{
	state = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
}

void Sprite::setDir(int value)
{
	dir = value; animStart = MainLoop::getMainLoop()->getMsecCounter();
}

void SpriteGroup::move(const std::shared_ptr<Sprite> &target, float destx, float desty, int steps) {
	auto animator = make_shared <MoveAnimator>(target, destx, desty, steps);
	sprites.push_back(animator);
}
