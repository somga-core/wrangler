#include "teco.hpp"
#include <random>
#include <ctime>

enum {
    BULB = 0,
    GLASSES,
    LENS,
    MONSTER
};

int ITEMS_COUNT = 4;
int HOTBAR_WIDTH_IN_ITEMS = 10;

int ITEM_WIDTH_IN_SYMBOLS = 16;
int ITEM_HEIGHT_IN_SYMBOLS = 6;

int SHIP_WIDTH_IN_SYMBOLS = 14;
int SHIP_HEIGHT_IN_SYMBOLS = 5;

const int HARBOR_X = 1;
const int HARBOR_Y = 1;

const int SEA_HEIGHT_IN_SYMBOLS = 42;
const int SEA_WIDTH_IN_SYMBOLS = 192;

const int SEA_X_IN_SYMBOLS = 1;
const int SEA_Y_IN_SYMBOLS = 1;

int HOTBAR_HEIGHT_IN_SYMBOLS = teco::HEIGHT_IN_SYMBOLS - SEA_HEIGHT_IN_SYMBOLS;

std::vector<int> hotbar;

class Ship {
public:
    teco::Sprite *sprite;
    
    int is_turned_right = 0;

    float speed_x;
    float speed_y;
    float speed;

    float x = 0;
    float y = 0;

    int left_point_y;
    int right_point_y;

    bool is_wrangled = false;

    Ship() {
        sprite = new teco::Sprite {
            0, 0,
            std::vector<teco::Animation> {
                teco::Animation {
                    std::vector<teco::Source> {
                        teco::Source {
                            "./assets/sources/ships/schooner/left/df.tcsb",
                            "./assets/sources/ships/schooner/left/df.tccl"
                        }
                    }
                },
                teco::Animation {
                    std::vector<teco::Source> {
                        teco::Source {
                            "./assets/sources/ships/schooner/right/df.tcsb",
                            "./assets/sources/ships/schooner/right/df.tccl"
                        }
                    }
                }
            }
        };

        left_point_y = SEA_Y_IN_SYMBOLS + rand() % SEA_HEIGHT_IN_SYMBOLS;
        right_point_y = SEA_Y_IN_SYMBOLS + rand() % SEA_HEIGHT_IN_SYMBOLS;

        speed = rand() % 100 + 50;
        speed /= 150;

        float speed_coefficent = speed / hypot(left_point_y - right_point_y, SEA_WIDTH_IN_SYMBOLS);

        speed_y = (right_point_y - left_point_y) * speed_coefficent;
        speed_x = (SEA_WIDTH_IN_SYMBOLS) * speed_coefficent;

        is_turned_right = rand() % 2;

        if (is_turned_right) {
            sprite->play_animation(1);
            y = left_point_y;
            x = SEA_X_IN_SYMBOLS - SHIP_WIDTH_IN_SYMBOLS;
        } else {
            sprite->play_animation(0);
            speed_x = -speed_x;
            speed_y = -speed_y;
            y = right_point_y;
            x = SEA_X_IN_SYMBOLS + SEA_WIDTH_IN_SYMBOLS;
        }
    }

    void tick() {
        if (!is_wrangled) {
            x += speed_x;
            y += speed_y;
        }

        sprite->x = x;
        sprite->y = y;
    }
};

class Item {

};

class Background {

};

class LightBeam {

};

std::vector<Ship*> ships;

teco::Sprite *background;

teco::Sprite *waves;

void tick_tock() {
    if (teco::is_key_pressed(SDLK_BACKSPACE))
        teco::exit();

    for (Ship *ship : ships) {
        ship->tick();
    }
}

void init_objects() {
    waves = new teco::Sprite {
        1, 1,
        std::vector<teco::Animation> {
            teco::Animation {
                std::vector<teco::Source> {
                    teco::Source {
                        "./assets/sources/gui/waves/fr1.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr2.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr3.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr4.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr5.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr6.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr7.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr8.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    },
                    teco::Source {
                        "./assets/sources/gui/waves/fr9.tcsb",
                        "./assets/sources/gui/waves/df.tccl"
                    }
                },
                teco::LOOPING,
                4
            }
        }
    };

    for (int ship_index = 0; ship_index < 1; ship_index++){
        ships.push_back(new Ship{});
    }

    background = new teco::Sprite {
        0, 0,
        std::vector<teco::Animation> {
            teco::Animation {
                std::vector<teco::Source> {
                    teco::Source {
                        "./assets/sources/gui/background/df.tcsb",
                        "./assets/sources/gui/background/df.tccl"
                    }
                }
            }
        }
    };
}

int main() {
    srand(time(0));

    teco::init(tick_tock, teco::TUI, 60, 20, 8, "Wrangler");

    init_objects();

    teco::mainloop();
    return 0;
}
