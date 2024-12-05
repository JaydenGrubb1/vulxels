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

#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.hpp>

namespace Vulxels::GFX {
	class Renderer {
	  public:
		Renderer() = default;
		Renderer(SDL_Window *window);
		~Renderer();

	  private:
		vk::ApplicationInfo m_appinfo;
		vk::Instance m_instance;
		vk::SurfaceKHR m_surface;
		SDL_Window *m_window;
	};
}
