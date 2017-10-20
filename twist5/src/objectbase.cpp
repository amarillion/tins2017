#include "objectbase.h"
#include "color.h"
#include "anim.h"
#include <allegro5/allegro_primitives.h>

using namespace std;

class MyObjectRemover
{
public:
	bool operator()(ObjectBase *o)
	{
		if (!o->isAlive())
		{
			delete o;
			return 1;
		}
		return 0;
	}
};

void ObjectListBase::purge()
{
	// remove all that are not alive!
	objects.remove_if (MyObjectRemover());
}

void ObjectListBase::update()
{
	list<ObjectBase*>::iterator i;
	for (i = objects.begin(); i != objects.end(); i++)
	{
		if ((*i)->isAlive()) (*i)->update();
	}

	// collission detection!
	list<ObjectBase*>::iterator j;
	for (i = objects.begin(); i != objects.end(); i++)
		for (j = objects.begin(); j != i; j++)
	{
		// see if bb interesect
		if ((*i)->isAlive() && (*j)->isAlive())
		{
			int x1 = (*i)->getx();
			int y1 = (*i)->gety();
			int w1 = (*i)->getw();
			int h1 = (*i)->geth();
			int x2 = (*j)->getx();
			int y2 = (*j)->gety();
			int w2 = (*j)->getw();
			int h2 = (*j)->geth();
			if(!((x1 >= x2+w2) || (x2 >= x1+w1) || (y1 >= y2+h2) || (y2 >= y1+h1)))
			{
				(*i)->handleCollission ((*j));
				(*j)->handleCollission ((*i));
			}
		}
	}

	purge();
	return;
}

void ObjectListBase::draw (const GraphicsContext &gc)
{
	list<ObjectBase*>::iterator i;
	for (i = objects.begin(); i != objects.end(); i++)
	{
		if ((*i)->isVisible() && (*i)->isAlive())
		{
			(*i)->draw(gc);
		}
	}
}

void ObjectListBase::killAll()
{
	list<ObjectBase*>::iterator i;
	for (i = objects.begin(); i != objects.end(); ++i)
	{
		delete (*i);
		(*i) = NULL;
	}
	objects.clear();
}

bool *ObjectBase::debugFlag = NULL;
ITimer *ObjectBase::timer = NULL;

void ObjectBase::draw (const GraphicsContext &gc)
{
	if (current)
	{
		int counter = timer->getCounter();
		current->drawFrame(gc.buffer, animstate, dir, counter - animstart, x - gc.xofst, y - gc.yofst);
	}
	if (debugFlag && (*debugFlag))
	{
		al_set_target_bitmap (gc.buffer);
		al_draw_rectangle (
			x - gc.xofst,
			y - gc.yofst,
			x - gc.xofst + w,
			y - gc.yofst + h,
			GREEN, 1.0);
	}

}

void ObjectBase::setAnim (Anim *a, int state)
{
	assert (timer);
	current = a;
	animstart = timer->getCounter();
	animstate = state;
	w = a->sizex;
	h = a->sizey;
}

void ObjectBase::setLocation (double nx, double ny)
{
	x = nx;
	y = ny;
}
