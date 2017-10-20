#ifndef XMLDLG_H
#define XMLDLG_H

//#include <MASkinG.h>
#include <list>
#include "dom.h"
#include "dialog.h"
#include "button.h"
#include "window.h"

class XmlHelper
{
public:
	virtual ~XmlHelper();
protected:
	std::map <int, WidgetPtr> widgetMap;
	WidgetPtr findWidgetById(int id);

};

class XmlWindow : public Window, public XmlHelper
{
public:
	XmlWindow() : Window(), XmlHelper() {}
protected:
	int readFromXml (xdom::DomNode &n);
};

class XmlDialog : public Dialog, public XmlHelper
{
public:
	XmlDialog() : Dialog(), XmlHelper() {}
protected:
	int readFromXml (xdom::DomNode &n);

};


typedef WidgetPtr(*WidgetConstr)(xdom::DomNode &n);

class XmlWidgetFactory
{
	private:
	static std::map <std::string, WidgetConstr> registeredWidgets;


	// constructors for all the built-in types.
	static WidgetPtr buttonConstructor(xdom::DomNode &n);
	static WidgetPtr labelConstructor(xdom::DomNode &n);
	static WidgetPtr checkboxConstructor(xdom::DomNode &n);
	static WidgetPtr radiobuttonConstructor(xdom::DomNode &n);
	static WidgetPtr panelConstructor(xdom::DomNode &n);
	static WidgetPtr listboxConstructor(xdom::DomNode &n);
	static WidgetPtr listboxexConstructor(xdom::DomNode &n);
	static WidgetPtr comboboxConstructor(xdom::DomNode &n);
	static WidgetPtr editboxConstructor(xdom::DomNode &n);
	static WidgetPtr acceleratorConstructor(xdom::DomNode &n);

	public:
	static void mapCommonAttributes(WidgetPtr, xdom::DomNode &n);
	/**
	register a callback for constructing a certain widget.
	for example:
	    XmlWidgetFactory::registerWidget("button", XmlWidgetFactory::buttonConstructor)

	This will register the constructor for buttons. However, you don't need to call
	this yourself for all the built-in widgets of masking, all you need for that
	is call XmlWidgetFactory::init(); This function is provided for registering
	your own widgets.
	*/
	static void registerWidget (std::string name, WidgetConstr wc);

	/**
	construct a widget from an xml fragment, by delegating to the correct
	registered widget constructor.
	*/
	static WidgetPtr constructWidget(xdom::DomNode &n);

	/**
	must be called before calling constructWidget.
	this will register all default constructors
	*/
	static void init();
};

#endif

