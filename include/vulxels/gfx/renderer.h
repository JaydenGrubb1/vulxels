/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>
#include <vulxels/gfx/instance.h>
#include <vulxels/gfx/swapchain.h>
#include <vulxels/gfx/window.h>

namespace Vulxels::GFX {
	class Renderer {
	  public:
		Renderer(Window& window) : m_window(window) {
			m_swapchain.create(m_window.extent());
		}
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	  private:
		Window& m_window;
		Instance m_instance {m_window};
		Device m_device {m_instance, m_window};
		Swapchain m_swapchain {m_device};
	};
} // namespace Vulxels::GFX
