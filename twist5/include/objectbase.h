#ifndef SPRITEBASE_H
#define SPRITEBASE_H

#include <list>
#include "component.h"
#include "timer.h"
#include <allegro5/allegro.h>

class Anim;

class ObjectBase : public IComponent
{
protected:
	int animstart;
	int animstate;
	Anim *current;

	static bool *debugFlag;
	static ITimer *timer;
	double x, y;
	int dir;
	int w, h;
	bool solid;
	virtual void setAnim (Anim *a, int state = 0);
public:
	ObjectBase () : animstart(0), animstate(0), current(NULL), x(0), y(0), dir(0), w(0), h(0), solid(true) {}
	double getx () { return x; }
	double gety () { return y; }
	int getw () { return w; }
	int geth () { return h; }
	bool isSolid() { return solid; } // can collide with other stuff?
	virtual void draw (const GraphicsContext &gc);
	virtual void handleCollission(ObjectBase *o) = 0;
	virtual int getType () = 0;
	int getDir() { return dir; }
	virtual void setDir(int _dir) { dir = _dir; }
	virtual void setLocation (double nx, double ny);
	static void init(bool *_debugFlag, ITimer *_timer) { debugFlag = _debugFlag; timer = _timer; }
};

class ObjectListBase : public IComponent
{
protected:
	std::list<ObjectBase *> objects;
	void purge();
public:
	virtual void update() override;
	virtual void draw (const GraphicsContext &gc) override;
	void killAll();
	unsigned int size () { return objects.size(); }
};

#endif
