#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include "resources.h"
#include "container.h"

class Engine : public Container {
protected:
	static Resources *resources;
	static bool debugMode;
public:
	virtual void init() = 0; // call once during startup
	static Resources* getResources() { return resources; }
	static bool isDebug() { return debugMode; }
	static std::shared_ptr<Engine> newInstance();

	enum { E_NONE = 0, E_RESUME, E_QUIT, E_BEFORE_QUIT, E_START, E_TOGGLE_FULLSCREEN };
};

#endif
