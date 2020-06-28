#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "container.h"

class Resources;

class Engine : public Container {
protected:
	static std::shared_ptr<Resources> resources;
	static bool debugMode;
public:
	virtual void logAchievement(const std::string &id) = 0;
	virtual void init() = 0; // call once during startup
	static std::shared_ptr<Resources> getResources() { return resources; }
	static bool isDebug() { return debugMode; }
	static std::shared_ptr<Engine> newInstance();

	enum { E_NONE = 0, E_RESUME, E_QUIT, E_BEFORE_QUIT, E_START, E_LOAD, E_TOGGLE_FULLSCREEN };
};

#endif
