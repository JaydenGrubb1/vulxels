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
