/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>
#include <vulxels/types.h>

#include <iterator>
#include <ranges>
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
		Buffer(Buffer&&) = default;
		Buffer& operator=(Buffer&&) = default;

		vk::raii::Buffer& buffer() {
			return m_buffer;
		}

		void* map(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
		void unmap();
		void flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
		void invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);
		void write(void* data, vk::DeviceSize size, vk::DeviceSize offset = 0);

		template<typename Iter>
			requires std::contiguous_iterator<Iter>
		void write(Iter begin, Iter end, vk::DeviceSize offset = 0) {
			write(
				std::addressof(*begin),
				std::distance(begin, end) * sizeof(*begin),
				offset
			);
		}

		template<typename R>
			requires std::ranges::contiguous_range<R>
		void write(R&& range, vk::DeviceSize offset = 0) {
			write(std::ranges::begin(range), std::ranges::end(range), offset);
		}

	  private:
		Device& m_device;
		vk::raii::Buffer m_buffer = nullptr;
		vk::raii::DeviceMemory m_memory = nullptr;

		vk::DeviceSize m_size;
		vk::BufferUsageFlags m_usage;
		vk::MemoryPropertyFlags m_properties;

		Buffer* m_staging = nullptr;
		vk::DeviceSize m_staging_size = 0;
		vk::DeviceSize m_staging_offset = 0;

		u32 find_memory_type(u32 type_filter, vk::MemoryPropertyFlags properties);
	};
} // namespace Vulxels::GFX
