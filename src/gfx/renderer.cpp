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

#include <array>
#include <cstdio>
#include <cstring>
#include <optional>

#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vulxels/gfx/renderer.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

using namespace Vulxels::GFX;

#ifdef DNDEBUG
static constexpr std::array<const char *, 0> VALIDATION_LAYERS = {};
#else
static constexpr std::array VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
#endif

struct Renderer::State {
	vk::raii::Context context;
	vk::ApplicationInfo appinfo;
	std::vector<const char *> extensions;
	vk::raii::Instance instance = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
};

Renderer::Renderer(SDL_Window *window) {
	m_state = new State;
	u32 ver = m_state->context.enumerateInstanceVersion();

#ifndef DNDEBUG
	printf(
		"Vulkan API version: %d.%d.%d-%d\n",
		VK_API_VERSION_MAJOR(ver),
		VK_API_VERSION_MINOR(ver),
		VK_API_VERSION_PATCH(ver),
		VK_API_VERSION_VARIANT(ver)
	);
#endif

	if (ver < VK_API_VERSION_1_2) {
		throw std::runtime_error("Vulkan API version is not supported");
	}

	m_state->appinfo.pApplicationName = "Vulxels";
	m_state->appinfo.applicationVersion = VK_MAKE_VERSION(VULXELS_VERSION_MAJOR, VULXELS_VERSION_MINOR, VULXELS_VERSION_PATCH);
	m_state->appinfo.pEngineName = "Vulxels";
	m_state->appinfo.engineVersion = VK_API_VERSION_1_2;
	m_state->appinfo.apiVersion = VK_API_VERSION_1_2;

	check_extensions(window);
	check_layers();

	m_state->instance = vk::raii::Instance(
		m_state->context,
		vk::InstanceCreateInfo()
			.setPApplicationInfo(&m_state->appinfo)
			.setEnabledExtensionCount(m_state->extensions.size())
			.setPpEnabledExtensionNames(m_state->extensions.data())
			.setEnabledLayerCount(VALIDATION_LAYERS.size())
			.setPpEnabledLayerNames(VALIDATION_LAYERS.data())
	);

	VkSurfaceKHR surface;
	if (SDL_Vulkan_CreateSurface(window, *m_state->instance, &surface) != SDL_TRUE) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	m_state->surface = vk::raii::SurfaceKHR(m_state->instance, surface);
}

Renderer::~Renderer() {
	delete m_state;
}

void Renderer::check_extensions(SDL_Window *window) {
	u32 count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	m_state->extensions.resize(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, m_state->extensions.data());

	// TODO: Push additional required extensions

#ifndef DNDEBUG
	printf("Required Vulkan extensions:\n");
	for (const auto &ext : m_state->extensions) {
		printf("  %s\n", ext);
	}
#endif

	auto supported = m_state->context.enumerateInstanceExtensionProperties();
	for (const auto &ext : m_state->extensions) {
		bool found = false;
		for (const auto &sup : supported) {
			if (strcmp(ext, sup.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			throw std::runtime_error("Required Vulkan extension not supported");
		}
	}
}

void Renderer::check_layers() {
	auto supported = m_state->context.enumerateInstanceLayerProperties();
	for (const auto &layer : VALIDATION_LAYERS) {
		bool found = false;
		for (const auto &sup : supported) {
			if (strcmp(layer, sup.layerName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			throw std::runtime_error("Validation layer not supported");
		}
	}
}
