#ifndef _PANEL_H_
#define _PANEL_H_

#include "componentbuilder.h"
#include "widget.h"

class Panel : public Widget
{
public:
	Panel (int bitmapIdx = Skin::PANEL_SUNKEN) {
		setBorder (bitmapIdx);
		setLayout(Layout::LEFT_TOP_RIGHT_BOTTOM, 0, 0, 0, 0);
		SetFlag(D_DOUBLEBUFFER);
	}
	virtual void doDraw(const GraphicsContext &gc) override;

	static ComponentBuilder<Panel> build(int bitmapIdx = Skin::PANEL_SUNKEN) {
		return ComponentBuilder<Panel>(std::make_shared<Panel>(bitmapIdx));
	}
};

class PanelRaised : public Panel
{
public:
	PanelRaised() : Panel (Skin::PANEL_RAISED) {}
};

#endif
