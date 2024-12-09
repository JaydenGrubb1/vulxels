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
#include <set>

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

static constexpr std::array DEVICE_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamily {
	std::optional<u32> index;
	vk::raii::Queue queue = nullptr;
};

struct Renderer::State {
	vk::raii::Context context;
	vk::ApplicationInfo appinfo;
	std::vector<const char *> extensions;
	vk::raii::Instance instance = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physical_device = nullptr;
	vk::raii::Device device = nullptr;
	QueueFamily graphics_queue;
	QueueFamily present_queue;
};

static bool check_extensions(Renderer::State *state, SDL_Window *window) {
	u32 count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	state->extensions.resize(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, state->extensions.data());

	// TODO: Push additional required extensions

#ifndef DNDEBUG
	printf("Required Vulkan extensions:\n");
	for (const auto &ext : state->extensions) {
		printf("  %s\n", ext);
	}
#endif

	auto supported = state->context.enumerateInstanceExtensionProperties();
	for (const auto &ext : state->extensions) {
		bool found = false;
		for (const auto &sup : supported) {
			if (strcmp(ext, sup.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

static bool check_layers(Renderer::State *state) {
	auto supported = state->context.enumerateInstanceLayerProperties();
	for (const auto &layer : VALIDATION_LAYERS) {
		bool found = false;
		for (const auto &sup : supported) {
			if (strcmp(layer, sup.layerName) == 0) {
				found = true;
				break;
			}
		}
		if (!found) {
			return false;
		}
	}
	return true;
}

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

	if (!check_extensions(m_state, window)) {
		throw std::runtime_error("Failed to find required Vulkan extensions");
	}
	if (!check_layers(m_state)) {
		throw std::runtime_error("Failed to find required Vulkan layers");
	}

	m_state->instance = vk::raii::Instance(
		m_state->context,
		vk::InstanceCreateInfo()
			.setPApplicationInfo(&m_state->appinfo)
			.setPEnabledExtensionNames(m_state->extensions)
			.setPEnabledLayerNames(VALIDATION_LAYERS)
	);

	VkSurfaceKHR surface;
	if (SDL_Vulkan_CreateSurface(window, *m_state->instance, &surface) != SDL_TRUE) {
		throw std::runtime_error("Failed to create Vulkan surface");
	}
	m_state->surface = vk::raii::SurfaceKHR(m_state->instance, surface);

	m_state->physical_device = m_state->instance.enumeratePhysicalDevices().front();
	// TODO: Pick a device properly, this will do for now

#ifndef DNDEBUG
	printf("Using physical device: %s\n", m_state->physical_device.getProperties().deviceName.data());
#endif

	auto queue_families = m_state->physical_device.getQueueFamilyProperties();
	for (u32 i = 0; i < queue_families.size(); i++) {
		if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			m_state->graphics_queue.index = i;
		}
		if (m_state->physical_device.getSurfaceSupportKHR(i, *m_state->surface)) {
			m_state->present_queue.index = i;
		}
		if (m_state->graphics_queue.index == m_state->present_queue.index) {
			break;
		}
	}

	if (!m_state->graphics_queue.index.has_value() || !m_state->present_queue.index.has_value()) {
		throw std::runtime_error("Failed to find required queue families");
	}

	std::set<u32> unique_queue_families = {
		m_state->graphics_queue.index.value(),
		m_state->present_queue.index.value()
	};

	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	f32 queue_priority = 1.0f;
	for (u32 queue_family : unique_queue_families) {
		queue_create_infos.push_back(
			vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(queue_family)
				.setQueueCount(1)
				.setPQueuePriorities(&queue_priority)
		);
	}

	vk::PhysicalDeviceFeatures features;
	features.samplerAnisotropy = VK_TRUE;

	m_state->device = vk::raii::Device(
		m_state->physical_device,
		vk::DeviceCreateInfo()
			.setPEnabledLayerNames(VALIDATION_LAYERS)
			.setPEnabledExtensionNames(DEVICE_EXTENSIONS)
			.setQueueCreateInfos(queue_create_infos)
			.setPEnabledFeatures(&features)
	);

	m_state->graphics_queue.queue = m_state->device.getQueue(m_state->graphics_queue.index.value(), 0);
	m_state->present_queue.queue = m_state->device.getQueue(m_state->present_queue.index.value(), 0);
}

Renderer::~Renderer() {
	delete m_state;
}
