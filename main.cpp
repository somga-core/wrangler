#include "teco.hpp"
#include <random>

enum {
	NOTHING,
    BULB,
    GLASSES,
    LENS,
    MONSTER,
	ITEMS_COUNT
};

const int HOTBAR_WIDTH_IN_ITEMS = 10;

const int ITEM_WIDTH_IN_SYMBOLS = 16;
const int ITEM_HEIGHT_IN_SYMBOLS = 6;

const int SHIP_WIDTH_IN_SYMBOLS = 14;
const int SHIP_HEIGHT_IN_SYMBOLS = 5;

const int SEA_HEIGHT_IN_SYMBOLS = 48;
const int HOTBAR_HEIGHT_IN_SYMBOLS = teco::HEIGHT_IN_SYMBOLS - SEA_HEIGHT_IN_SYMBOLS;

int position_in_hotbar = 0;

std::vector<int> hotbar;

teco::Sprite background = teco::Sprite {
    0, 0,
    std::vector<teco::Animation> {
        teco::Animation {
            std::vector<teco::Source> {
                teco::Source {
                    "./assets/sources/gui/background/df.tcsb",
                    "./assets/sources/empty.tccl"
                }
            }
        }
    }
};

teco::Sprite hand {
	1, 48,
	std::vector<teco::Animation> {
		teco::Animation {
			std::vector<teco::Source> {
				teco::Source {
					"assets/sources/gui/hand.tcsb",
					"assets/sources/gui/hand.tccl"
				}
			}
		}
	}
};

teco::Sprite ship = teco::Sprite {
    12, 10,
    std::vector<teco::Animation> {
        teco::Animation {
            std::vector<teco::Source> {
                teco::Source {
                    "./assets/sources/ships/schooner/right/df.tcsb",
                    "./assets/sources/empty.tccl"
                }
            }
        }
    }
};


class Ship {
public:
    // teco::Sprite sprite = teco::Sprite {
    //     0, 0,
    //     std::vector<teco::Animation> {
    //         teco::Animation {
    //             std::vector<teco::Source> {
    //                 teco::Source {
    //                     "./assets/sources/ships/schooner/right/df.tcsb",
    //                     "./assets/sources/empty.tccl"
    //                 }
    //             }
    //         }
    //     },
    //     0
    // };
    
    // int is_turned_right = 0;
    // float speed_x;
    // float speed_y;
    // float x = 0;
    // float y = 0;

    // bool is_wrangled = false;

    Ship() {
        // is_turned_right = rand() % 2;

        // speed_y = rand() % 100 - 50;
        // speed_y /= 100;

        // speed_x = rand() % 100;
        // speed_x /= 50;

        // y = rand() % SEA_HEIGHT_IN_SYMBOLS;

        // if (is_turned_right) {
        //     sprite.animations[0].sources[0].load_symbols("assets/sources/ships/schooner/right/df.tcsb");
        // } else {
        //     sprite.animations[0].sources[0].load_symbols("assets/sources/ships/schooner/left/df.tcsb");
        //     speed_x = -speed_x;
        //     x = teco::WIDTH_IN_SYMBOLS * 4;
        // }

        // y = 20;
        // x = 80;
    }

    // void tick() {
    //     // if (!is_wrangled) {
    //     //     x += speed_x;
    //     //     y += speed_y;
    //     // }

    //     // std::cout << speed_x << std::endl;
    //     // std::cout << speed_y << std::endl;

    //     // sprite.x = 20;
    //     // sprite.y = 80;

    //     // std::cout << sprite.x << std::endl;
    //     // std::cout << sprite.y << std::endl;
    // }
};

std::vector<Ship*> ships;

class Item {

};

class Background {

};

class LightBeam {

};

void process_key_presses() {
    if (teco::is_key_pressed(SDLK_BACKSPACE)) {
        teco::exit();
	}
    else if (teco::is_key_pressed(SDLK_q) and position_in_hotbar < HOTBAR_WIDTH_IN_ITEMS) {
        position_in_hotbar++;
    }
    else if (teco::is_key_pressed(SDLK_e) and position_in_hotbar > 0) {
        position_in_hotbar--;
    }
    else if (teco::is_key_pressed(SDLK_f) and hotbar[position_in_hotbar] != NOTHING) {
        switch (hotbar[position_in_hotbar]) {
            case BULB:
                break;
            case GLASSES:
                break;
            case LENS:
                break;
            case MONSTER:
                break;
        }
    }
}

void tick_tock() {
	process_key_presses();

    ship.x++;

    // for (Ship *ship : ships) {
    //     ship->tick();
    // }
}

int main() {
    srand(100);

    teco::init(tick_tock, teco::TUI, 60, 20, 8);

    // for (int ship_index = 0; ship_index < 1; ship_index++){
    //     Ship ship = Ship {};`
    //     ships.push_back(&ship);
    // }

    teco::mainloop();
    return 0;
}
