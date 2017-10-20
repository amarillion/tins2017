#ifndef MENUBASE_H
#define MENUBASE_H

#include <string>
#include <vector>
#include "component.h"
#include <memory>
#include "container.h"

struct ALLEGRO_BITMAP;
struct ALLEGRO_SAMPLE;
struct ALLEGRO_FONT;

class Sound;

class MenuList;

class MenuItem : public IComponent
{
	protected:
		MenuList *parent;
		bool flashing;
		bool enabled;
	public:
		enum { MENU_NONE = 1000, MENU_PREV, MENU_NEXT };
		MenuItem () : parent(NULL), flashing (false), enabled(true) {}
		virtual ~MenuItem() {}
		bool isSelected();
		void setParent (MenuList *val) { parent = val; }
		virtual std::string getText() = 0;
		virtual std::string getHint() = 0;
		virtual void draw(const GraphicsContext &gc);
		void setEnabled (bool value) { enabled = value; }
		virtual bool isEnabled() { return enabled; }

		ALLEGRO_COLOR getColor();

		virtual std::string const className() const override { return "MenuItem"; }
};

class ActionMenuItem : public MenuItem
{
	private:
		int action; // code returned from handleInput
		std::string text;
		std::string hint;
	public:
		ActionMenuItem (int _action, std::string _text, std::string _hint) :
			MenuItem(), action (_action), text (_text), hint (_hint) {}
		virtual void handleEvent(ALLEGRO_EVENT &event) override;
		virtual std::string getText() { return text; }
		virtual std::string getHint() { return hint; }
		void setText (std::string value) { text = value; }
};

class ToggleMenuItem : public MenuItem
{
private:
	int action;
	std::string a;
	std::string b;
	std::string hint;
	bool toggle;
public:
	ToggleMenuItem(int _action, std::string _a, std::string _b, std::string _hint);
	void setToggle(bool value) { toggle = value; }
	bool getToggle() { return toggle; }
	virtual void handleEvent(ALLEGRO_EVENT &event) override;
	virtual std::string getText() { return toggle ? a : b; }
	virtual std::string getHint() { return hint; }
};

typedef std::shared_ptr<MenuItem> MenuItemPtr;

struct ALLEGRO_SAMPLE;

class MenuList : public Container
{
private:
	IComponentPtr hint;
	std::vector<MenuItemPtr> items;
	ALLEGRO_SAMPLE *sound_enter;
	ALLEGRO_SAMPLE *sound_cursor;
	Sound *sound;
	ALLEGRO_COLOR colorNormal;
	ALLEGRO_COLOR colorFlash1;
	ALLEGRO_COLOR colorFlash2;
	ALLEGRO_COLOR colorDisabled;
	friend class MenuItem;
	void prev();
	void next();
	unsigned int selected;
	int tFlash;
	int topMargin;
	int bottomMargin;
public:
	MenuList(Sound *sound = NULL);
	virtual ~MenuList() {}
	unsigned int size () { return items.size(); }
	void push_back(MenuItemPtr item)
	{
		// add both to items and children.
		items.push_back(item);
		item->setParent(this);
		add (item);
	}
	MenuItemPtr& operator[] (int idx) { return items[idx]; }
	void setMargin(int top, int bottom) { topMargin = top; bottomMargin = bottom; }
	virtual void onUpdate() override;
	virtual void handleEvent(ALLEGRO_EVENT &event) override;
	virtual void onFocus () override;
	int getSelectedIdx() { return selected; }
	MenuItemPtr getSelectedItem() { return items[selected]; }

	virtual std::string const className() const override { return "MenuList"; }

	/** Slightly more customizable layout, still with vertical equal spacing, but allows setting top and bottom margins */
	static void marginAdjustedlayout(int topMargin, int bottomMargin, IComponentPtr comp, IComponentPtr prev, int idx, int size, int px, int py, int pw, int ph);
	/** Standard menu layout, stacks menu items vertically, equally spaced */
	static void layoutFunction(IComponentPtr comp, IComponentPtr prev, int idx, int size, int px, int py, int pw, int ph);

	/** Standard menu layout, stacks menu items vertically in two columns */
	static void twoColumnLayoutFunction(IComponentPtr comp, IComponentPtr prev, int idx, int size, int px, int py, int pw, int ph);
};

typedef std::shared_ptr<MenuList> MenuListPtr;

class MenuBuilder
{
public:
	MenuBuilder(Container *parent, Sound *sound);

	/** by default, menu items will be in layout group 1 */
	MenuBuilder &push_back (MenuItemPtr item, int groupId = 1)
	{
		item->setGroupId(groupId);
		result->push_back(item);
		return *this;
	}

	MenuListPtr build () { return result; }

private:
	MenuListPtr result;
};

class Hint : public IComponent
{
private:
	MenuList *parent;
public:
	Hint(MenuList *parent) { this->parent = parent; }
	virtual void draw (const GraphicsContext &gc);

	virtual std::string const className() const override { return "Hint"; }
};

#endif
