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
#include <thread>

#include <SDL2/SDL.h>

#include <vulxels/app.h>
#include <vulxels/types.h>

using namespace Vulxels;

static SDL_Window *s_window;
static bool s_running = true;

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

static void fixed_update() {
	// Placeholder for fixed update
}

static void render_update(float delta) {
	// Placeholder for render update
	(void)delta;
}

static void late_update(float delta) {
	// Placeholder for late update
	(void)delta;
}

void App::run() {
	auto fixed_loop = std::thread([]() {
		while (s_running) {
			fixed_update();
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	});

	SDL_Event e;
	f32 last_time = static_cast<f32>(SDL_GetTicks());

	while (s_running) {
		f32 current_time = static_cast<f32>(SDL_GetTicks());
		f32 delta = current_time - last_time;
		last_time = current_time;

		render_update(delta);
		late_update(delta);

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				s_running = false;
			}
		}
	}

	fixed_loop.join();
}
