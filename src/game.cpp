#include <assert.h>

#include "game.h"
#include "color.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "engine.h"

using namespace std;

class GameImpl : public Game {
private:
public:
	virtual void initGame() override
	{
	}

	virtual void update() override;
	virtual void draw(const GraphicsContext &gc) override;
	virtual void handleEvent(ALLEGRO_EVENT &event) override;
	virtual void init() override;
	virtual ~GameImpl() {}
	GameImpl();

};

shared_ptr<Game> Game::newInstance()
{
	return make_shared<GameImpl>();
}

void GameImpl::init() {
}

GameImpl::GameImpl()
{
}

void GameImpl::update()
{
}

void GameImpl::handleEvent(ALLEGRO_EVENT &event)
{
	// handle some keyboard and mouse events here...
}

void GameImpl::draw(const GraphicsContext &gc)
{
	al_clear_to_color(BLACK);
	if (Engine::isDebug())
	{
		al_draw_text(Engine::getFont(), LIGHT_BLUE, 0, 0, ALLEGRO_ALIGN_LEFT, "DEBUG ON");
	}

	al_draw_text(Engine::getFont(), LIGHT_BLUE, getw() / 2, 100, ALLEGRO_ALIGN_CENTER, "Hello World");

}
