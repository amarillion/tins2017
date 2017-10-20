#ifndef DIALOG_H
#define DIALOG_H

#include "component.h"
#include "container.h"
#include "DrawStrategy.h"

void MASSetMouseCursor (Cursor &val);

class Dialog;

class Dialog : public Container
{
public:
	void Add (Widget &widget); //TODO: harmonise with Container->add
	void Remove (Widget &widget); //TODO: harmonise with Container->remove
	void Close();

	void Execute();
	void ChangeResolution(int gfxMode, bool fullscreen, int w, int h, int colorDepth, Dialog *dlg);

	void Popup(IComponent *parent) {}

	virtual void draw(const GraphicsContext &gc) override;

	virtual std::string const className() const override { return "Dialog"; }
protected:
	virtual void MsgStart() override { /* TODO dialog intialisation */ }

};

class Shortcut : public Widget
{
private:
	int scancode;
	int keymod;
public:
	void Setup (int scancode, int keymod = 0);
	virtual std::string const className() const override { return "Shortcut"; }
	virtual bool MsgChar (int keycode, int modifiers) override;
	Shortcut(int scancode, int keymod = 0) : scancode(scancode), keymod(keymod) { setVisible(false); }
	static ComponentBuilder<Shortcut> build(int scancode, int keymod = 0);
};

class TabPanel : public Widget
{
public:
	void Attach(std::shared_ptr<Widget> widget, const char *title);
};

class CheckBox : public TextWidget
{
};

class RadioButton : public TextWidget
{
};

class ListItemString : public Widget
{
public:
	ListItemString(const char * val);
};

class ListBox : public Widget
{
public:
	void InsertItem(ListItemString *item, int pos = 0);
	void SetMultiSelect(bool val);
};

class ListBoxEx : public Widget
{
public:
	class Item
	{
	public:
		void InsertColumn() {/* TODO */}
		void SetText(const char *val, int i) { /* TODO */ }
	};
	void SetItem(int i, int j, const char *text);
	void InsertItem(ListBoxEx::Item *item, int pos = 0);
	void InsertColumn(const char *text, int, int, int);
};

// TODO: merge Accelerator, Input (former Button) and Shortcut into one.
// also combine with AnyKey
class Accelerator : public Widget
{
public:
	void Key(int scancode);
};

class EditBox : public TextWidget
{
public:
	void SetNumber(int number);
	int GetInt();
	void Setup (int x, int y, int  w, int h, int a, int b, const char *value, int c);
};

class ComboBox : public Widget
{
public:
	ListBox list;
	EditBox editBox;
};

class TextArea : public EditBox
{
public:
	void LoadLinesFromFile (const char *filename);
};

// TODO: figure if Accelerator, Input (former Button) and Shortcut can be merged into one.
class ShortCut : public Widget
{
};
class PanelSunken : public Widget
{
};

//TODO
#define FA_ARCH -1

class FileSelect : public Dialog
{
public:
	FileSelect(const char *question, const char *path, const char *filter, int flag);
	char *Popup(Dialog *parent);
};


#endif
