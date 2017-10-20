#ifndef COMPONENT_H
#define COMPONENT_H

#include "graphicscontext.h"
#include <list>
#include "motion.h"
#include <memory>
#include "motionimpl.h"
#include <allegro5/allegro.h>
#include <iostream>

struct ALLEGRO_FONT;

class Layout {

public:

	// implementation constants, don't use directly
	enum _Impl {
		LEFT   = 1,
		RIGHT  = 2,
		TOP    = 4,
		_W      = 16,
		BOTTOM = 8,
		_H      = 32,
		CENTER = 64,
		MIDDLE = 128,
		TO_BOTTOM = 256,
		TO_RIGHT = 512,
		_DISABLED = 1024
	};


	enum {
		LEFT_TOP_W_H          = _Impl::LEFT   | _Impl::TOP    | _Impl::_W       | _Impl::_H,
		LEFT_TOP_RIGHT_H      = _Impl::LEFT   | _Impl::TOP    | _Impl::TO_RIGHT | _Impl::_H,
		LEFT_TOP_W_BOTTOM     = _Impl::LEFT   | _Impl::TOP    | _Impl::_W       | _Impl::TO_BOTTOM,
		LEFT_TOP_RIGHT_BOTTOM = _Impl::LEFT   | _Impl::TOP    | _Impl::TO_RIGHT | _Impl::TO_BOTTOM,
		LEFT_MIDDLE_W_H       = _Impl::LEFT   | _Impl::MIDDLE | _Impl::_W       | _Impl::_H,
		LEFT_MIDDLE_RIGHT_H   = _Impl::LEFT   | _Impl::MIDDLE | _Impl::TO_RIGHT | _Impl::_H,
		LEFT_BOTTOM_W_H       = _Impl::LEFT   | _Impl::BOTTOM | _Impl::_W       | _Impl::_H,
		LEFT_BOTTOM_RIGHT_H   = _Impl::LEFT   | _Impl::BOTTOM | _Impl::TO_RIGHT | _Impl::_H,
		CENTER_TOP_W_H        = _Impl::CENTER | _Impl::TOP    | _Impl::_W       | _Impl::_H,
		CENTER_TOP_W_BOTTOM   = _Impl::CENTER | _Impl::TOP    | _Impl::_W       | _Impl::TO_BOTTOM,
		CENTER_MIDDLE_W_H     = _Impl::CENTER | _Impl::MIDDLE | _Impl::_W       | _Impl::_H,
		CENTER_BOTTOM_W_H     = _Impl::CENTER | _Impl::BOTTOM | _Impl::_W       | _Impl::_H,
		RIGHT_TOP_W_H         = _Impl::RIGHT  | _Impl::TOP    | _Impl::_W       | _Impl::_H,
		RIGHT_TOP_W_BOTTOM    = _Impl::RIGHT  | _Impl::TOP    | _Impl::_W       | _Impl::TO_BOTTOM,
		RIGHT_MIDDLE_W_H      = _Impl::RIGHT  | _Impl::MIDDLE | _Impl::_W       | _Impl::_H,
		RIGHT_BOTTOM_W_H      = _Impl::RIGHT  | _Impl::BOTTOM | _Impl::_W       | _Impl::_H,
		DISABLED              = _Impl::_DISABLED

	};
};

/** Maximum value you can use for setGroupId */
const int MAX_GROUP_ID = 16;

class Reflectable
{
public:
	virtual std::string const className() const = 0;
	virtual ~Reflectable() {}
};

class IComponent : public std::enable_shared_from_this<IComponent>, public Reflectable
{
private:
	// messages destined for the parent.
	std::list<int> msgQ;

	/** layout flags determine how the layout coordinates are applied (i.e. align top-left, center, fill horizontally, etc.) */
	int layout_flags;

	/** layout_initialised indicates that doLayout has been called by the parent,
	 * and so we can expect reasonably valid coordinates to have been set. This is used by Container to
	 * determine if it's ok to call doLayout with its own coordinates. */
	bool layout_initialised;

	int layout_x1, layout_y1, layout_x2, layout_y2;

protected:
	// if awake is set to false, update() will not be called.
	bool awake;
	// if alive is set to false, this component will be removed
	// neither update() nor draw() will be called.
	bool alive;
	// if visible is set to false, draw() will not be called
	bool visible;

	ALLEGRO_FONT *sfont;

	//TODO: perhaps the Motion should get the counter and not the component.
	int counter;

	IMotionPtr motion;

	double x, y;
	int w, h;

	int groupId;
	/**
	 * Add a message to the queue. The message queue will be read by this component's parent.
	 * pushMsg() may be called from any method including handleEvent, handleMessage and update.
	 */
	void pushMsg(int msg) { msgQ.push_back(msg); assert (msgQ.size() < 10); }

	/** Helper used by Containers to handle messages. Check the messages of a (child) component and call handleMessage if there are any in the queue */
	void checkMessages(std::shared_ptr<IComponent> ptr);
public:
	bool isLayoutInitialised() { return layout_initialised; }

	/** may be overridden to return a more optimal preferred width */
	virtual double getPreferredWidth() { return w; }

