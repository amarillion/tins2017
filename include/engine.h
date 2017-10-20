#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include "resources.h"
#include "container.h"
#include "menubase.h"

class Engine : public Container {

private:
	static Resources *resources;
	static ALLEGRO_FONT *font;
	static bool debugMode;
	void initMenu();
	MenuListPtr mMain;
public:
	enum { E_NONE = 0, E_RESUME, E_QUIT, E_START, E_TOGGLE_FULLSCREEN };
	std::shared_ptr<Game> game;

	virtual void draw(const GraphicsContext &gc) {
		focus->draw(gc);
	}

	virtual ~Engine();
	virtual void handleEvent(ALLEGRO_EVENT &event) override;
	virtual void handleMessage(std::shared_ptr<IComponent> src, int msg) override;

	static bool isDebug()
	{
		return debugMode;
	}

	static Resources *getResources() { return resources; }

	static ALLEGRO_FONT *getFont() { return font; }
	//TODO: Engine has series of global accessors including getFont(), getResources(), and isDebug() flag.

	void init();
};

#endif
