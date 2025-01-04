/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/instance.h>
#include <vulxels/gfx/queue.h>
#include <vulxels/gfx/window.h>
#include <vulxels/types.h>

#include <cstddef>
#include <optional>
#include <set>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	class Device {
	  public:
		Device(Instance& instance, Window& window);
		~Device() = default;

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		vk::raii::SurfaceKHR& surface() {
			return m_surface;
		}

		vk::raii::PhysicalDevice& physical_device() {
			return m_physical_device;
		}

		vk::raii::Device& device() {
			return m_device;
		}

		vk::raii::CommandPool& command_pool() {
			return m_primary_pool;
		}

		Queue& graphics_queue() {
			return m_graphics_queue;
		}

		Queue& present_queue() {
			return m_present_queue;
		}

		void wait_idle() {
			m_device.waitIdle();
		}

		bool wait_for_fence(
			vk::raii::Fence& fence,
			u64 timeout = std::numeric_limits<u64>::max()
		) {
			auto res = m_device.waitForFences({*fence}, VK_TRUE, timeout);
			if (res != vk::Result::eSuccess && res != vk::Result::eTimeout) {
				throw std::runtime_error("Failed to wait for fence");
			}
			return res == vk::Result::eTimeout;
		}

		SwapchainSupportDetails query_swapchain_support() const;
		vk::raii::CommandBuffer* begin_one_time_command();
		void end_one_time_command(vk::raii::CommandBuffer* cmd);

	  private:
		Instance& m_instance;
		vk::raii::SurfaceKHR m_surface = nullptr;
		vk::raii::PhysicalDevice m_physical_device = nullptr;
		vk::raii::Device m_device = nullptr;
		vk::raii::CommandPool m_primary_pool = nullptr;
		vk::raii::CommandPool m_secondary_pool = nullptr;
		Queue m_graphics_queue;
		Queue m_present_queue;

		void pick_physical_device();
		void create_logical_device(std::set<u32> queues);
		void create_command_pool();
	};
} // namespace Vulxels::GFX
