#ifndef WINDOW_H
#define WINDOW_H

#include "dialog.h"

class Button;
class Panel;
class Label;

class Window : public Dialog
{
	std::shared_ptr<Panel> back;
	std::shared_ptr<Container> client;
	std::shared_ptr<Label> title;
	std::shared_ptr<Panel> titleBar;
	std::shared_ptr<Button> btnClose;

public:
	int Popup(IComponent *parent);

	void Resize(int w, int h);
	void Centre();
	// void HandleEvent (Widget &obj, int msg, int arg1, int arg2);
	void setTitle(const char *value);
	Window();
	virtual ~Window() {}
	std::shared_ptr<Container> getClient() { return client; }
};

#endif
