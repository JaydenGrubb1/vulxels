/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <array>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
#ifdef NDEBUG
	static constexpr const std::array<const char*, 0> VALIDATION_LAYERS;
#else
	static constexpr const std::array VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
#endif

	class Instance {
	  public:
		Instance();

		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		vk::raii::Instance& instance() {
			return m_instance;
		}

	  private:
		vk::raii::Context m_context;
		vk::ApplicationInfo m_appinfo;
		vk::raii::Instance m_instance = nullptr;
	};
} // namespace Vulxels::GFX
