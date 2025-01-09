/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <SDL2/SDL.h>

#include <string_view>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Window {
	  public:
		Window() = default;
		Window(std::string_view title, int width, int height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		std::vector<const char*> get_required_extensions();
		vk::raii::SurfaceKHR create_surface(vk::raii::Instance& instance);
		vk::Extent2D extent() const;

		SDL_Window* window() {
			return m_window;
		}

	  private:
		SDL_Window* m_window = nullptr;
	};
} // namespace Vulxels::GFX
