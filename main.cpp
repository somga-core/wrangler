#include "teco/teco_engine.hpp"
#include "teco/teco_gui.hpp"
#include "teco/teco_tui.hpp"

#include <random>
#include <ctime>

#include "main_gameplay.cpp"

int main() {
	srand(time(0));

	teco::init(&main_gameplay::screen, "Wrangler");
	teco::init_gui();
	teco::mainloop_gui();

	return 0;
}

