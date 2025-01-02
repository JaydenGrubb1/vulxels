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
		static constexpr u32 MAX_FRAMES_IN_FLIGHT = 2;

		Renderer(Window& window);
		~Renderer();

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

		u32 m_current_frame = 0;
		vk::raii::CommandBuffers m_commands = nullptr;
		std::vector<vk::raii::Fence> m_frame_ready;
		std::vector<vk::raii::Semaphore> m_image_available;
		std::vector<vk::raii::Semaphore> m_render_finished;

	  public:
		// TODO: make these private
		vk::raii::CommandBuffer* begin_frame();
		void end_frame(vk::raii::CommandBuffer* cmd);
	};
} // namespace Vulxels::GFX
