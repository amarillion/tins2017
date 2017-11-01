#include "sprite.h"
#include "engine.h"

using namespace std;

Sprite::Sprite() : sprite(nullptr), x(0), y(0), w(16), h(16), alive(true), visible(true), awake(true) {
	font = Engine::getResources()->getFont("builtin_font");
}

void Sprite::draw(const GraphicsContext &gc) {
	if (sprite) {
		al_draw_bitmap(sprite, x + gc.xofst, y + gc.yofst, 0);
	}
};

void SpriteGroup::move(const std::shared_ptr<Sprite> &target, float destx, float desty, int steps) {
	auto animator = make_shared <MoveAnimator>(target, destx, desty, steps);
	sprites.push_back(animator);
}

