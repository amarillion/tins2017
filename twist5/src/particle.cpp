#include <iostream>
#include <allegro5/allegro_primitives.h>
#include "util.h"
#include "color.h"
#include "particle.h"

using namespace std;

void clearGenerateFunction (list<Particle> &particles)
{
	return; // do nothing
}

void clearUpdateFunction (Particle *p)
{
	return; // do nothing
}

ParticleEffectImpl::ParticleEffectImpl() : generateFunction(clearGenerateFunction),
		updateFunction(clearUpdateFunction), drawFunction(clearUpdateFunction) {}

void snowGenerateFunction (list<Particle> &particles, int w)
{
	if (particles.size() < 1000)
	{
		// new ones at top of screen
		Particle particle;
		particle.alive = true;
		particle.x = random(w);
		particle.y = 0;
		particle.color = WHITE;
		particles.push_back(particle);
	}
}

void antigravGenerateFunction (list<Particle> &particles, int w, int h)
{
	if (particles.size() < 500)
	{
		// new ones at bottom of screen
		Particle particle;
		particle.alive = true;
		double scale = ((double)random(100)) / 100.0;
		particle.x = random(w);
		particle.y = h;
		particle.dx = 0;
		particle.dy = -6.0 * (1 + scale);
		particle.color = al_map_rgb_f (0, scale, 0);
		particles.push_back(particle);
	}
}

void confettiGenerateFunction (list<Particle> &particles, int w)
{
	if (particles.size() < 1000)
	{
		// new ones at top of screen
		Particle particle;
		particle.alive = true;
		particle.x = random(w);
		particle.y = 0;
		double r = (double)(random (100)) / 100.0;
		double g = (double)(random (100)) / 100.0;
		double b = (double)(random (100)) / 100.0;
		particle.color = al_map_rgb_f(r, g, b);
		particles.push_back(particle);
	}
}

void rainGenerateFunction (list<Particle> &particles, int w)
{
	if (particles.size() < 500)
	{
		// new ones at top of screen
		Particle particle;
		particle.alive = true;
		double scale = ((double)random(100)) / 100.0;
		particle.x = random(w * 3 / 2);
		particle.y = 0;
		particle.dx = -3.0 * (1 + scale);
		particle.dy = 6.0 * (1 + scale);
		particle.color = GREY;
		particles.push_back(particle);
	}
}

void starGenerateFunction (list<Particle> &particles, int h)
{
	if (particles.size() < 800)
	{
		Particle particle;
		particle.alive = true;
		double scale = (random(100) / 100.0);
		double speed = scale * 4.0;
		particle.x = 0;
		particle.dx = speed;
		particle.y = random(h);
		particle.dy = 0;
		particle.color = al_map_rgb_f(scale, scale, scale);
		particles.push_back(particle);
	}
}

void meteorGenerateFunction (list<Particle> &particles, int h)
{
	if (particles.size() < 800)
	{
		Particle particle;
		particle.alive = true;
		double scale = (random(100) / 100.0);
		double speed = scale * 4.0;
		particle.x = 0;
		particle.dx = speed;
		particle.y = random(h);
		particle.dy = 0;
		particle.color = al_map_rgb_f(scale, scale, scale);
		particles.push_back(particle);
	}
}

void linearUpdateFunction (Particle *p)
{
	p->x += p->dx;
	p->y += p->dy;
	// fast movement down and left
}

void flurryUpdateFunction (Particle *p)
{
	double dx = (double)(random(5) - 2) / 2.0;
	double dy = (double)(random(5)) / 2.0;
	p->x += dx;
	p->y += dy;
	// flurry. random movement downwards
}

void circleDrawFunction (Particle *p)
{
	al_draw_filled_circle(p->x, p->y, 2.0, p->color);
}

void lineDrawFunction (Particle *p)
{
	al_draw_line(p->x, p->y, p->x + (p->dx * 2), p->y + (p->dy * 2), p->color, 2.0);
}

class ParticleRemover
{
public:
	bool operator()(Particle &p)
	{
		if (!p.alive)
		{
			return true;
		}
		return false;
	}
};

void Particles::update()
{
	currentEffect.generateFunction(particles);

	// update existing
	for (list<Particle>::iterator p = particles.begin(); p != particles.end(); ++p)
	{
		if (!p->alive) continue;

		currentEffect.updateFunction(&(*p));

		if (p->x > (w + 200) || p->x < (-w - 200) || p->y > (h + 100) || p->y < (-100)) { p->alive = false; }
	}

	// remove all that are not alive!
	particles.remove_if (ParticleRemover());
}


Particles::Particles() : effect(CLEAR), currentEffect(), particles()
{
	ALLEGRO_DISPLAY *d = al_get_current_display();
	w = al_get_display_width(d);
	h = al_get_display_height(d);

}

void Particles::draw(const GraphicsContext &gc)
{
	for (list<Particle>::iterator p = particles.begin(); p != particles.end(); ++p)
	{
		if (!p->alive) continue;
		currentEffect.drawFunction(&(*p));
	}
}


Particles::~Particles() {}

using namespace std::placeholders;

void Particles::setEffect(ParticleEffect p)
{
	if (p != effect)
	{
		particles.clear();
		effect = p;

		switch (effect)
		{
		case CLEAR:
			currentEffect.generateFunction = &clearGenerateFunction;
			currentEffect.updateFunction = &clearUpdateFunction;
			currentEffect.drawFunction = &clearUpdateFunction;
			break;
		case METEOR:
			currentEffect.generateFunction = std::bind (meteorGenerateFunction, _1, geth());
			currentEffect.updateFunction = &linearUpdateFunction;
			currentEffect.drawFunction = &circleDrawFunction;
			break;
		case SNOW:
			currentEffect.generateFunction = std::bind(snowGenerateFunction, _1, getw());
			currentEffect.updateFunction = &flurryUpdateFunction;
			currentEffect.drawFunction = &circleDrawFunction;
			break;
		case CONFETTI:
			currentEffect.generateFunction = std::bind(confettiGenerateFunction, _1, getw());
			currentEffect.updateFunction = &flurryUpdateFunction;
			currentEffect.drawFunction = &circleDrawFunction;
			break;
		case STARS:
			currentEffect.generateFunction = std::bind(starGenerateFunction, _1, geth());
			currentEffect.updateFunction = &linearUpdateFunction;
			currentEffect.drawFunction = &circleDrawFunction;
			break;
		case RAIN:
			currentEffect.generateFunction = std::bind(rainGenerateFunction, _1, getw());
			currentEffect.updateFunction = &linearUpdateFunction;
			currentEffect.drawFunction = &lineDrawFunction;
			break;
		case ANTIGRAV:
			currentEffect.generateFunction = std::bind(antigravGenerateFunction, _1, getw(), geth());
			currentEffect.updateFunction = &linearUpdateFunction;
			currentEffect.drawFunction = &lineDrawFunction;
			break;
		}

	}

}
