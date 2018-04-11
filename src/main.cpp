#include "engine.h"
#include "mainloop.h"

using namespace std;

int main(int argc, const char *const *argv)
{
	MainLoop mainloop = MainLoop();
	auto engine = Engine::newInstance();

	mainloop
		.setEngine(engine)
		.setTitle("Peppy Protein Puzzle")
		.setAppName("peppy");

	mainloop.setPreferredGameResolution(960, 600);

	mainloop.init(argc, argv);
	engine->init();
	mainloop.run();
}
