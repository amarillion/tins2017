#include "engine.h"
#include "util.h"
#include "mainloop.h"
#include "menubase.h"
#include "DrawStrategy.h"

Resources *Engine::resources = NULL;
ALLEGRO_FONT *Engine::font = NULL;
bool Engine::debugMode = false;

using namespace std;

void Engine::init()
{
	resources = new Resources();

	resources->addDir("data");

	font = resources->getFont("DejaVuSans_16");
	if (!font) {
		allegro_message("Error loading font.\n");
		exit(1);
	}

	setFont(font);

	game = Game::newInstance();
	game->init();
	add(game, Container::FLAG_SLEEP);

	initMenu();

	setFocus(mMain);
}

void Engine::initMenu()
{
	mMain = MenuBuilder(this, NULL)
		.push_back (make_shared<ActionMenuItem> (E_START, "Start", "Start a new game"))
		.push_back (make_shared<ActionMenuItem> (E_QUIT, "Quit", "Exit"))
		.push_back (make_shared<ActionMenuItem> (E_TOGGLE_FULLSCREEN, "Toggle Fullscreen", "Switch fullscreen / windowed mode"))
		.build();
	mMain->setFont(font);
	mMain->add(ClearScreen::build(BLUE).get(), Container::FLAG_BOTTOM);
	add (mMain);
}

Engine::~Engine() {
	delete resources; resources = NULL;
}

void Engine::handleEvent(ALLEGRO_EVENT &event)
{
	//TODO: implement F11 as a child of mainloop that send action events on press
#ifdef DEBUG
	if (event.type == ALLEGRO_EVENT_KEY_CHAR &&
		event.keyboard.keycode == ALLEGRO_KEY_F11)
	{
		debugMode = !debugMode;
		return; // consume event
	}
#endif
	Container::handleEvent(event);
}


void Engine::handleMessage(shared_ptr<IComponent> src, int event)
{
	switch (event)
	{
	case E_RESUME:
		setFocus (game);
		break;
	case E_TOGGLE_FULLSCREEN:
		MainLoop::getMainLoop()->toggleWindowed();
		break;
	case E_START:
		game->initGame();
		setFocus(game);
		break;
	case E_QUIT:
		pushMsg(MSG_CLOSE);
		break;
	}
}
