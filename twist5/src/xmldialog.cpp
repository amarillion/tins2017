#include "xmldialog.h"
#include <assert.h>
#include "dialog.h"
#include "panel.h"
#include "label.h"

using namespace std;
using namespace xdom;

int readFromXmlHelper (Container *dlg, DomNode &n, map <int, WidgetPtr> &widgets)
{
	// now add widgets
	vector<DomNode>::iterator w;
	for (w = n.children.begin(); w != n.children.end(); ++w)
	{
		shared_ptr<Widget> widget = XmlWidgetFactory::constructWidget(*w);
		if (widget)
		{
			dlg->add (widget);
			int val = widget->GetCallbackID();
			if (val >= 0)
				widgets.insert (pair<int, WidgetPtr> (val, widget));
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

int XmlWindow::readFromXml (xdom::DomNode &n)
{
	string s = n.attributes["title"];
	setTitle (s.c_str());

	setDimension(
		atoi(n.attributes["width"].c_str()),
		atoi(n.attributes["height"].c_str())
	);

	return readFromXmlHelper (getClient().get(), n, widgetMap);
}

int XmlDialog::readFromXml (xdom::DomNode &n)
{
	auto desktop = ClearScreen::build(BLUE /* TODO: use default skin desktop color */ ).get();
	add (desktop);
	int result = readFromXmlHelper (this, n, widgetMap);
	return result;
}

XmlHelper::~XmlHelper()
{
	// TODO - already handled by Container...
	/*
	list<Widget*>::iterator i;
	for (i = dynamicWidgets.begin(); i != dynamicWidgets.end(); ++i)
	{
		delete (*i);
	}
	*/
}

WidgetPtr XmlHelper::findWidgetById(int id)
{
	assert (widgetMap.find (id) != widgetMap.end());
	return widgetMap[id];
}

void XmlWidgetFactory::mapCommonAttributes(WidgetPtr w, DomNode &n)
{
	int left = atoi(n.attributes["left"].c_str());
	int top = atoi(n.attributes["top"].c_str());
	int width = atoi(n.attributes["width"].c_str());
	int height = atoi(n.attributes["height"].c_str());
	int id;
	if (n.attributes.find("id") == n.attributes.end())
		id = -1;
	else
		id = atoi(n.attributes["id"].c_str());
	string tooltiptext = n.attributes["tooltiptext"];
	w->setLayout(Layout::LEFT_TOP_W_H, left, top, width, height);
	w->SetTooltipText (tooltiptext.c_str());
	w->SetCallbackID(id);
}

WidgetPtr XmlWidgetFactory::buttonConstructor(DomNode &n)
{
	auto b = make_shared<Button>();
	b->SetText (n.attributes["text"].c_str());
	if ((n.attributes.find("exit") != n.attributes.end()) && n.attributes["exit"] == "true")
		b->MakeExit();
	if (n.attributes.find("id") != n.attributes.end())
	{
		int id = atoi(n.attributes["id"].c_str());
		b->setActionCode(id);
	}

	mapCommonAttributes(b, n);
	return b;
}

WidgetPtr XmlWidgetFactory::labelConstructor(DomNode &n)
{
	auto l = make_shared<Label>();
	l->SetText (n.attributes["text"].c_str());
	mapCommonAttributes(l, n);
	return l;
}

WidgetPtr XmlWidgetFactory::checkboxConstructor(DomNode &n)
{
	auto cb = make_shared<CheckBox>();
	cb->SetText (n.attributes["text"].c_str());
	mapCommonAttributes(cb, n);
	return cb;
}

WidgetPtr XmlWidgetFactory::radiobuttonConstructor(DomNode &n)
{
	auto rb = make_shared<RadioButton>();
	rb->SetText (n.attributes["text"].c_str());
	mapCommonAttributes(rb, n);
	return rb;
}

WidgetPtr XmlWidgetFactory::panelConstructor(DomNode &n)
{
	auto p = make_shared<Panel>();
	mapCommonAttributes(p, n);
	return p;
}

WidgetPtr XmlWidgetFactory::listboxConstructor(DomNode &n)
{
	auto lb = make_shared<ListBox>();
	lb->SetMultiSelect(true);
	mapCommonAttributes(lb, n);
	return lb;
}

WidgetPtr XmlWidgetFactory::listboxexConstructor(DomNode &n)
{
	auto lbe = make_shared<ListBoxEx>();
	mapCommonAttributes(lbe, n);
	return lbe;
}

WidgetPtr XmlWidgetFactory::acceleratorConstructor(xdom::DomNode &n)
{
	auto k = make_shared<Accelerator>();
	string keyname = n.attributes["key"];

	if (keyname == "KEY_PGUP") k->Key(ALLEGRO_KEY_PGUP);
	else if (keyname == "KEY_PGDN") k->Key(ALLEGRO_KEY_PGDN);
	else if (keyname == "KEY_LEFT") k->Key(ALLEGRO_KEY_LEFT);
	else if (keyname == "KEY_RIGHT") k->Key(ALLEGRO_KEY_RIGHT);
	else if (keyname == "KEY_UP") k->Key(ALLEGRO_KEY_UP);
	else if (keyname == "KEY_DOWN") k->Key(ALLEGRO_KEY_DOWN);
	return k;
}

WidgetPtr XmlWidgetFactory::comboboxConstructor(DomNode &n)
{
	auto cbb = make_shared<ComboBox>();
	cbb->list.InsertItem(new ListItemString("option 1"), 0);
	cbb->list.InsertItem(new ListItemString("option 2"), 1);
	cbb->list.InsertItem(new ListItemString("option 3"), 2);
	cbb->list.seth(100);
	cbb->editBox.SetFlag(D_READONLY);
	mapCommonAttributes(cbb, n);
	return cbb;
}

WidgetPtr XmlWidgetFactory::editboxConstructor(DomNode &n)
{
	auto eb = make_shared<EditBox>();
	eb->SetText (n.attributes["text"].c_str());
	mapCommonAttributes(eb, n);
	return eb;
}

void XmlWidgetFactory::registerWidget (string name, WidgetConstr wc)
{
	registeredWidgets.insert (pair<string, WidgetConstr>(name, wc));
}

WidgetPtr XmlWidgetFactory::constructWidget(DomNode &n)
{
	WidgetConstr wc = registeredWidgets[n.name];
	return wc(n);
}

void XmlWidgetFactory::init()
{
	registerWidget ("button", buttonConstructor);
	registerWidget ("label", labelConstructor);
	registerWidget ("checkbox", checkboxConstructor);
	registerWidget ("radiobutton", radiobuttonConstructor);
	registerWidget ("panel", panelConstructor);
	registerWidget ("listbox", listboxConstructor);
	registerWidget ("listboxex", listboxexConstructor);
	registerWidget ("combobox", comboboxConstructor);
	registerWidget ("editbox", editboxConstructor);
	registerWidget ("accelerator", acceleratorConstructor);
}

map <string, WidgetConstr> XmlWidgetFactory::registeredWidgets;
