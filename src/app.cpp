/**
 * @author Jayden Grubb (contact@jaydengrubb.com)
 * @date 2024-12-05
 *
 * Copyright (c) 2024, Jayden Grubb
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cstdio>
#include <stdexcept>

#include <SDL2/SDL.h>

#include <vulxels/app.h>

using namespace Vulxels;

static SDL_Window *s_window;

App::App() {
	SDL_Init(SDL_INIT_VIDEO);
	s_window = SDL_CreateWindow("Vulxels", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								800, 600, SDL_WINDOW_SHOWN);

	if (s_window == nullptr) {
		throw std::runtime_error("Failed to create window");
	}
}

App::~App() {
	SDL_DestroyWindow(s_window);
	SDL_Quit();
}

void App::run() {
	bool running = true;
	SDL_Event e;
	while (running) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}
	}
}
