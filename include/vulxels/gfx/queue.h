/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

// #include <vulxels/gfx/device.h>
#include <vulxels/types.h>

#include <tuple>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Device;
	class Queue {
	  public:
		static std::tuple<u32, u32>
		find_families(vk::raii::PhysicalDevice& device, vk::raii::SurfaceKHR& surface);

		Queue() = default;
		Queue(Device& device, u32 index);
		~Queue() = default;

		Queue(const Queue&) = delete;
		Queue& operator=(const Queue&) = delete;
		Queue(Queue&&) = default;
		Queue& operator=(Queue&&) = default;

		vk::raii::Queue& queue() {
			return m_queue;
		}

		u32 index() {
			return m_index;
		}

	  private:
		vk::raii::Queue m_queue = nullptr;
		u32 m_index;
	};
} // namespace Vulxels::GFX
