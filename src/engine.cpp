#include "engine.h"
#include "util.h"
#include "mainloop.h"
#include "menubase.h"
#include "DrawStrategy.h"
#include "updatechecker.h"
#include "version.h"
#include "fileutil.h"

using namespace std;

class EngineImpl : public Engine {
private:
	MenuScreenPtr mMain;
	MenuItemPtr menuLoad;
	std::shared_ptr<AnimComponent> bunny;
	shared_ptr<UpdateChecker> updates;
public:
	std::shared_ptr<Game> game;

	virtual void draw(const GraphicsContext &gc) {
		focus->draw(gc);
	}

	static bool isDebug()
	{
		return debugMode;
	}

	void init() override
	{
		resources = new Resources();

		if (!(
			resources->addFiles("data/*.ttf") &&
			resources->addFiles("data/*.png") &&
			resources->addFiles("data/*.xml") //&&
	//			resources->addFiles("data/*.ogg") &&
	//		resources->addFiles("data/*.xm") &&
	//		resources->addFiles("data/*.wav")
			))
		{
			allegro_message ("Error while loading resources!\n", resources->getErrorMsg());
			assert(false);
		}

		resources->addDir("data");

		auto font = resources->getFont("DejaVuSans_16");
		if (!font) {
			allegro_message("Error loading font.\n");
			exit(1);
		}

		setFont(font);

		string cacheDir = Path::getUserSettingsPath().toString(ALLEGRO_NATIVE_PATH_SEP);
		updates = UpdateChecker::newInstance(cacheDir, APPLICATION_VERSION, Engine::E_QUIT);
	 	add (updates, Container::FLAG_SLEEP);
	 	updates->start_check_thread("peppy", "en");

		game = Game::newInstance();
		game->init();
		add(game, Container::FLAG_SLEEP);

		initMenu();

		setFocus(mMain);
		if (game->hasSavedLevel()) {
			mMain->setSelectedItem(menuLoad);
		}
	}

	void initMenu()
	{
		menuLoad = make_shared<ActionMenuItem> (E_LOAD, "Continue game", "Continue where you left off");
		mMain = MenuBuilder(this, NULL)
			.push_back (make_shared<ActionMenuItem> (E_START, "Start game", "Start a new game"))
			.push_back (menuLoad)
			.push_back (make_shared<ActionMenuItem> (E_BEFORE_QUIT, "Quit", "Exit"))
			.push_back (make_shared<ActionMenuItem> (E_TOGGLE_FULLSCREEN, "Toggle Fullscreen", "Switch fullscreen / windowed mode"))
			.build();
		mMain->setFont(sfont);
		mMain->add(ClearScreen::build(BLACK).get(), Container::FLAG_BOTTOM);
		bunny = AnimComponent::build(resources->getAnim("intro")).layout(Layout::LEFT_TOP_W_H, 50, 50, 300, 200).get();
		mMain->add(bunny);
		add (mMain);

		if (!game->hasSavedLevel()) {
			menuLoad->setEnabled(false);
		}
	}

	~EngineImpl() {
		delete resources; resources = NULL;
	}

	void handleEvent(ALLEGRO_EVENT &event) override {
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

	void handleMessage(shared_ptr<IComponent> src, int event)
	{
		switch (event)
		{
		case E_RESUME:
			setFocus (game);
			break;
		case E_TOGGLE_FULLSCREEN:
			MainLoop::getMainLoop()->toggleWindowed();
			break;
		case E_LOAD:
			game->loadCurrentLevel();
			// intentional fall-through
		case E_START:
			bunny->setState(1);
			mMain->setAwake(false);
			MainLoop::getMainLoop()->playSample(resources->getSample("sound_scared"));
			add (Timer::build(100, [=]() {
				game->initGame();
				setFocus(game);
			}).get());
			break;
		case E_BEFORE_QUIT:
			setFocus(updates);
			break;
		case E_QUIT:
			pushMsg(MSG_CLOSE);
			break;
		}
	}
};

Resources *Engine::resources = NULL;
bool Engine::debugMode = false;

shared_ptr<Engine> Engine::newInstance()
{
	return make_shared<EngineImpl>();
}
