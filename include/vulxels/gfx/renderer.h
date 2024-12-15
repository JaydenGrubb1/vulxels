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

#include <utility>

#include <SDL2/SDL.h>

namespace Vulxels::GFX {
	class Renderer {
	  public:
		Renderer() = default;
		Renderer(SDL_Window *window);
		~Renderer();

	  private:
		struct State;
		State *m_state;

		bool find_and_check_required_extensions(SDL_Window *window);
		bool check_validation_layers();
		bool find_queue_families();
	};
}
