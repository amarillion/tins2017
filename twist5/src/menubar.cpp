#include "menubar.h"
#include "button.h"
#include "panel.h"
#include "mainloop.h"

using namespace std;

class PopupMenuImpl;

class MenuBaseImpl : public Menu
{
protected:
	int menuItemCount = 0;
	virtual void recalculatePreferredSize() = 0;
	shared_ptr<IComponent> childPopup = nullptr;
public:
	virtual TextWidget *GetItem(int index) override { assert(false); return NULL; /* TODO */ }
	virtual void Check(int pos) override { /* TODO */ }
	virtual void Uncheck(int pos) override { /* TODO */ }
	virtual void Add(const char *item, int msg) override
	{
		auto btn = Button::build(msg, item).get();
		menuItemCount++;
		btn->setGroupId(2);
		add (btn);
		recalculatePreferredSize();
	}

	virtual void Add() override {
		menuItemCount++; // increase for layout purposes.

		auto spacer = Panel::build().get();
		spacer->setGroupId(2);
		add (spacer);

		recalculatePreferredSize();
	}

	virtual void Add(const char *submenu, shared_ptr<Menu> menu) override
	{
		auto btn = Button::build(0, submenu).get();
		btn->setGroupId(2);
		btn->setActionFunc( [=] () {

			if (this->childPopup && this->childPopup != menu)
			{
				this->childPopup->setVisible(false);
				this->childPopup->setAwake(false);
				this->childPopup = nullptr;
			}

			menu->setxy(btn->getLeft() + 10, btn->getBottom());
			menu->setVisible(true);
			menu->setAwake(true);
			this->childPopup = menu;
			MainLoop::getMainLoop()->popup(menu, shared_from_this());
		});

		add (btn);
		menuItemCount++;
		recalculatePreferredSize();
	}

	virtual ~MenuBaseImpl() {}
};

class PopupMenuImpl : public MenuBaseImpl
{
	static const int MENU_ITEM_HEIGHT = 16; //TODO: part of skin
	static const int MENU_ITEM_WIDTH = 160; //TODO: part of skin

	static void layoutFunction(std::shared_ptr<IComponent> comp, std::shared_ptr<IComponent> prev, int idx, int size, int px, int py, int pw, int ph)
	{
		int dy = MENU_ITEM_HEIGHT;
		comp->setLocation(px, py + idx * dy, MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT);
	}

	void recalculatePreferredSize()
	{
		setDimension(MENU_ITEM_WIDTH, menuItemCount * MENU_ITEM_HEIGHT);
	}

public:
	PopupMenuImpl()
	{
		setGroupLayout(2, PopupMenuImpl::layoutFunction);
	}

	virtual void close()
	{
		pushMsg(MSG_CLOSE); // should trigger closing of popup...
	}

	virtual void onMouseLeave() override
	{
		close();
	}

	virtual ~PopupMenuImpl() {}
};

class MenuBarImpl : public MenuBaseImpl
{
	static const int MENU_ITEM_HEIGHT = 16; //TODO: part of skin
	static const int MENU_ITEM_WIDTH = 160; //TODO: part of skin
private:
	static void layoutFunction(std::shared_ptr<IComponent> comp, std::shared_ptr<IComponent> prev, int idx, int size, int px, int py, int pw, int ph)
	{
		int xco;
		if (prev)
		{
			xco = prev->getRight();
		}
		else
		{
			xco = px;
		}

		int dx = MENU_ITEM_WIDTH;
		comp->setLocation(xco, py, comp->getPreferredWidth(), MENU_ITEM_HEIGHT);
	}

	void recalculatePreferredSize()
	{
		setLayout(Layout::LEFT_TOP_W_H, getx(), gety(), menuItemCount * MENU_ITEM_WIDTH, MENU_ITEM_HEIGHT);
	}

public:
	MenuBarImpl()
	{
		setGroupLayout(2, MenuBarImpl::layoutFunction);
	}

	virtual ~MenuBarImpl() {}

	virtual void update() override {
		MenuBaseImpl::update();
		if (childPopup != nullptr)
		{
			checkMessages(childPopup);
		}
	}

};

ComponentBuilder<Menu> Menu::build(bool fMenuBar)
{
	if (fMenuBar)
	{
		return ComponentBuilder<Menu>(make_shared<MenuBarImpl>());
	}
	else
	{
		return ComponentBuilder<Menu>(make_shared<PopupMenuImpl>());
	}
}