	/*
	 * Check if there are any messages queued.
	 * Should be called by this component's parent.
	 */
	bool hasMsg() { return !msgQ.empty(); }

	/*
	 * Pop a message from the queue
	 * Should be called by this component's parent.
	 */
	int popMsg() { int msg = msgQ.front(); msgQ.pop_front(); return msg; }


	enum { MSG_FOCUS = 2000, MSG_UNFOCUS, MSG_KILL };

	virtual ~IComponent() {}

	IComponent () :
		layout_flags(Layout::LEFT_TOP_RIGHT_BOTTOM), layout_initialised(false), layout_x1(0), layout_y1(0), layout_x2(0), layout_y2(0),
		awake(true), alive(true), visible(true), sfont(NULL), counter(0), motion(),
		x(0), y(0), w(0), h(0),
		groupId(0)
	{}


	/**
	 * update, called for each heartbeat tick.
	 *
	 * Don't override this, override onUpdate, unless you want to change behavior drastically
	 */
	virtual void update() { counter++; onUpdate(); }

	/**
	 * update, called for each heartbeat tick.
	 * Designed to be overridden.
	 */
	virtual void onUpdate() {}

	virtual void draw(const GraphicsContext &gc);

	void setMotion(const IMotionPtr &value) { motion = value; }
	IMotionPtr getMotion() { return motion; }

	//int getCounter() { return counter; }

	/**
	 * handle a control message.
	 * Override onHandleMessage, unless you wish to alter behavior drastically
	 */
	virtual void handleMessage(std::shared_ptr<IComponent> src, int msg);

	/**
	 * handle Control message. destined for overriding.
	 *
	 * return true if the message has been handled, otherwise,
	 * the message will be passed on to the parent of this component.
	 */
	virtual bool onHandleMessage(int msg) { return false; };

	/**
	 * Called whenever this component receives a MSG_FOCUS message.
	 * By default, component will become awake and visible.
	 *
	 * Override this to do additional initialisation when focus is gained.
	 */
	virtual void onFocus() { }

	/**
	 * Override this,
	 *
	 * 	returns true in case this element can receive focus, and will be picked when searching for the next
	 * 	focus item after pressing tab.
	 *
	 * 	returns false in case this component can not receive focus, will be skipped when searching for next
	 * 	focus item after pressing tab.
	 *
	 */
    virtual bool wantsFocus () { return false; }

	/** handle input event. */
	virtual void handleEvent(ALLEGRO_EVENT &event) {}

	// if awake is set to false, update() will not be called.
	bool isAwake() const { return awake; }
	void setAwake(bool value) { awake = value; }

	// if alive is set to false, this component will be removed
	// neither update() nor draw() will be called.
	bool isAlive() const { return alive; }
	void kill() { alive = false; }
	void respawn() { alive = true; }

	// if visible is set to false, draw() will not be called
	bool isVisible() const { return visible; }
	void setVisible(bool value) { visible = value; }

	virtual void setFont(ALLEGRO_FONT *font) { this->sfont = font; }
	ALLEGRO_FONT *getFont() { return sfont; }

	double gety() { return y; }
	double getx() { return x; }
	int getw () const { return w; }
	int geth () const { return h; }
	void sety(double _y) { y = _y; }
	void setx(double _x) { x = _x; }

	double getRight() { return x + w; }
	double getBottom() { return y + h; }
	double getTop() { return y; }
	double getLeft() { return x; }

	void seth(int _h) { h = _h; UpdateSize(); }
	void setxy(double _x, double _y) {
		layout_flags = Layout::DISABLED;
		x = _x; y = _y; UpdateSize();
	}
	void setLocation (double _x, double _y, int _w, int _h) {
		layout_flags = Layout::DISABLED;
		x = _x; y = _y; w = _w; h = _h; UpdateSize();
	}
	void setDimension (int _w, int _h)
	{
		w = _w;
		h = _h;
		UpdateSize();
	}

	void setLayout (int flags, int x1, int y1, int x2, int y2)
	{
		layout_flags = flags;
		layout_x1 = x1;
		layout_y1 = y1;
		layout_x2 = x2;
		layout_y2 = y2;
	}

	/** calculate position based on parent size. Must be invoked by the parent Container. After calling this at least once, layout_initialised will be true. */
	void doLayout(int px, int py, int pw, int ph);

	/** set group id. Note that value of 0 means 'no group' */
	void setGroupId(int val) { assert (groupId >= 0 && groupId < MAX_GROUP_ID); groupId = val; }
	int getGroupId() { return groupId; }
protected:
	/** NB: preferred method for Widget and up is to override 'onResize' This is called at least once at initialisation time */
	virtual void UpdateSize() {}
public:

	/**
	check if the point cx, cy is within the bounding box of the gui item.
	usefull for checking if the mouse is over the gui item.
	*/
	bool contains (int cx, int cy) {
	    return (cx >= x && cy >= y && cx < (x + w) && cy < (y + h));
	}


	/** write stats on this component for debugging purposes */
	virtual void repr(int indent, std::ostream &out) const;

	virtual std::string const className() const override { return "Component"; }
};

typedef std::shared_ptr<IComponent> IComponentPtr;

#endif
