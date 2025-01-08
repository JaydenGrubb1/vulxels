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
	class DescriptorLayout {
	  public:
		DescriptorLayout(Device& device) : m_device(device) {}
		~DescriptorLayout() = default;

		DescriptorLayout(const DescriptorLayout&) = delete;
		DescriptorLayout& operator=(const DescriptorLayout&) = delete;

		vk::raii::DescriptorSetLayout& layout() {
			return m_layout;
		}

		void add_binding(
			u32 binding,
			vk::DescriptorType type,
			vk::ShaderStageFlags stages,
			u32 count = 1,
			const vk::Sampler* samplers = nullptr
		);

		void create();

	  private:
		Device& m_device;
		vk::raii::DescriptorSetLayout m_layout = nullptr;
		std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
	};

	class DescriptorPool;

	class DescriptorSet {
	  public:
		DescriptorSet() = delete;
		~DescriptorSet() = default;

		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;
		DescriptorSet(DescriptorSet&&) = default;
		DescriptorSet& operator=(DescriptorSet&&) = default;

		vk::raii::DescriptorSet& set() {
			return m_set;
		}

		void bind_buffer(
			u32 binding,
			vk::raii::Buffer& buffer,
			vk::DeviceSize range,
			vk::DeviceSize offset = 0
		);
		// TODO: Add other bind methods

		void write();

	  private:
		vk::raii::DescriptorSet m_set = nullptr;
		std::vector<vk::WriteDescriptorSet> m_writes;
		std::vector<vk::DescriptorBufferInfo> m_buffer_infos;

		DescriptorSet(vk::raii::DescriptorSet&& set) : m_set(std::move(set)) {}

		friend class DescriptorPool;
	};

	class DescriptorPool {
	  public:
		DescriptorPool(Device& device) : m_device(device) {}
		~DescriptorPool() = default;

		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorLayout&) = delete;

		vk::raii::DescriptorPool& pool() {
			return m_pool;
		}

		void add_pool_size(vk::DescriptorType type, u32 size) {
			m_sizes.push_back({type, size});
		}

		void set_max_sets(u32 max) {
			m_max_sets = max;
		}

		void create();

		DescriptorSet allocate(DescriptorLayout& layout);
		// TODO: Add a method to allocate multiple descriptor sets

	  private:
		Device& m_device;
		vk::raii::DescriptorPool m_pool = nullptr;
		std::vector<vk::DescriptorPoolSize> m_sizes;
		u32 m_max_sets;
	};
} // namespace Vulxels::GFX
