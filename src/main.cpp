#include "engine.h"
#include "mainloop.h"

using namespace std;

int main(int argc, const char *const *argv)
{
	MainLoop mainloop = MainLoop();
	auto engine = make_shared<Engine>();

	mainloop
		.setEngine(engine)
		.setTitle("TINS 2017!")
		.setAppName("tins17");

	mainloop.setPreferredResolution(960, 600);

	mainloop.init(argc, argv);
	engine->init();
	mainloop.run();
}
