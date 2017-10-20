#include "dialog.h"
#include "color.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "button.h"
#include "mainloop.h"
#include "panel.h"

using namespace std;

void MASSetMouseCursor (Cursor &val) { /* TODO */ }

void Dialog::Add (Widget &widget) { /* TODO */ } //TODO: harmonise with Container->add
void Dialog::Remove (Widget &widget) { /* TODO */ } //TODO: harmonise with Container->remove

void Dialog::Close() { kill(); /* TODO */ }

void Dialog::draw(const GraphicsContext &gc)
{
	Container::draw(gc);
}


void Dialog::Execute() { /* TODO */ }
void Dialog::ChangeResolution(int gfxMode, bool fullscreen, int w, int h, int colorDepth, Dialog *dlg) { /* TODO */ }

void TextWidget::SetText(const char *val) { text = val; }

void Shortcut::Setup (int scancode, int keymod)
{
	this->scancode = scancode;
	this->keymod = keymod;
}

bool Shortcut::MsgChar(int keycode, int modifiers)
{
	if (keycode == this->scancode && modifiers == this->keymod) {
		pushMsg(MSG_ACTIVATE);
		return true;
	}
	return false;
}

void TabPanel::Attach(shared_ptr<Widget> widget, const char *title) { /* TODO */ }

ListItemString::ListItemString(const char * val) { /* TODO */ }

void ListBox::InsertItem(ListItemString *item, int pos) { /* TODO */ }
void ListBox::SetMultiSelect(bool val) { /* TODO */ }

void ListBoxEx::SetItem(int i, int j, const char *text) { /* TODO */ }
void ListBoxEx::InsertItem(ListBoxEx::Item *item, int pos) { /* TODO */ }
void ListBoxEx::InsertColumn(const char *text, int, int, int) { /* TODO */ }

void Accelerator::Key(int scancode) { /* TODO */ }


void EditBox::SetNumber(int number) { /* TODO */ }
int EditBox::GetInt() { return -1; /* TODO */ }
void EditBox::Setup (int x, int y, int  w, int h, int a, int b, const char *value, int c)
{ setLocation(x, y, w, h); /* TODO other params */ }

void TextArea::LoadLinesFromFile (const char *filename) { /* TODO */ }

FileSelect::FileSelect(const char *question, const char *path, const char *filter, int flag) { /* TODO */ }
char *FileSelect::Popup(Dialog *parent) { return NULL; /* TODO */ }

ComponentBuilder<Shortcut> Shortcut::build(int scancode, int keymod)
{
	return ComponentBuilder<Shortcut>(make_shared<Shortcut>(scancode, keymod));
}
