/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/window.h>

#include <array>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	static constexpr const std::array VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

	class Instance {
	  public:
		Instance(Window& window);

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
