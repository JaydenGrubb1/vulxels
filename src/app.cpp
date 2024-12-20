/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/app.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

#include <cstdio>
#include <stdexcept>
#include <thread>

using namespace Vulxels;

App::App() {
	// TODO
}

App::~App() {
	// TODO
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
	auto fixed_loop = std::thread([&]() {
		while (m_running) {
			fixed_update();
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	});

	SDL_Event e;
	f32 last_time = static_cast<f32>(SDL_GetTicks());

	while (m_running) {
		f32 current_time = static_cast<f32>(SDL_GetTicks());
		f32 delta = current_time - last_time;
		last_time = current_time;

		render_update(delta);
		late_update(delta);

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				m_running = false;
			}
		}
	}

	fixed_loop.join();
}
