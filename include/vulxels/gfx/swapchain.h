/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>
#include <vulxels/gfx/window.h>

#include <memory>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Swapchain {
	  public:
		Swapchain(Device& device, Window& window);
		~Swapchain() = default;

		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		vk::SurfaceFormatKHR format() {
			return m_format;
		}

		vk::PresentModeKHR present_mode() {
			return m_present_mode;
		}

		vk::Extent2D extent() {
			return m_extent;
		}

		u32 image_count() {
			return m_image_count;
		}

		void set_render_pass(std::shared_ptr<vk::raii::RenderPass> pass);
		void recreate();

	  private:
		Device& m_device;
		Window& m_window;

		vk::raii::SwapchainKHR m_swapchain = nullptr;
		std::vector<vk::Image> m_images;
		std::vector<vk::raii::ImageView> m_image_views;
		std::vector<vk::raii::Framebuffer> m_framebuffers;
		std::shared_ptr<vk::raii::RenderPass> m_render_pass;

		vk::SurfaceFormatKHR m_format;
		vk::PresentModeKHR m_present_mode;
		vk::Extent2D m_extent;
		u32 m_image_count;

		void create_swapchain();
		void create_image_views();
		void create_framebuffers();

		void choose_format(std::vector<vk::SurfaceFormatKHR>& formats);
		void choose_present_mode(std::vector<vk::PresentModeKHR>& modes);
		void choose_image_count(vk::SurfaceCapabilitiesKHR& capabilities);
		bool choose_extent(vk::SurfaceCapabilitiesKHR& capabilities);
	};
} // namespace Vulxels::GFX
