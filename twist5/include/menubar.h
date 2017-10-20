#ifndef _MENUBAR_H_

#include "container.h"

class Menu : public Container
{
public:
	virtual void Check(int pos) = 0;
	virtual void Uncheck(int pos) = 0;
	virtual void Add(const char *item, int msg) = 0;
	virtual void Add() = 0; // insert spacer;
	virtual void Add(const char *submenu, std::shared_ptr<Menu> menu) = 0;
	virtual TextWidget *GetItem(int index) = 0;
	virtual ~Menu() {}
	static ComponentBuilder<Menu> build(bool fMenuBar);

	virtual std::string const className() const override { return "Menu"; }
};


#endif
