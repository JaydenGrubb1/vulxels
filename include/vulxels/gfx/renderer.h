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
	class Swapchain;

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

		friend class Swapchain;
	};

	class Swapchain {
	  public:
		Swapchain() = default;
		Swapchain(Renderer *renderer, SDL_Window *window);
		~Swapchain();

		Swapchain(const Swapchain &) = delete;
		Swapchain &operator=(const Swapchain &) = delete;

		Swapchain(Swapchain &&other) noexcept : m_state(std::exchange(other.m_state, nullptr)) {}
		Swapchain &operator=(Swapchain &&other) noexcept {
			std::swap(m_state, other.m_state);
			return *this;
		}

	  private:
		struct State;
		State *m_state = nullptr;
		Renderer *m_renderer = nullptr;

		bool pick_surface_format();
		bool pick_present_mode();
		void find_extent(SDL_Window *window);
		void create(Swapchain *old = nullptr);

		friend class Renderer;
	};
}
