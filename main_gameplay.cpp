namespace main_gameplay {

enum {
	NOTHING,
    BULB,
    GLASSES,
    LENS,
    CHUDISHCHE,
	ITEMS_COUNT
};

const int HOTBAR_VOLUME = 10;

class LightBeam {
public:
	teco::Sprite *sprite = NULL;

	int x = 0;
	int y = 0;
	int width = 34;
	int height = 69;

	LightBeam();
};

class Ship {
	teco::Sprite *sprite = NULL;

	int is_turned_right = 0;
	int is_correct = 1;
	int is_wrangled = 0;
	int wrangle_counter = 0;
	int ready_to_delete = 0;

	double speed_x;
	double speed_y;
	double speed;

	double x = 0;
	double y = 0;

	int width = 34;
	int height = 69;

	int left_point_y;
	int right_point_y;

	Ship();
	void tick();
};

teco::Screen main_screen;

teco::Screen sea;
teco::Screen hotbar;
teco::Screen useless_fucking_bloated_gui_panel_use_keyboard_instead;

teco::Sprite waves_sprite;
teco::Sprite turned_left_ship_sprite;
teco::Sprite turned_right_ship_sprite;
teco::Sprite light_beam_sprite;
teco::Sprite background_sprite;
teco::Sprite hand_sprite;

teco::Sprite nothing_sprite;
teco::Sprite bulb_sprite;
teco::Sprite glasses_sprite;
teco::Sprite lens_sprite;
teco::Sprite chudishche_sprite;

LightBeam light_beam;

int position_in_hotbar;
int hotbar_inventory[HOTBAR_VOLUME] = {0, 0, 1, 2, 1, 0};

int score;
int forfeit_ships;

std::vector<Ship*> ships;

void init();

void process_keypresses();

bool do_intersect(int xa, int ya, int wa, int ha, int xb, int yb, int wb, int hb);

void main_tick();
void main_draw();

void sea_tick();
void sea_draw();
void hotbar_tick();
void hotbar_draw();

void ship_tick();

// Place where we can separate this file to header and actual code part

void init() { // Temporary solution for "error: does not name a type" problem
	main_screen = teco::Screen(192, 56, &main_tick, &main_draw);

	sea = teco::Screen(117, 46, &sea_tick, &sea_draw);
	hotbar = teco::Screen(69, 34, &hotbar_tick, &hotbar_draw);

	waves_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/gui/waves/fr1.tcsb",
						"assets/sources/gui/waves/df.tccl",
						"assets/sources/gui/waves/fr1.tcsb"
					)
				}
			)
		}
	);

	turned_left_ship_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/ships/schooner/left/cl.tcsb",
						"assets/sources/ships/schooner/left/cl.tccl",
						"assets/sources/ships/schooner/left/cl.tcsb"
					)
				}
			)
		}
	);

	turned_right_ship_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/ships/schooner/right/cl.tcsb",
						"assets/sources/ships/schooner/right/cl.tccl",
						"assets/sources/ships/schooner/right/cl.tcsb"
					)
				}
			)
		}
	);

	light_beam_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/light_beam/df.tcsb",
						"assets/sources/light_beam/df.tccl",
						"assets/sources/light_beam/df.tcsb"
					)
				}
			)
		}
	);

	hand_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/gui/hand.tcsb",
						"assets/sources/gui/hand.tccl",
						"assets/sources/gui/hand.tcsb"
					)
				}
			)
		}
	);

	nothing_sprite = teco::Sprite(
		std::vector<teco::Animation> {
			teco::Animation(
				std::vector<teco::Source> {
					teco::Source(
						"assets/sources/empty.tcsb",
						"assets/sources/empty.tccl",
						"assets/sources/empty.tcsb"
					)
				}
			)
		}
	);

	position_in_hotbar = 0;

	score = 0;
	forfeit_ships = 0;
}

LightBeam::LightBeam() {
	sprite = &light_beam_sprite;
}

Ship::Ship() {
	// That's a wierd line of code right here
	// tick = &ship_tick;

	left_point_y = sea.height + rand() % (sea.height - this->height);
	right_point_y = sea.height + rand() % (sea.height - this->width);

	speed = (rand() % 100 + 50) / 450;

	float speed_coefficient = speed / hypot(left_point_y - right_point_y, sea.width);

	speed_x = sea.width * speed_coefficient;
	speed_y = (right_point_y - left_point_y) * speed_coefficient;

	is_turned_right = rand() % 2;
	is_correct = rand() % 2;

	if (is_turned_right) {
		sprite = &turned_right_ship_sprite;
		x = sea.width;
		y = left_point_y;
	} else {
		sprite = &turned_left_ship_sprite;
		speed_x = -speed_x;
		speed_y = -speed_y;
		x = sea.width - width;
		y = right_point_y;
	}
}

void Ship::tick() {
	if (do_intersect(x, y, width, height, light_beam.x, light_beam.y, light_beam.width, light_beam.height)) {
		wrangle_counter++;
	}

	if (
		(is_turned_right and x < sea.width or !is_turned_right and x > 0) and
		(speed_y > 0 and y > 0 or speed_y <= 0 and y < sea.height)
	) {
		x += speed_x;
		y += speed_y;
	}
	else {
		// delete
		ready_to_delete = 1;
	}

	if (wrangle_counter >= 100) {
		ready_to_delete = 1;
		score++;
		wrangle_counter = 0;
	}
}

bool do_intersect(int xa, int ya, int wa, int ha, int xb, int yb, int wb, int hb) {
	return (
		(abs(xa - xb) < wa && abs(ya - yb) < ha) ||
		(abs(xa - xb) < wb && abs(ya - yb) < ha) ||
		(abs(xa - xb) < wa && abs(ya - yb) < hb) ||
		(abs(xa - xb) < wb && abs(ya - yb) < hb)
	);
}

void main_tick() {

}

void main_draw() {

}

void sea_tick() {

}

void sea_draw() {

}

void hotbar_tick() {

}

void hotbar_draw() {

}

void ship_tick() {

}

}

