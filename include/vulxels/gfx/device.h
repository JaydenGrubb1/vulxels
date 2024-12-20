/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/instance.h>
#include <vulxels/gfx/window.h>
#include <vulxels/types.h>

#include <cstddef>
#include <optional>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	struct QueueFamily {
		vk::raii::Queue queue = nullptr;
		std::optional<u32> index;
	};

	class Device {
	  public:
		Device(Instance& instance, Window& window);
		~Device() = default;

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

	  private:
		Instance& m_instance;
		vk::raii::SurfaceKHR m_surface = nullptr;
		vk::raii::PhysicalDevice m_physical_device = nullptr;
		vk::raii::Device m_device = nullptr;
		QueueFamily m_graphics_queue;
		QueueFamily m_present_queue;

		void pick_physical_device();
		void find_queue_families();
		void create_logical_device();
	};
} // namespace Vulxels::GFX
