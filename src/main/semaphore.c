#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <cairo.h>
#include <argp.h>

#include "command.h"

int main(/*int argc, char* argv[]*/) {
	Command command;
	while (command_read_from(stdin, &command)) {

	}

	return EXIT_SUCCESS;
}

#if 0
int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return EXIT_FAILURE;
	}
	if (atexit(SDL_Quit) != 0) {
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	int running = 1;
	
	while (running) {
		SDL_Event e;
		e.type = SDL_KEYDOWN;
		e.key.state = SDL_PRESSED;
		e.key.keysym.sym = SDLK_F12;	
		/* TODO: scancode? */
		SDL_PushEvent(&e);
		/* TODO: check success */
		printf("event pushed");
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_F12) {
					printf("a key!");
					goto mainloop;
				}
				break;
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
	}
	mainloop: printf("break");

	cairo_surface_t* const surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 128, 128);
	/* TODO: check valid */
	cairo_t* const cr = cairo_create(surface);
	/* TODO: check with cairo_status() */
	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 127, 127);
	cairo_stroke(cr);

	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		if (strstr(line, "SCREENSHOT") == line) {
			char* filename = strchr(line, ' ') + 1;
			filename[strlen(filename)-1] = '\0'; // nukes trailing newline
			cairo_surface_write_to_png(surface, filename);
			/* TODO: check == CAIRO_STATUS_SUCCESS */
		} else if (strstr(line, "KEY") == line) {
			SDL_Event e;
			e.type = SDL_KEYDOWN;
			e.key.state = SDL_PRESSED;
			e.key.keysym.sym = SDLK_F12;	
			/* TODO: scancode? */
			SDL_PushEvent(&e);
			/* TODO: check success */
		}
	}

	free(line); /* TODO: are we not leaking lines here? */
	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	return EXIT_SUCCESS;
}
#endif
