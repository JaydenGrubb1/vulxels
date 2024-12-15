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
#include <limits>
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
static constexpr std::array<const char *, 0> s_validation_layers = {};
#else
static constexpr std::array s_validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};
#endif

static constexpr std::array s_instance_extensions = {
	VK_KHR_SURFACE_EXTENSION_NAME
};
static constexpr std::array s_device_extensions = {
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
	Swapchain swapchain;
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

	if (!find_and_check_required_extensions(window)) {
		throw std::runtime_error("Vulkan instance does not support required extensions");
	}
	if (!check_validation_layers()) {
		throw std::runtime_error("Vulkan instance does not support required validation layers");
	}

	m_state->instance = vk::raii::Instance(
		m_state->context,
		vk::InstanceCreateInfo()
			.setPApplicationInfo(&m_state->appinfo)
			.setPEnabledExtensionNames(m_state->extensions)
			.setPEnabledLayerNames(s_validation_layers)
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

	if (!find_queue_families()) {
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
			.setPEnabledLayerNames(s_validation_layers)
			.setPEnabledExtensionNames(s_device_extensions)
			.setQueueCreateInfos(queue_create_infos)
			.setPEnabledFeatures(&features)
	);

	m_state->graphics_queue.queue = m_state->device.getQueue(m_state->graphics_queue.index.value(), 0);
	m_state->present_queue.queue = m_state->device.getQueue(m_state->present_queue.index.value(), 0);

	m_state->swapchain = Swapchain(this, window);
	m_state->swapchain.create();
	m_state->swapchain.create(&m_state->swapchain);
}

Renderer::~Renderer() {
	delete m_state;
}

bool Renderer::find_and_check_required_extensions(SDL_Window *window) {
	u32 count;
	SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
	m_state->extensions.resize(count);
	SDL_Vulkan_GetInstanceExtensions(window, &count, m_state->extensions.data());

	m_state->extensions.insert(
		m_state->extensions.end(),
		s_instance_extensions.begin(),
		s_instance_extensions.end()
	);

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
			return false;
		}
	}
	return true;
}

bool Renderer::check_validation_layers() {
	auto supported = m_state->context.enumerateInstanceLayerProperties();
	for (const auto &layer : s_validation_layers) {
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

bool Renderer::find_queue_families() {
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
	return m_state->graphics_queue.index.has_value() && m_state->present_queue.index.has_value();
}

struct Swapchain::State {
	std::optional<vk::SurfaceFormatKHR> format;
	std::optional<vk::PresentModeKHR> present_mode;
	vk::Extent2D extent;
	u32 image_count;
	vk::raii::SwapchainKHR swapchain = nullptr;
	std::vector<vk::Image> images;
	std::vector<vk::raii::ImageView> image_views;
};

Swapchain::Swapchain(Renderer *renderer, SDL_Window *window) : m_renderer(renderer) {
	m_state = new State;

	if (!pick_surface_format()) {
		throw std::runtime_error("Failed to find suitable surface format");
	}
	if (!pick_present_mode()) {
		throw std::runtime_error("Failed to find suitable present mode");
	}
	find_extent(window);
}

Swapchain::~Swapchain() {
	delete m_state;
}

bool Swapchain::pick_surface_format() {
	auto formats = m_renderer->m_state->physical_device.getSurfaceFormatsKHR(*m_renderer->m_state->surface);
	for (const auto &format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			m_state->format = format;
			return true;
		}
	}
	return false;
}

bool Swapchain::pick_present_mode() {
	auto modes = m_renderer->m_state->physical_device.getSurfacePresentModesKHR(*m_renderer->m_state->surface);
	for (const auto &mode : modes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			m_state->present_mode = mode;
			return true;
		}
	}
	return false;
}

void Swapchain::find_extent(SDL_Window *window) {
	auto capabilities = m_renderer->m_state->physical_device.getSurfaceCapabilitiesKHR(*m_renderer->m_state->surface);

	if (capabilities.currentExtent.width == std::numeric_limits<u32>::max()) {
		int width, height;
		SDL_Vulkan_GetDrawableSize(window, &width, &height);
		m_state->extent.width = std::clamp<u32>(
			width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width
		);
		m_state->extent.height = std::clamp<u32>(
			height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);
	} else {
		m_state->extent = capabilities.currentExtent;
	}

#ifndef DNDEBUG
	printf("Swapchain extent: %dx%d\n", m_state->extent.width, m_state->extent.height);
#endif

	m_state->image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0) {
		m_state->image_count = std::min<u32>(m_state->image_count, capabilities.maxImageCount);
	}
}

void Swapchain::create(Swapchain *old) {
	vk::SwapchainCreateInfoKHR info;
	info.surface = *m_renderer->m_state->surface;
	info.minImageCount = m_state->image_count;
	info.imageFormat = m_state->format->format;
	info.imageColorSpace = m_state->format->colorSpace;
	info.imageArrayLayers = 1;
	info.imageExtent = m_state->extent;
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	info.presentMode = m_state->present_mode.value();
	info.clipped = VK_TRUE;
	info.oldSwapchain = old ? *old->m_state->swapchain : nullptr;

	std::array<u32, 2> queue_families = {
		m_renderer->m_state->graphics_queue.index.value(),
		m_renderer->m_state->present_queue.index.value()
	};
	if (queue_families[0] == queue_families[1]) {
		info.imageSharingMode = vk::SharingMode::eExclusive;
	} else {
		info.imageSharingMode = vk::SharingMode::eConcurrent;
		info.queueFamilyIndexCount = 2;
		info.pQueueFamilyIndices = queue_families.data();
	}

	m_state->swapchain = vk::raii::SwapchainKHR(m_renderer->m_state->device, info);

	m_state->images = m_state->swapchain.getImages();

	m_state->image_views.reserve(m_state->images.size());
	for (const auto &image : m_state->images) {
		m_state->image_views.push_back(
			vk::raii::ImageView(
				m_renderer->m_state->device,
				vk::ImageViewCreateInfo()
					.setImage(image)
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(m_state->format->format)
					.setComponents({vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity})
					.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1})
			)
		);
	}
}
