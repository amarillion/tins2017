#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <allegro5/allegro.h>
#include "component.h"
#include "sound.h"
#include "timer.h"

#include <string>
#include <vector>
#include <map>

#ifdef USE_MONITORING
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
#endif

/**
 * Equivalent of mainLoop->getw().
 * see there.
 * <p>
 * Represents the logical screen size, the optimal screen size for which the game was designed to run.
 * Buffer or display size may be different because they may have a transformation on them.
 * <p>
 * For example, the game may (conservatively) be designed for a 640x480 screen resolution.
 * However, the desktop resolution is 1920x1080, and this is the size you'd get for
 * the buffer bitmap width or the display width. Because the transformation applies, you'd draw
 * to the buffer as though it was sized 640x480.
 */
#define MAIN_WIDTH MainLoop::getMainLoop()->getw()

/**
 * Equivalent of mainLoop->geth().
 * See MAIN_WIDTH
 */
#define MAIN_HEIGHT MainLoop::getMainLoop()->geth()

#define TICKS_FROM_MSEC(x) ((x) / MainLoop::getMainLoop()->getLogicIntervalMsec())
#define MSEC_FROM_TICKS(x) ((x) * MainLoop::getMainLoop()->getLogicIntervalMsec())

class RootComponent;

class MainLoop final : public IComponent, public Sound, public ITimer
{
private:
	ALLEGRO_BITMAP *buffer;
	ALLEGRO_EVENT_QUEUE *equeue;
	ALLEGRO_TIMER *logicTimer;
	ALLEGRO_DISPLAY *display;

	std::shared_ptr<RootComponent> rootPane;
	std::shared_ptr<IComponent> engine;

	ALLEGRO_PATH *localAppData;
	ALLEGRO_PATH *configPath;

	const char *configFilename;
	const char *title;
	const char *appname;
	
	int frame_count;
	int frame_counter;
	int last_fps;
	int lastUpdateMsec;

	enum ScreenMode { WINDOWED = 0, FULLSCREEN_WINDOW, FULLSCREEN };
	ScreenMode screenMode;

	// indicates whether buffer size is different from display size
	bool stretch;

	// If useFixedResolution is on, the buffer will be the same size as w, h.
	// if useFixedResolution is off, the buffer can be any size, but w, h will be used as the default window size if there is that flexibility.
	bool useFixedResolution = true;

/*
 //TODO - implement this with letterboxing
	bool useFixedAspectRatio = false;
	Point fixedAspectRatio;
 */

	// in smoke test mode: don't create display, just test loading resources.
	// smokeTest is in headless mode.
	bool smokeTest;
	
	void getFromConfig(ALLEGRO_CONFIG *config);
	void getFromArgs(int argc, const char *const *argv);

	int logicIntervalMsec;

	static MainLoop *instance;
	int initDisplay();

	/** initialises default skin */
	void initSkin();

	std::vector<std::string> options;

	bool checkMessages();
#ifdef USE_MONITORING
	Clock::time_point t0; // time since start of program
	Clock::time_point t1; // time since start of update loop
	Clock::time_point t2; // time since start of phase
	void logStartTime(const std::string &phase);
	void logEndTime(const std::string &phase);
	std::map<std::string, long> sums;
	std::map<std::string, int> counts;
#endif
protected:
	ALLEGRO_CONFIG *config;
	bool fpsOn;
public:
	bool isSmokeTest() { return smokeTest; }
	void adjustMickey(int &x, int &y);

	/** return vector of unhandled command-line arguments */
	std::vector<std::string> &getOpts() { return options; }

	ALLEGRO_CONFIG *getConfig() { return config; }
	
	//TODO: DEPRECATED. use getMsecCounter
	int getCounter () { return al_get_timer_count(logicTimer) * logicIntervalMsec; }
	int getMsecCounter () { return al_get_timer_count(logicTimer) * logicIntervalMsec; }
	void setFpsOn (bool value) { fpsOn = value; }

	MainLoop (IComponent *_engine, const char *configFilename, const char *title, int _bufw = 640, int _bufh = 480);
	MainLoop ();

	/**
	 * indicate that the game is
	 *
	 * a. designed for a certain fixed resolution,
	 * OR
	 * b. adapts to any resolution it's given
	 *
	 * In case a), a fixed size buffer will be created and stretched / transformed to match the actual display resolution.
	 *      (possibly letterboxing may be used to maintain aspect ratio depending on the setFixedAspectRatio setting)
	 *
	 * In case b), game routines are expected to handle a variety of resolutions dynamically.
	 *      w, h are used for the default window size size in windowed mode.
	 *
	 */
	MainLoop &setFixedResolution (bool fixed);

	//TODO: not yet implemented
//	MainLoop &setFixedAspectRatio (bool fixed, int x = 4, int y = 3);

	MainLoop &setTitle(const char *_title);
	MainLoop &setAppName(const char *_appname);
	MainLoop &setConfigFilename(const char *_configFilename);
	MainLoop &setEngine(std::shared_ptr<IComponent> _engine);
	MainLoop &setLogicIntervalMsec (int value) { logicIntervalMsec = value; return *this; }

	IComponentPtr getEngine() { return engine; }
	/**
	 * returns 0 on success, 1 on failure
	 */
	int init(int argc, const char *const *argv);
	void run();	
	virtual ~MainLoop();
	
	virtual void parseOpts(std::vector<std::string> &opts) {};
	int getLogicIntervalMsec () { return logicIntervalMsec; }

	void toggleWindowed();

	/**
	 * If parent != null, any command messages coming from the popupWindow
	 * are passed to the parent.
	 */
	void popup (IComponentPtr popupWindow, IComponentPtr parent = nullptr);

	void pumpMessages();

	static MainLoop *getMainLoop();
};

#endif
