#include "teco.hpp"
#include <random>
#include <ctime>

enum {
	NOTHING,
    BULB,
    GLASSES,
    LENS,
    CHUDISHCHE,
	ITEMS_COUNT
};

const int HOTBAR_WIDTH_IN_ITEMS = 10;

const int ITEM_WIDTH_IN_SYMBOLS = 16;
const int ITEM_HEIGHT_IN_SYMBOLS = 6;

const int SHIP_WIDTH_IN_SYMBOLS = 14;
const int SHIP_HEIGHT_IN_SYMBOLS = 5;

const int HARBOR_X = 1;
const int HARBOR_Y = 1;

const int SEA_HEIGHT_IN_SYMBOLS = 46;
const int SEA_WIDTH_IN_SYMBOLS = 117;

const int SEA_X_IN_SYMBOLS = 1;
const int SEA_Y_IN_SYMBOLS = 1;

const int BULB_FACTOR = 5;

const int HOTBAR_HEIGHT_IN_SYMBOLS = teco::HEIGHT_IN_SYMBOLS - SEA_HEIGHT_IN_SYMBOLS;
int position_in_hotbar = 0;

int hotbar[HOTBAR_WIDTH_IN_ITEMS] = {0, 0, 1, 2, 1, 0};

int forfeit_ships = 0;
int score = 0;

teco::Sprite waves {
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



class LightBeam {
public:
	teco::Sprite *sprite;

	LightBeam() {
		sprite = new teco::Sprite {
			1, 1,
			std::vector<teco::Animation> {
				teco::Animation {
					std::vector<teco::Source> {
						teco::Source {
							"assets/sources/light_beam/df.tcsb",
							"assets/sources/light_beam/df.tccl"
						}
					}
				}
			}
		};
	}

	~LightBeam() {
		delete sprite;
	}
};

LightBeam light_beam;

class Ship {
public:
    teco::Sprite *sprite;
    
    int is_turned_right = 0;
	int is_correct = 1;
	int wrangle_counter = 0;
	int ready_to_delete = 0;

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
                            "./assets/sources/ships/schooner/left/cl.tcsb",
                            "./assets/sources/ships/schooner/left/cl.tccl"
                        }
                    }
                },
                teco::Animation {
                    std::vector<teco::Source> {
                        teco::Source {
                            "./assets/sources/ships/schooner/right/cl.tcsb",
                            "./assets/sources/ships/schooner/right/cl.tccl"
                        }
                    }
                }
            }
        };

        left_point_y = SEA_Y_IN_SYMBOLS + rand() % (SEA_HEIGHT_IN_SYMBOLS-SHIP_HEIGHT_IN_SYMBOLS);
        right_point_y = SEA_Y_IN_SYMBOLS + rand() % (SEA_HEIGHT_IN_SYMBOLS-SHIP_HEIGHT_IN_SYMBOLS);

        speed = rand() % 100 + 50;
        speed /= 450;

        float speed_coefficent = speed / hypot(left_point_y - right_point_y, SEA_WIDTH_IN_SYMBOLS);

        speed_y = (right_point_y - left_point_y) * speed_coefficent;
        speed_x = (SEA_WIDTH_IN_SYMBOLS) * speed_coefficent;

        is_turned_right = rand() % 2;
		is_correct = rand() % 2;

        if (is_turned_right) {
            sprite->play_animation(1);
            y = left_point_y;
            x = SEA_X_IN_SYMBOLS;
        } else {
            sprite->play_animation(0);
			speed_x = -speed_x;
            speed_y = -speed_y;
            y = right_point_y;
            x = SEA_X_IN_SYMBOLS + SEA_WIDTH_IN_SYMBOLS - SHIP_WIDTH_IN_SYMBOLS;
        }
    }

	~Ship() {
		delete sprite;
	}

    void tick() {
		if (
			//(light_beam.sprite->x <= x or light_beam.sprite->x+7 <= x + SHIP_WIDTH_IN_SYMBOLS) and
			//(light_beam.sprite->y <= y or light_beam.sprite->y+5 <= y + SHIP_HEIGHT_IN_SYMBOLS)
			!(light_beam.sprite->x+7 < SHIP_WIDTH_IN_SYMBOLS or x+SHIP_WIDTH_IN_SYMBOLS < light_beam.sprite->x or
			light_beam.sprite->y+5 < SHIP_HEIGHT_IN_SYMBOLS or y+SHIP_HEIGHT_IN_SYMBOLS < light_beam.sprite->y)
			//(light_beam.sprite->x <= x or
		) {
			is_wrangled = true;
			std::cout << "rrrrrrrr" << std::endl;
		}
		else {
			is_wrangled = false;
		}

		if (is_wrangled) {
			wrangle_counter++;
		}

		if (
			((is_turned_right and x < 104) or (!is_turned_right and x > 2)) and
			((speed_y > 0 and y > 2) or (speed_y <= 0 and y < 40))
		) {
			x += speed_x;
			y += speed_y;
		}
		else {
			//delete
			ready_to_delete = 1;
		}
		
        sprite->x = x;
        sprite->y = y;

		if (wrangle_counter >= 100) {
			ready_to_delete = 1;
			score++;
			wrangle_counter = 0;
		}
    }
};

std::vector<Ship*> ships;

class Item {
public:
	teco::Sprite *sprite;
	int type;
	
