/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <SDL2/SDL.h>
#include <vulxels/gfx/renderer.h>

namespace Vulxels {
	class App {
	  public:
		App();
		~App();
		void run();

	  private:
		bool m_running = true;
		SDL_Window* m_window;
		GFX::Renderer m_renderer;
	};
} // namespace Vulxels
