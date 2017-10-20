#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "motionimpl.h"
#include "component.h"
#include "widget.h"
#include "data.h"

/**
 * A viewport is a container that defines a different coordinate system for its children.
 * The coordinate system is passed in the draw method through GraphicsContext.
 *
 * The viewport location can be animated for different effects.
 */
class ViewPort : public IComponent, public DataWrapper
{
private:
	int xofst;
	int yofst;

	// base offset without tremble
	int basex;
	int basey;

	// destination for movement
	int destx;
	int desty;

	// TODO: try not to use this in public header
	Quake quake;

	IComponentPtr child;

	enum {
		/** offsets have changed through external action */ EVT_OFFSET_CHANGED = 10,
		/** child of viewport was set or changed */ EVT_CHILD_CHANGED,
		/** viewport has changed size */ EVT_VIEWPORT_RESIZED
	};

public:
	ViewPort () : xofst (0), yofst (0), basex(0), basey(0), destx (0),
		desty (0), quake(), child() {}

	virtual void repr(int indent, std::ostream &out) const override;

	// set directly
	void setOfst (int _xofst, int _yofst)
	{
		if (xofst != _xofst || yofst != _yofst)
		{
			xofst = _xofst;
			yofst = _yofst;
			basex = xofst;
			basey = yofst;
			destx = xofst;
			desty = yofst;
			FireEvent(EVT_OFFSET_CHANGED);
		}
	}

	void moveTo (int _xofst, int _yofst);
	void tremble (float ampl);

	/** tries to make sure the area is visible */
	void scrollToArea (int x, int y, int w, int h)
	{
	    if (x + w > getXofst() + getClientWidth())
	        moveTo (x + w - getClientWidth(), getYofst());
	    else if (x < getXofst())
	        moveTo (x, getYofst());
	    if (y + h > getYofst() + getClientHeight())
	        moveTo (getXofst(), y + h - getClientHeight());
	    else if (y < getYofst())
	        moveTo (getXofst(), y);
	}

	void setChild (IComponentPtr value);

	virtual void setFont(ALLEGRO_FONT *font) override
	{
		sfont = font;
		if (child && !child->getFont()) child->setFont(font);
	}

	virtual ~ViewPort() {}

	virtual void update() override;
	virtual void draw (const GraphicsContext &gc) override;

	virtual void UpdateSize() override
	{
		if (child)
		{
			child->handleMessage(nullptr, MSG_VIEWPORT_RESIZED);
		}
		FireEvent(EVT_VIEWPORT_RESIZED);
	}

	virtual int getXofst() { return xofst; }
	virtual int getYofst() { return yofst; }

	int getClientHeight() { if (child) return child->geth(); else return 0; }
	int getClientWidth() { if (child) return child->getw(); else return 0; }

	void handleEvent (ALLEGRO_EVENT &event) override;

	virtual std::string const className() const override { return "ViewPort"; }

	void center()
	{
		if (!child) return;

		setOfst(
			(getw() - child->getw()) / 2,
			(geth() - child->geth()) / 2
		);
	}
};

/**
 * Any Component can be a child of Viewport, but that child is not aware that it's inside that viewport.
 * Unless that is a Scrollable - then it gets access to the ViewPort, and the ability to move itself around
 * in response to events etc, find out what the viewport size is, which area is visible, etc.
 */
class Scrollable : public Widget
{
private:
	ViewPort *parent = nullptr; // TODO: use proper weak pointer structure
	friend class ViewPort;
public:
	void setParent (ViewPort *parent);
	void center() { if (parent) parent->center(); }
	int getViewPortWidth() { return parent ? parent->getw() : 0; }
};

#endif
