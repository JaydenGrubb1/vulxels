/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <SDL3/SDL_vulkan.h>
#include <vulxels/gfx/window.h>
#include <vulxels/types.h>

#include <stdexcept>

using namespace Vulxels::GFX;

Window::Window(std::string_view title, int width, int height) {
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow(
		title.data(),
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
	auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
	return {extensions, extensions + count};
}

vk::raii::SurfaceKHR Window::create_surface(vk::raii::Instance& instance) {
	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(m_window, *instance, nullptr, &surface)) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	return vk::raii::SurfaceKHR(instance, surface);
}

vk::Extent2D Window::extent() const {
	int width, height;
	SDL_GetWindowSizeInPixels(m_window, &width, &height);
	return vk::Extent2D(width, height);
}
