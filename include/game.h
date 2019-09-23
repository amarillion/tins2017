#ifndef GAME_H
#define GAME_H

#include "state.h"

class Metrics;

class Game : public State
{
public:
	virtual ~Game() {};
	virtual void update() override = 0;
	virtual void draw(const GraphicsContext &gc) override = 0;
	virtual void init() = 0;
	virtual void handleEvent(ALLEGRO_EVENT &event) override = 0;

	virtual void initGame() = 0;
	virtual bool hasSavedLevel() = 0;
	virtual void loadCurrentLevel() = 0;

	static std::shared_ptr<Game> newInstance(std::shared_ptr<Metrics> metrics);

	virtual std::string const className() const override { return "Game"; }
};


#endif
