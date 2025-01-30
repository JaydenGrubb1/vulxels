/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <SDL3/SDL.h>

#include <span>
#include <string_view>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Window {
	  public:
		Window() = default;
		Window(std::string_view title, int width, int height);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		vk::raii::SurfaceKHR create_surface(const vk::raii::Instance& instance) const;
		vk::Extent2D extent() const;

		SDL_Window* window() const {
			return m_window;
		}

		static std::span<const char* const> get_required_extensions();

	  private:
		SDL_Window* m_window = nullptr;
	};
} // namespace Vulxels::GFX
