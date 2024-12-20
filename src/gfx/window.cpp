/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <SDL2/SDL_vulkan.h>
#include <vulxels/gfx/window.h>
#include <vulxels/types.h>

#include <stdexcept>

using namespace Vulxels::GFX;

Window::Window(std::string_view title, int width, int height) {
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow(
		title.data(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);

	if (m_window == nullptr) {
		throw std::runtime_error("Failed to create window");
	}
}

Window::~Window() {
	SDL_DestroyWindow(m_window);
}

std::vector<const char*> Window::get_required_extensions() {
	u32 count;
	SDL_Vulkan_GetInstanceExtensions(m_window, &count, nullptr);
	std::vector<const char*> extensions(count);
	SDL_Vulkan_GetInstanceExtensions(m_window, &count, extensions.data());
	return extensions;
}

vk::raii::SurfaceKHR Window::create_surface(vk::raii::Instance& instance) {
	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(m_window, *instance, &surface)) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	return vk::raii::SurfaceKHR(instance, surface);
}
