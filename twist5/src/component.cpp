#include "component.h"
#include <stdio.h>
#include <iostream>

using namespace std;

void IComponent::draw(const GraphicsContext &gc)
{
	if (!(alive && visible)) return;
}

/** helper to handle messages */
void IComponent::checkMessages(IComponentPtr ptr)
{
	while (ptr->hasMsg())
	{
		int msg = ptr->popMsg();
		handleMessage(ptr, msg);
	}
}

/**
 * IComponent version handles MSG_FOCUS, MSG_UNFOCUS and MSG_KILL.
 * Any other message is passed to onHandleMessage.
 */
void IComponent::handleMessage(std::shared_ptr<IComponent> src, int msg)
{
	switch (msg)
	{
	case MSG_FOCUS:
		awake = true;
		visible = true;
		onFocus();
		break;
	case MSG_UNFOCUS:
		awake = false;
		visible = false;
		break;
	case MSG_KILL:
		kill();
		break;
	default:
		bool handled = onHandleMessage(msg);
		if (!handled)
		{
			pushMsg(msg); // let parent handle it.
		}
		break;
	}

}

void IComponent::repr(int indent, std::ostream &out) const
{
	for (int i = 0; i < indent; ++i)
	{
		out << "  ";
	}

	out << className() << " [" << x << ", " << y << " - " << w << "x" << h << "] "
			<< (isVisible() ? "v" : "!v")
			<< endl;
}

void IComponent::doLayout(int px, int py, int pw, int ph)
{
	if (layout_flags & Layout::DISABLED) return; // automatic layout disabled
	layout_initialised = true;

	int sub_flags;

	sub_flags = layout_flags & (Layout::_Impl::LEFT | Layout::_Impl::CENTER | Layout::_Impl::RIGHT);
	switch (sub_flags)
	{
	case Layout::_Impl::LEFT:
		x = layout_x1 + px;
		break;
	case Layout::_Impl::CENTER:
		x = layout_x1 + px + (pw - layout_x2) / 2 ;
		break;
	case Layout::_Impl::RIGHT:
		x = (px + pw) - layout_x1 - layout_x2;
		break;
	default:
		assert(false); /* "Invalid flag combination" */
		break;
	}

	if (layout_flags & Layout::_Impl::_W)
	{
		w = layout_x2;
	}
	else if (layout_flags & Layout::_Impl::TO_RIGHT)
	{
		w = (px + pw) - layout_x2 - x;
	}
	else { assert(false); /* "Invalid flag combination" */ }


	sub_flags = layout_flags & (Layout::_Impl::TOP | Layout::_Impl::MIDDLE | Layout::_Impl::BOTTOM);
	switch (sub_flags)
	{
	case Layout::_Impl::TOP:
		y = layout_y1 + py;
		break;
	case Layout::_Impl::MIDDLE:
		y = layout_y1 + (ph - layout_y2) / 2;
		break;
	case Layout::_Impl::BOTTOM:
		y = (py + ph) - layout_y1 - layout_y2;
		break;
	default:
		assert(false); /* "Invalid flag combination" */
		break;
	}


	if (layout_flags & Layout::_Impl::_H)
	{
		h = layout_y2;
	}
	else if (layout_flags & Layout::_Impl::TO_BOTTOM)
	{
		h = (py + ph) - layout_y2 - y;
	}
	else { assert(false); /* "Invalid flag combination" */ }


	UpdateSize();
}
