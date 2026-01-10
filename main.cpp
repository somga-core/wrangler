#include "teco/teco_gui.hpp"

#include <random>
#include <ctime>
#include <vector>

#include "main_gameplay.cpp"

int main() {
	srand(time(0));

	teco::init(&main_gameplay::main_screen, "Wrangler");
	teco::init_gui();
	teco::mainloop_gui();

	return 0;
}

