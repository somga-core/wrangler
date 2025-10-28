/*
TeCo - one-file-headder C++ terminal and gui game engine
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>  
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>

#define unfduration std::chrono::nanoseconds
#define second_ratio 1000000000L
#define unftime() std::chrono::time_point_cast<unfduration>(std::chrono::system_clock::now())

#ifdef _WIN32
#include <Windows.h>
#define unfsleep(sleep) usleep((sleep * 1000L) / second_ratio)

#else
#include <unistd.h>
#define unfsleep(sleep) usleep((sleep * 1000000L) / second_ratio) 

#endif

namespace teco {

// functions inits
void init(void (*) (), int, int, int, int, const char[32]);
void exit();
void tick();

void mainloop();

void process_io();
void handle_events();
void draw();
void playsounds(const char[64]);

bool is_key_pressed(int);

// enums
enum {
    GUI = 0,
    TUI = 1
};

enum {
    LOOPING = 0,
    STOP_ON_FIRST_FRAME = 1,
    STOP_ON_LAST_FRAME = 2
};

// consts
const int STANDARD_BACKGROUND_RED = 0x12; 
const int STANDARD_BACKGROUND_GREEN = 0x12;
const int STANDARD_BACKGROUND_BLUE = 0x12;	
const int STANDARD_WINDOW_WIDTH = 1152;
const int STANDARD_WINDOW_HEIGHT = 672;
const int WIDTH_IN_SYMBOLS = 192;
const int HEIGHT_IN_SYMBOLS = 56;
const int WIDTH_PER_SYMBOL = 6;
const int HEIGHT_PER_SYMBOL = 12;

const SDL_Color COLORS[16] {
    SDL_Color {229, 229, 229},
    SDL_Color {160, 160, 160},
    SDL_Color {6, 163, 143},
    SDL_Color {5, 127, 113},
    SDL_Color {168, 88, 175},
    SDL_Color {103, 0, 154},
    SDL_Color {42, 110, 192},
    SDL_Color {22, 67, 124},
    SDL_Color {202, 152, 16},
    SDL_Color {176, 97, 7},
    SDL_Color {135, 172, 0},
    SDL_Color {94, 119, 0},
    SDL_Color {174, 1, 0},
    SDL_Color {124, 0, 27},
    SDL_Color {103, 103, 103},
    SDL_Color {0, 0, 0}
};

// variables
int fps;
int tps;
auto tick_slice = unfduration::zero();
auto draw_slice = unfduration::zero();

long tick_counter = 0;

auto last_update_time = unftime();
auto accumulator = unfduration::zero();

void (*tick_function) ();

int graphics_type;

std::vector<int> pressed_keys;

bool run = true;
  
int window_width = STANDARD_WINDOW_WIDTH;
int window_height = STANDARD_WINDOW_HEIGHT;

SDL_DisplayMode display_mode;
SDL_Event event;
SDL_Renderer *renderer = NULL;
SDL_Window *window = NULL;
SDL_Surface *window_surface = NULL;
SDL_Surface *text_surface = NULL;
TTF_Font *font = NULL;
SDL_Texture *text_texture = NULL;

SDL_Texture *symbol_textures[16][24];
char symbols_to_prepare_textures_from_them[] = "+-_()'\"=|\\/~#$%&^*.,;:";

// classes
class Source {
public:
    std::vector<std::string> symbols;
    std::vector<std::string> colors;

    Source(const char symbols_path[64], const char colors_path[64]) {
        load_symbols(symbols_path);
        load_colors(colors_path);
    }

    std::vector<std::string> read_file(const char file_name[64]) {
        std::vector<std::string> result;
        std::string line;
     
        std::ifstream in (file_name);
        if (in.is_open()) {
            while (std::getline(in, line)) {
                result.push_back(line);
            }
        }
        in.close();

        return result;
    }

    void load_symbols(const char symbols_path[64]) {
        symbols = read_file(symbols_path);
    }

    void load_colors(const char colors_path[64]) {
        colors = read_file(colors_path);
    }
};

class Animation {
public:
    std::vector<Source> sources;
    int loop_mode;
    int ticks_per_frame;

    Animation(std::vector<Source> _sources, int _loop_mode = STOP_ON_FIRST_FRAME, int _ticks_per_frame = 2) {
        sources = _sources;
        loop_mode = _loop_mode;
        ticks_per_frame = _ticks_per_frame;
    }
};

class Sprite {
public:
    int x = 0;
    int y = 0;
    std::vector<Animation> animations;
    int layer;
    int current_animation_index = 0;
    int current_frame_index = 0;
    bool is_playing_animations = true;
    int current_tick = 0;

    Sprite() {};

    Sprite(int, int, std::vector<Animation>);

    void play_animation(int animation_index) {
        if (animation_index != current_animation_index) {
            current_animation_index = animation_index;
            current_frame_index = 0;
            current_tick = 0;
        }
    }

    void update_animations() {
        if (is_playing_animations && current_tick++ == 0) {
            if (++current_frame_index >= animations[current_animation_index].sources.size()) {
                switch (animations[current_animation_index].loop_mode) {
                    case LOOPING:
                        current_frame_index = 0;
                        break;
                    case STOP_ON_LAST_FRAME:
                        is_playing_animations = false;
                        current_frame_index--;
                        break;
                    case STOP_ON_FIRST_FRAME:
                        is_playing_animations = false;
                        current_frame_index = 0;
                        break;
                }
            }
        }
        if (current_tick > animations[current_animation_index].ticks_per_frame - 1) {
            current_tick = 0;
        }
    }
};

std::vector<Sprite*> sprites;

Sprite::Sprite(int _x, int _y, std::vector<Animation> _animations) {
    x = _x;
    y = _y;
    animations = _animations;
    
    sprites.push_back(this);
}

class Screen {
public:
	char symbols[HEIGHT_IN_SYMBOLS][WIDTH_IN_SYMBOLS];
	char colors[HEIGHT_IN_SYMBOLS][WIDTH_IN_SYMBOLS];

	void add_sprite(Sprite sprite) {
        Source source = sprite.animations[sprite.current_animation_index].sources[sprite.current_frame_index];

        add_array(source.symbols, symbols, sprite.y, sprite.x);
        add_array(source.colors, colors, sprite.y, sprite.x);
    }
        
    void add_array(std::vector<std::string> array, char (&target)[HEIGHT_IN_SYMBOLS][WIDTH_IN_SYMBOLS], int offset_y, int offset_x) {
        for (int line = 0; line < array.size(); line++) {
            for (int column = 0; column < array[line].size(); column++) {
                int target_line = line + offset_y;
                int target_column = column + offset_x;
                if (target_line >= 0 && target_line < HEIGHT_IN_SYMBOLS && 
                    target_column >= 0 && target_column < WIDTH_IN_SYMBOLS) {
                    if (array[line][column] != ' ')
                        target[target_line][target_column] = array[line][column];
                }
			}
		}
    }

    void clear() {
        for (int line = 0; line < HEIGHT_IN_SYMBOLS; line++) {
			for (int column = 0; column < WIDTH_IN_SYMBOLS; column++) {
				symbols[line][column] = ' ';
			}
		}
    }
};

Screen screen;

// functions
void init(void (*_tick_function) (), int _graphics_type = TUI, int _fps = 60, int _tps = 20, int _layer_count = 8, const char _title[32] = "TeCo app") {
    graphics_type = _graphics_type;
    fps = _fps;
    tps = _tps;
    tick_slice = unfduration(second_ratio / tps);
    draw_slice = unfduration(second_ratio / fps);
    tick_function = _tick_function;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit();
    }

    window = SDL_CreateWindow(
        _title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        STANDARD_WINDOW_WIDTH, STANDARD_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE
    );

    if (window == NULL) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    }

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == NULL)
    {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    }

	if (TTF_Init() == -1) {
		std::cout << "TTF_Init ne initializirovalsa" << std::endl;
	}

	font = TTF_OpenFont("./JetBrainsMono-Regular.ttf", 20);

    SDL_SetRenderDrawColor(renderer, STANDARD_BACKGROUND_RED, STANDARD_BACKGROUND_GREEN, STANDARD_BACKGROUND_BLUE, 0x00);

	Screen screen;

    char temp_char[2];
    temp_char[1] = '\0';

    for (int color = 0; color < 16; color++) {
        SDL_Color text_color = COLORS[color];

        for (int symbol_index = 0; symbol_index < strlen(symbols_to_prepare_textures_from_them); symbol_index++) {
            temp_char[0] = symbols_to_prepare_textures_from_them[symbol_index];
            text_surface = TTF_RenderText_Solid(font, temp_char, text_color);
            symbol_textures[color][symbol_index] = SDL_CreateTextureFromSurface(
                renderer,
                text_surface
            );
            SDL_FreeSurface(text_surface);
        }
    }
}


void tick() {
    tick_function();
    tick_counter++;
    for (auto sprite : sprites) {
		sprite->update_animations();
    }
}

bool is_key_pressed(int key) {
    bool result = std::find(pressed_keys.begin(), pressed_keys.end(), key) != pressed_keys.end();

    if (result) {
		for (int key_index = 0; key_index < pressed_keys.size(); key_index++) {
			if (pressed_keys[key_index] == key) {
				std::vector<int>::iterator key_iterator = pressed_keys.begin() + key_index;
				pressed_keys.erase(key_iterator);
			}
		}
	}

    return result;
}

void mainloop() {
    while (run) {
        auto delta_time = unftime() - last_update_time;
        last_update_time = unftime();
        accumulator += delta_time;
        
        while (accumulator > tick_slice) {
            tick();
            accumulator -= tick_slice;
        }

        process_io();
        
        if (delta_time < draw_slice)
            unfsleep((draw_slice - delta_time).count());
    }
}

void process_io() {
	handle_events();
	draw();
    const char *path = "path";
	playsounds(path);
}

void handle_events() {
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			exit();
		}

		else if (event.type == SDL_WINDOWEVENT) {
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
				window_width = event.window.data1;
				window_height = event.window.data2;
			}
		}

		else if (event.type == SDL_KEYDOWN) {
			pressed_keys.push_back(event.key.keysym.sym);
		}
	}
}

void draw() {
	SDL_RenderClear(renderer);
    
    screen.clear();

	for (auto sprite : sprites) {
        screen.add_sprite(*sprite);
    }

	char current_symbol[2];
	current_symbol[1] = '\0';

	char current_color[2];
	current_color[1] = '\0';

	for (int line = 0; line < teco::HEIGHT_IN_SYMBOLS; line++) {
		for (int column = 0; column < teco::WIDTH_IN_SYMBOLS; column++) {
			if (screen.symbols[line][column] != ' ') {
                current_symbol[0] = screen.symbols[line][column];
                current_color[0] = screen.colors[line][column];

                int color = strtol(current_color, NULL, 16);
                
                if (strstr(symbols_to_prepare_textures_from_them, current_symbol)) {
                    const char *ptr = strchr(symbols_to_prepare_textures_from_them, screen.symbols[line][column]);
                    int index = ptr - symbols_to_prepare_textures_from_them;

                    SDL_Rect text_rectangle = {
                        column*window_width/WIDTH_IN_SYMBOLS,
                        line*window_height/HEIGHT_IN_SYMBOLS,
                        window_width/WIDTH_IN_SYMBOLS, 
                        window_height/HEIGHT_IN_SYMBOLS
                    };
                    SDL_RenderCopy(renderer, symbol_textures[color][index], NULL, &text_rectangle);
                } else {
                    text_surface = TTF_RenderText_Solid(font, current_symbol, COLORS[color]);
                    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                    SDL_FreeSurface(text_surface);
                    SDL_Rect text_rectangle = {
                        column*window_width/WIDTH_IN_SYMBOLS,
                        line*window_height/HEIGHT_IN_SYMBOLS,
                        window_width/WIDTH_IN_SYMBOLS, 
                        window_height/HEIGHT_IN_SYMBOLS
                    };
                    SDL_RenderCopy(renderer, text_texture, NULL, &text_rectangle);
                    SDL_DestroyTexture(text_texture);
                }
			}
		}
	}
	
	SDL_RenderPresent(renderer);
}

void playsounds(const char path_to_sound[64]) {
}

void exit() {
    run = false;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

}
