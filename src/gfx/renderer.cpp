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

#include <SDL2/SDL_vulkan.h>

#include <vulxels/gfx/renderer.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

using namespace Vulxels::GFX;

Renderer::Renderer(SDL_Window *window) : m_window(window) {
	u32 vk_ver = vk::enumerateInstanceVersion();

#ifndef DNDEBUG
	printf("Vulkan API version: %d.%d.%d-%d\n",
		   VK_API_VERSION_MAJOR(vk_ver), VK_API_VERSION_MINOR(vk_ver),
		   VK_API_VERSION_PATCH(vk_ver), VK_API_VERSION_VARIANT(vk_ver));
#endif

	if (vk_ver < VK_API_VERSION_1_2) {
		throw std::runtime_error("Vulkan API version is not supported");
	}

	m_appinfo.pApplicationName = "Vulxels";
	m_appinfo.applicationVersion = VK_MAKE_VERSION(VULXELS_VERSION_MAJOR, VULXELS_VERSION_MINOR, VULXELS_VERSION_PATCH);
	m_appinfo.pEngineName = "Vulxels";
	m_appinfo.engineVersion = VK_API_VERSION_1_2;
	m_appinfo.apiVersion = VK_API_VERSION_1_2;

	u32 extension_count;
	SDL_Vulkan_GetInstanceExtensions(m_window, &extension_count, nullptr);
	std::vector<const char *> extensions(extension_count);
	SDL_Vulkan_GetInstanceExtensions(m_window, &extension_count, extensions.data());

#ifndef DNDEBUG
	printf("Required Vulkan extensions:\n");
	for (const auto &ext : extensions) {
		printf("  %s\n", ext);
	}
#endif

	vk::InstanceCreateInfo info;
	info.pApplicationInfo = &m_appinfo;
	info.ppEnabledExtensionNames = extensions.data();
	info.enabledExtensionCount = extensions.size();

#ifdef DNDEBUG
	info.enabledLayerCount = 0;
#else
	constexpr std::array requested_layers = {"VK_LAYER_KHRONOS_validation"};
	auto supported_layers = vk::enumerateInstanceLayerProperties();

	for (const auto &layer : requested_layers) {
		bool found = false;
		for (const auto &supported : supported_layers) {
			if (strcmp(layer, supported.layerName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			throw std::runtime_error("Validation layer not supported");
		}
	}

	info.ppEnabledLayerNames = requested_layers.data();
	info.enabledLayerCount = requested_layers.size();
#endif

	if (vk::createInstance(&info, nullptr, &m_instance) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create Vulkan instance");
	}

	if (SDL_Vulkan_CreateSurface(m_window, m_instance, reinterpret_cast<VkSurfaceKHR *>(&m_surface)) != SDL_TRUE) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
}

Renderer::~Renderer() {
	m_instance.destroySurfaceKHR(m_surface);
	m_instance.destroy();
}
