#include "widget.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "color.h"

void Widget::SetTooltipText (const char *txt) { /* TODO */ }
void Widget::SetCallbackID(int id) { key = id; }
void Widget::Redraw() { /* TODO */ }
void Widget::Redraw(const Rect &rect) { /* TODO */ }

bool Widget::MsgClose() { return false; /* TODO */ }
void Widget::MsgTick() { /* TODO assert (false && "deprecated, use 'update' instead"); */ } // TODO same as update!
void Widget::MsgTimer(int id) { /* TODO */ }
void Widget::HandleEvent( Widget &source, int msg, int value) { /* TODO */ }
void Widget::MsgStart () { /* TODO */ }
void Widget::MsgMousemove(const Point &d) { }
void Widget::MsgDraw () { /* TODO */ }
bool Widget::HasMouse () { return false; /* TODO */ }
void Widget::MsgInitSkin () { /* TODO */ }
Skin *Widget::GetSkin() { assert (theSkin != NULL); return theSkin; }
ALLEGRO_COLOR Widget::GetFontColor(int state) { return RED; /* TODO */ }
ALLEGRO_COLOR Widget::GetShadowColor(int state) { return GREY; /* TODO */ }
ALLEGRO_FONT *Widget::GetFont(int state) { return sfont; /* TODO */ }

void Widget::draw(const GraphicsContext &gc)
{
	if (TestFlag(D_DOUBLEBUFFER))
	{
		if (buffer == NULL) resetBuffer();
		if (dirty) updateBuffer();

		al_draw_bitmap(buffer, x + gc.xofst, y + gc.yofst, 0);
	}
	else
	{
		doDraw(gc);
	}
}

void Widget::resetBuffer() {
	if (buffer != NULL)
	{
		al_destroy_bitmap(buffer);
		buffer = NULL;
	}
	dirty = true;
}

void Widget::updateBuffer()
{
	if (buffer == NULL) {
		buffer = al_create_bitmap (getw(), geth());
		assert(buffer);
	}

	ALLEGRO_BITMAP* saved = al_get_target_bitmap();
	al_set_target_bitmap(buffer);

	GraphicsContext gc;
	gc.xofst = -getx();
	gc.yofst = -gety();
	gc.buffer = buffer;

	doDraw (gc);

	dirty = false;

	al_set_target_bitmap(saved);
}

void Widget::handleEvent(ALLEGRO_EVENT &event)
{
	switch (event.type)
	{
		case ALLEGRO_EVENT_MOUSE_AXES:
			{
				MsgMousemove(Point(event.mouse.x, event.mouse.y));
			}
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			if (event.mouse.button & 1) // left button
			{
				MsgLPress(Point(event.mouse.x, event.mouse.y));
			}
			if (event.mouse.button & 2)
			{
				MsgRPress(Point(event.mouse.x, event.mouse.y));
			}
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (event.mouse.button & 1) // left button
			{
				MsgLRelease(Point(event.mouse.x, event.mouse.y));
			}
			if (event.mouse.button & 2)
			{
				MsgRRelease(Point(event.mouse.x, event.mouse.y));
			}
			break;
		case ALLEGRO_EVENT_KEY_CHAR:
			//TODO
			MsgChar(event.keyboard.keycode, event.keyboard.modifiers);
			break;
		case ALLEGRO_EVENT_TWIST_MOUSE_ENTER_WIDGET:
			onMouseEnter();
			break;
		case ALLEGRO_EVENT_TWIST_MOUSE_LEAVE_WIDGET:
			onMouseLeave();
			break;
    }


}

void Widget::SetFlag(int flag) { flags |= flag; }
void Widget::ClearFlag(int flag) { flags &= -flag; }
bool Widget::TestFlag(int flag) const { return ((flags & flag) == flag); }

void Widget::Setup(int x, int y, int w, int h, int key, int flags)
{
	setLocation(x, y, w, h);
	SetCallbackID(key);
	this->flags = flags;
}

bool Widget::operator==(const Widget& obj) const { return (this == &obj); }
bool Widget::operator!=(const Widget& obj) const { return (this != &obj); }

