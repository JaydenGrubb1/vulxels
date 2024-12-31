/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>
#include <vulxels/gfx/instance.h>
#include <vulxels/gfx/pipeline.h>
#include <vulxels/gfx/shader.h>
#include <vulxels/gfx/swapchain.h>
#include <vulxels/gfx/window.h>

namespace Vulxels::GFX {
	class Renderer {
	  public:
		Renderer(Window& window) : m_window(window) {}
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		Instance& instance() {
			return m_instance;
		}

		Device& device() {
			return m_device;
		}

		Swapchain& swapchain() {
			return m_swapchain;
		}

		Shader create_shader(std::string_view path) {
			return Shader(m_device, path);
		}

		Pipeline::Builder create_pipeline() {
			return Pipeline::Builder(m_device);
		}

	  private:
		Window& m_window;
		Instance m_instance {m_window};
		Device m_device {m_instance, m_window};
		Swapchain m_swapchain {m_device, m_window};
	};
} // namespace Vulxels::GFX
