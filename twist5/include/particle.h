#ifndef PARTICLE_H
#define PARTICLE_H

#include <list>
#include <functional>
#include <allegro5/allegro.h>
#include "component.h"

enum ParticleEffect {
	SNOW, RAIN, STARS, CLEAR, CONFETTI, ANTIGRAV, METEOR
};

struct Particle
{
	double x;
	double y;
	double z;
	double dx;
	double dy;
	double dz;
	bool alive;
	ALLEGRO_COLOR color;
};

typedef std::function< void(Particle *)> ParticleFunc;
typedef std::function< void(std::list<Particle> &)> ParticleFactoryFunc;

struct ParticleEffectImpl
{
public:
	ParticleEffectImpl();

	ParticleFactoryFunc generateFunction;
	ParticleFunc updateFunction;
	ParticleFunc drawFunction;
};

class Particles : public IComponent
{
private:
	ParticleEffect effect;
	ParticleEffectImpl currentEffect;
	std::list<Particle> particles;
public:
	Particles();
	virtual void update();
	virtual void draw(const GraphicsContext &gc);
	virtual ~Particles();
	void setEffect(ParticleEffect p);

	void setGenerateFunction(ParticleFactoryFunc value) { currentEffect.generateFunction = value; }
	void setUpdateFunction(ParticleFunc value) { currentEffect.updateFunction = value; }
	void setDrawFunction(ParticleFunc value) { currentEffect.drawFunction = value; }
};

#endif
