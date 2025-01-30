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

Window::Window(const std::string_view title, const int width, const int height) {
	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	if (m_window == nullptr) {
		throw std::runtime_error("Failed to create window");
	}
}

Window::~Window() {
	SDL_DestroyWindow(m_window);
}

vk::raii::SurfaceKHR Window::create_surface(const vk::raii::Instance& instance) const {
	VkSurfaceKHR surface;
	if (!SDL_Vulkan_CreateSurface(m_window, *instance, nullptr, &surface)) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	return {instance, surface};
}

vk::Extent2D Window::extent() const {
	int width, height;
	SDL_GetWindowSizeInPixels(m_window, &width, &height);
	return {static_cast<u32>(width), static_cast<u32>(height)};
}

std::span<const char* const> Window::get_required_extensions() {
	u32 count;
	return {SDL_Vulkan_GetInstanceExtensions(&count), count};
}
