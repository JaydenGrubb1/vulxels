/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>
#include <vulxels/types.h>

#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Buffer {
	  public:
		Buffer(
			Device& device,
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		~Buffer() = default;

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		vk::raii::Buffer& buffer() {
			return m_buffer;
		}

	  private:
		Device& m_device;
		vk::raii::Buffer m_buffer = nullptr;
		vk::raii::DeviceMemory m_memory = nullptr;

		vk::DeviceSize m_size;
		vk::BufferUsageFlags m_usage;
		vk::MemoryPropertyFlags m_properties;

		u32 find_memory_type(u32 type_filter, vk::MemoryPropertyFlags properties);
	};
} // namespace Vulxels::GFX