	Item(int);

	void change(int _type) {
		type = _type;
		switch (type) {
			case NOTHING:
				sprite->animations = std::vector<teco::Animation> {
					teco::Animation {
						std::vector<teco::Source> {
						teco::Source {
							"assets/sources/empty.tcsb",
							"assets/sources/empty.tccl"
							}
						}
					}
				};
				break;
			case BULB:
				sprite->animations = std::vector<teco::Animation> {
					teco::Animation {
						std::vector<teco::Source> {
						teco::Source {
							"assets/sources/items/bulb/df.tcsb",
							"assets/sources/items/bulb/df.tccl"
							}
						}
					}
				};
				break;
			case GLASSES:
				sprite->animations = std::vector<teco::Animation> {
					teco::Animation {
						std::vector<teco::Source> {
						teco::Source {
							"assets/sources/items/glasses/df.tcsb",
							"assets/sources/items/glasses/df.tccl"
							}
						}
					}
				};
				break;
			case LENS:
				sprite->animations = std::vector<teco::Animation> {
					teco::Animation {
						std::vector<teco::Source> {
						teco::Source {
							"assets/sources/items/lens/df.tcsb",
							"assets/sources/items/lens/df.tccl"
							}
						}
					}
				};
				break;
			case CHUDISHCHE:
				sprite->animations = std::vector<teco::Animation> {
					teco::Animation {
						std::vector<teco::Source> {
						teco::Source {
							"assets/sources/items/chudishche/df.tcsb",
							"assets/sources/items/chudishche/df.tccl"
							}
						}
					}
				};
				break;
		}
	}

	~Item() {
		delete sprite;
	}
};

Item *item_sprites[HOTBAR_WIDTH_IN_ITEMS] = {
	new Item(0),
	new Item(1),
	new Item(2),
	new Item(3),
	new Item(4),
	new Item(5),
	new Item(6),
	new Item(7),
	new Item(8),
	new Item(9),
};

Item::Item(int item_index) {
	sprite = new teco::Sprite {
		1 + 17*item_index, 48,
		std::vector<teco::Animation> {
			teco::Animation {
				std::vector<teco::Source> {
					teco::Source {
						"assets/sources/empty.tcsb",
						"assets/sources/empty.tccl"
					}
				}
			}
		}
	};
}

class Background {

};

void process_key_presses();
void tick_tock();


teco::Sprite background {
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

int main() {
    srand(time(0));

    teco::init(tick_tock, teco::TUI, 60, 20, 8, "Wrangler");

    for (int ship_index = 0; ship_index < 7; ship_index++){
        ships.push_back(new Ship{});
    }

    teco::mainloop();
    return 0;
}

void process_key_presses() {
    if (teco::is_key_pressed(SDLK_BACKSPACE)) {
        teco::exit();
	}
	else if (teco::is_key_pressed(SDLK_w) and light_beam.sprite->y > SEA_X_IN_SYMBOLS) {
		light_beam.sprite->y-=3;
	}
	else if (teco::is_key_pressed(SDLK_a) and light_beam.sprite->x > SEA_Y_IN_SYMBOLS) {
		light_beam.sprite->x-=3;
	}
	else if (teco::is_key_pressed(SDLK_s) and light_beam.sprite->y < SEA_Y_IN_SYMBOLS + SEA_HEIGHT_IN_SYMBOLS - 5 /*42*/) {
		light_beam.sprite->y+=3;
	}
	else if (teco::is_key_pressed(SDLK_d) and light_beam.sprite->x < SEA_X_IN_SYMBOLS + SEA_WIDTH_IN_SYMBOLS - 9 /*109*/) {
		light_beam.sprite->x+=3;
	}
    else if (teco::is_key_pressed(SDLK_q) and position_in_hotbar > 0) {
        position_in_hotbar--;
    }
    else if (teco::is_key_pressed(SDLK_e) and position_in_hotbar < HOTBAR_WIDTH_IN_ITEMS-1) { 
        position_in_hotbar++;
    }
    else if (teco::is_key_pressed(SDLK_f) and hotbar[position_in_hotbar] != NOTHING) {
        switch (hotbar[position_in_hotbar]) {
            case BULB:
                break;
            case GLASSES:
                break;
            case LENS:
                break;
            case CHUDISHCHE:
                break;
        }
    }
}

void tick_tock() {
	process_key_presses();

	hand.x = 1 + 17*position_in_hotbar;

	for (int item_index = 0; item_index < HOTBAR_WIDTH_IN_ITEMS; item_index++) {
		switch (hotbar[item_index]) {
			case NOTHING:
				item_sprites[item_index]->change(NOTHING);
				break;
			case BULB:
				item_sprites[item_index]->change(BULB);
				break;
			case GLASSES:
				item_sprites[item_index]->change(GLASSES);
				break;
			case LENS:
				item_sprites[item_index]->change(LENS);
				break;
			case CHUDISHCHE:
				item_sprites[item_index]->change(CHUDISHCHE);
				break;
		}
	}

    for (Ship *ship : ships) {
        ship->tick();
		std::cout << ship->wrangle_counter << std::endl
			<< ship->is_wrangled << std::endl;
		if (ship->ready_to_delete == 1) {
			ship->sprite->x = 1000;
			ship->sprite->y = 1000;
		}
    }
}

