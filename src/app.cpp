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

#include <vulxels/app.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

#include <cstdio>
#include <stdexcept>
#include <thread>

using namespace Vulxels;

App::App() {
	SDL_Init(SDL_INIT_VIDEO);
	m_window = SDL_CreateWindow(
		"Vulxels",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);

	if (m_window == nullptr) {
		throw std::runtime_error("Failed to create window");
	}

	new (&m_renderer) GFX::Renderer(m_window);
}

App::~App() {
	SDL_DestroyWindow(m_window);
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
