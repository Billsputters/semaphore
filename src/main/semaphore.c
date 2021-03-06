#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include <cairo.h>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "sem_action_list.h"
#include "sem_compass.h"
#include "sem_dynamic_array.h"
#include "sem_error.h"
#include "sem_game.h"
#include "sem_heap.h"
#include "sem_input.h"
#include "sem_render.h"
#include "sem_serialize.h"
#include "sem_timer.h"
#include "sem_train.h"
#include "sem_world.h"

int main(int argc, char **argv) {
	int fullscreen = 0;
	int c;
	struct option long_options[] = { {"fullscreen", no_argument, &fullscreen, true}, {0,0,0,0}};
	do {
		int option_index = 0;
		c = getopt_long(argc, argv, "", long_options, &option_index);
	} while (c != -1);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sem_set_error("Unable to initialize SDL: %s", SDL_GetError());
		return sem_fatal_error();
	}
	atexit(SDL_Quit);

	SDL_Window* window;
	if (fullscreen) {
		window = SDL_CreateWindow("semaphore", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	} else {
		window = SDL_CreateWindow("semaphore", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
	}
	if (window == NULL) {
		sem_set_error("Unable to create window: %s", SDL_GetError());
		return sem_fatal_error();
	}

	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer == NULL) {
		sem_set_error("Unable to create renderer: %s", SDL_GetError());
		return sem_fatal_error();
	}

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	SDL_Texture *texture;
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);
	if (texture == NULL) {
		sem_set_error("Unable to create texture: %s", SDL_GetError());
		return sem_fatal_error();
	}

	void *pixels;
	int pitch;

	if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
		sem_set_error("Failed to lock texture: %s", SDL_GetError());
		return sem_fatal_error();
	}

	cairo_surface_t* cairo_surface = cairo_image_surface_create_for_data(
		pixels,
		CAIRO_FORMAT_ARGB32,
		width, height, pitch);

	cairo_t* cr = cairo_create(cairo_surface);

	SDL_UnlockTexture(texture);

	sem_render_context render_ctx;
	sem_render_context_init(&render_ctx, cr, (uint32_t) width, (uint32_t) height);

	sem_render_style render_style;
	render_ctx.style = &render_style;
	sem_render_default_style(&render_style);

	FILE* map;
	if (optind < argc) {
		map = fopen(argv[optind], "r");
	} else {
		map = fopen("maps/64x64test", "r");
	}

	sem_game game;
	if (sem_serialize_load(map, &game) != SEM_OK) return sem_fatal_error();
	fclose(map);

	SDL_Event e;
	bool quit = false;	
	uint64_t frames = 0;
	while (!quit) {
		sem_timer_now(game.world.timer);

		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_LCTRL || e.key.keysym.sym == SDLK_RCTRL) {
					// ignore
				} else if (e.key.keysym.sym == SDLK_l) {
					sem_game_destroy(&game);
					FILE* load = fopen("build/main/saved_map", "r");
					if (sem_serialize_load(load, &game) != SEM_OK) return sem_fatal_error();
					fclose(load);
				} else if (e.key.keysym.sym == SDLK_s) {
					FILE* save = fopen("build/main/saved_map", "w");
					if (sem_serialize_save(save, &game) != SEM_OK) return sem_fatal_error();
					fclose(save);
				} else if (e.key.keysym.sym == SDLK_LEFT) {
					sem_render_translate(&render_ctx, 1.0, 0.0);
				} else if (e.key.keysym.sym == SDLK_RIGHT) {
					sem_render_translate(&render_ctx, -1.0, 0.0);
				} else if (e.key.keysym.sym == SDLK_UP) {
					sem_render_translate(&render_ctx, 0.0, 1.0);
				} else if (e.key.keysym.sym == SDLK_DOWN) {
					sem_render_translate(&render_ctx, 0.0, -1.0);
				} else {
					quit = true;
				}
			}
			if (e.type == SDL_MOUSEWHEEL) {
				if (SDL_GetModState() & KMOD_CTRL) {
					if (e.wheel.y > 0) {
						game.world.timer->multiplier *= 1.1 * e.wheel.y;
					} else if (e.wheel.y < 0) {
						game.world.timer->multiplier /= -1.1 * e.wheel.y;
					}
				} else {
					if (e.wheel.y > 0) {
						sem_render_scale_up(&render_ctx, e.wheel.y);
					} else if (e.wheel.y < 0) {
						sem_render_scale_down(&render_ctx, e.wheel.y);
					}
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP) {
				double x = e.button.x;
				double y = e.button.y;

				sem_coordinate coord;
				sem_render_device_to_coord(&coord, &render_ctx, x, y);
				sem_input_event input;
				input.time = game.world.timer->now;
				input.tile = &coord;
				
				if (e.button.button == SDL_BUTTON_LEFT) {
					input.rank = PRIMARY;
				} else {
					input.rank = SECONDARY;
				}

				sem_action* a = NULL;

				// TODO: wrap up train/tile input handlers into one big input routine
				sem_train_input_act_upon(&input, &game, &a);

				if (a != NULL) {
					// TODO: should just chuck this onto the action list with "immediate" flag set
					a->function(game.world.actions, a);
				} else {
					sem_tile_input_act_upon(&input, &game, &a);
					if (a != NULL) a->function(game.world.actions, a);
				}
			}
		}

		if (sem_action_list_execute(game.world.actions, game.world.timer->now) != SEM_OK) {
			return sem_fatal_error();
		}

		sem_render_game(&render_ctx, &game);

		SDL_UnlockTexture(texture);

		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		frames++;
	}

	sem_game_destroy(&game);
	cairo_destroy(cr);
	SDL_DestroyTexture(texture);
	return EXIT_SUCCESS;
}
