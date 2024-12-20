/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/window.h>

#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Instance {
	  public:
		Instance(Window& window);

		Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;

		vk::raii::Instance& get() { return m_instance; }

	  private:
		vk::raii::Context m_context;
		vk::ApplicationInfo m_appinfo;
		vk::raii::Instance m_instance = nullptr;
	};
} // namespace Vulxels::GFX
