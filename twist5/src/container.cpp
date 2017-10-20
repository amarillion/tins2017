/*
 * Container.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "container.h"
#include "timer.h"
#include <allegro5/allegro.h>
#include <numeric>
#include <algorithm>
#include "util.h"

using namespace std;

void Container::setFont(ALLEGRO_FONT *font)
{
	// set the font and the font of children, if needed.
	IComponent::setFont(font);
	list<IComponentPtr>::iterator i;
	for (auto i : children)
	{
		if (!(i->getFont()))
			i->setFont(font);
	}
}

void Container::add (std::shared_ptr<IComponent> item, int flags)
{
	assert (item != nullptr);

	// prevent duplicates
	if (find(children.begin(), children.end(), item) != children.end())
	{
		cout << "WARN: Double addition of component, ignoring" << endl;
	}
	else
	{
		if (flags & FLAG_BOTTOM)
			children.push_front(item);
		else
			children.push_back (item);
	}

	if (flags & FLAG_SLEEP)
	{
		item->setAwake(false);
		item->setVisible(false);
	}

	if (isLayoutInitialised())
	{
		item->doLayout(getx(), gety(), getw(), geth());
	}

	//TODO: might needs some rethinking.
	// Inheritance of font like this is not ideal.
	// it is not clear when to set them if they should be different from parent.

	if (!(item->getFont()))
		item->setFont(sfont);
}

void Container::update()
{
	IComponent::update();

	for (auto &i : children)
	{
		if (i->isAwake())
		{
			i->update();
			checkMessages(i);
		}
	}

	purge();
}

void Container::draw(const GraphicsContext &gc)
{
	for (auto i : children)
	{
		if (i->isVisible())
		{
			assert (i->getw() * i->geth() > 0); //trying to draw an invisibly small component

			if (!TestFlag(D_DISABLE_CHILD_CLIPPING)) {
				al_set_clipping_rectangle(i->getx() + gc.xofst, i->gety() + gc.yofst, i->getw() + 1, i->geth() + 1);
				i->draw(gc);
				al_reset_clipping_rectangle();
			}
			else
			{
				i->draw(gc);
			}

		}
	}
}

void Container::setFocus(IComponentPtr _focus)
{
	assert (_focus);
	assert (find(children.begin(), children.end(), _focus) != children.end()); // trying to set focus to somethign that wasn't added first

	if (focus)
	{
		focus->handleMessage(nullptr, MSG_UNFOCUS);
	}
	focus = _focus;
	focus->handleMessage(nullptr, MSG_FOCUS);
}

void Container::handleEvent (ALLEGRO_EVENT &event)
{
	switch (event.type)
	{
		// twist events
		case TWIST_START_EVENT:
		{
			MsgStart();
			break;
		}

		// mouse events

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		case ALLEGRO_EVENT_MOUSE_AXES:
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			int current_mouse_x = event.mouse.x;
			int current_mouse_y = event.mouse.y;

			// find new mouse component
			IComponentPtr newMouseFocus = nullptr;

			// we have to send a click message
			// go through in reverse drawing order and find the component on top.
			for (auto i = children.rbegin(); i != children.rend(); i++)
			{
				if ((*i)->contains(current_mouse_x, current_mouse_y) && (*i)->isVisible())
				{
					newMouseFocus = *i;

					//TODO - item should request focus itself...
//					if ((*i)->wantsFocus())
//					   setFocus (*i);

					break; // break from loop
				}
			}

			if (newMouseFocus != mouseFocus)
			{
				ALLEGRO_EVENT evt2 = event;
				evt2.type = ALLEGRO_EVENT_TWIST_MOUSE_LEAVE_WIDGET;

				if (mouseFocus)	mouseFocus->handleEvent(evt2);
				mouseFocus = newMouseFocus;

				evt2.type = ALLEGRO_EVENT_TWIST_MOUSE_ENTER_WIDGET;
				if (mouseFocus) mouseFocus->handleEvent(evt2);
			}

			if (mouseFocus)
			{
				mouseFocus->handleEvent(event);
				checkMessages(mouseFocus);
			}

//			if (!focus) searchNextFocusItem();
		}
			break;
		// non-mouse events
		default:
			if (focus)
			{
				focus->handleEvent(event);
				checkMessages(focus);
			}
			break;
	}

}

void Container::handleMessage(std::shared_ptr<IComponent> src, int msg)
{
	IComponent::handleMessage(src, msg);

	switch (msg)
	{
	case MSG_CLOSE:
		// src->kill(); // TODO: need a way to schedule removal without setting awake = false;
		break;
	}

}

void Container::setTimer(int msec, int event)
{
	TimerPtr timer = TimerPtr(new Timer(msec, event));
	add (timer);
}

class MyComponentRemover
{
public:
	bool operator()(IComponentPtr o)
	{
		if (!o->isAlive())
		{
			o.reset();
			return 1;
		}
		return 0;
	}
};

void Container::purge()
{
	// remove all that are not alive!
	children.remove_if (MyComponentRemover());
}

void Container::killAll()
{
	list<IComponentPtr>::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		(*i)->kill();
	}
	purge();
}

void Container::repr(int indent, std::ostream &out) const
{
	IComponent::repr (indent, out);

	for (auto child : children)
	{
		child->repr(indent + 1, out);
	}
}

void Container::UpdateSize()
{
	int groupIdx[MAX_GROUP_ID] = {0};
	int groupSizes[MAX_GROUP_ID] = {0};

	// layout children that are not part of a group while counting group sizes
	for (auto child : children)
	{
		if (child->getGroupId() == 0)
		{
			child->doLayout(getx(), gety(), getw(), geth());
		}
		else
		{
			groupSizes[child->getGroupId()] ++;
		}
	}

	// layout children that *are* part of a group
	IComponentPtr prev = IComponentPtr();
	for (auto child : children)
	{
		int groupId = child->getGroupId();
		if (groupId != 0)
		{
			int groupSize = groupSizes[groupId];
			int idx = groupIdx[groupId];
			groupLayouts[groupId](child, prev, idx, groupSize, getx(), gety(), getw(), geth());
			groupIdx[groupId]++;
			prev = child;
		}
	}

	onResize();
}

double Container::getMaxRight()
{
	return accumulate(children.begin(), children.end(),
			-numeric_limits<double>::max(),
			[] (double chain, IComponentPtr a) { return max(a->getRight(), chain); });
}

double Container::getMinLeft()
{
	return accumulate(children.begin(), children.end(),
			numeric_limits<double>::max(),
			[] (double chain, IComponentPtr a) { return min(a->getLeft(), chain); });
}

double Container::getMinTop()
{
	return accumulate(children.begin(), children.end(),
			numeric_limits<double>::max(),
			[] (double chain, IComponentPtr a) { return min(a->getTop(), chain); });
}

double Container::getMaxBottom()
{
	return accumulate(children.begin(), children.end(),
			-numeric_limits<double>::max(),
			[] (double chain, IComponentPtr a) { return max(a->getBottom(), chain); });
}

void Container::resizeToChildren()
{
	repr (0, cout);
	int a = getMinLeft();
	int b = getMinTop();
	int c = getMaxRight();
	int d = getMaxBottom();
	setLocation(a, b, c, d);
}
