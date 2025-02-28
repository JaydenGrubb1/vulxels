/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/swapchain.h>
#include <vulxels/log.h>

#include <algorithm>
#include <array>
#include <limits>

using namespace Vulxels::GFX;

Swapchain::Swapchain(Device& device, Window& window) : m_device(device), m_window(window) {
	auto support = m_device.query_swapchain_support();
	choose_format(support.formats);
	choose_present_mode(support.present_modes);
	choose_image_count(support.capabilities);
	choose_extent(support.capabilities);

	create_swapchain();
	create_image_views();
}

void Swapchain::set_render_pass(const std::shared_ptr<vk::raii::RenderPass>& pass) {
	// TODO: don't recreate framebuffers if render pass is compatible
	m_render_pass = pass;
	create_framebuffers();
}

void Swapchain::set_resized() {
	m_resized = true;
}

void Swapchain::recreate() {
	m_device.wait_idle();
	m_resized = false;
	const auto support = m_device.query_swapchain_support();
	if (choose_extent(support.capabilities)) {
		create_swapchain();
		create_image_views();
		create_framebuffers();
	}
}

bool Swapchain::acquire(const vk::raii::Semaphore& signal) {
	try {
		auto [res, idx] = m_swapchain.acquireNextImage(std::numeric_limits<u64>::max(), *signal);
		m_current_image = idx;
		if (res == vk::Result::eSuboptimalKHR) {
			m_resized = true;
		}
		return true;
	} catch ([[maybe_unused]] vk::OutOfDateKHRError& e) {
		recreate();
		return false;
	}
}

bool Swapchain::present(const vk::raii::Semaphore& wait) {
	try {
		const auto res = m_device.present_queue().present(
			vk::PresentInfoKHR().setSwapchains(*m_swapchain).setImageIndices(m_current_image).setWaitSemaphores(*wait)
		);
		if (res == vk::Result::eSuboptimalKHR || m_resized) {
			recreate();
			return false;
		}
	} catch ([[maybe_unused]] vk::OutOfDateKHRError& e) {
		recreate();
		return false;
	}
	return true;
}

void Swapchain::create_swapchain() {
	vk::SwapchainCreateInfoKHR create {};
	create.surface = m_device.surface();
	create.minImageCount = m_image_count;
	create.imageFormat = m_format.format;
	create.imageColorSpace = m_format.colorSpace;
	create.imageExtent = m_extent;
	create.imageArrayLayers = 1;
	create.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	create.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	create.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	create.presentMode = m_present_mode;
	create.clipped = VK_TRUE;
	create.oldSwapchain = nullptr;

	if (*m_swapchain) {
		create.oldSwapchain = *m_swapchain;
	} else {
		create.oldSwapchain = nullptr;
	}

	const std::array queue_indices = {m_device.graphics_queue().index(), m_device.present_queue().index()};

	if (m_device.graphics_queue().index() != m_device.present_queue().index()) {
		create.imageSharingMode = vk::SharingMode::eConcurrent;
		create.queueFamilyIndexCount = queue_indices.size();
		create.pQueueFamilyIndices = queue_indices.data();
	} else {
		create.imageSharingMode = vk::SharingMode::eExclusive;
	}

	auto temp = vk::raii::SwapchainKHR(m_device.device(), create);
	m_swapchain = std::move(temp);

	VX_DEBUG("Created swapchain ({}x{})", m_extent.width, m_extent.height);
}

void Swapchain::create_image_views() {
	m_images = m_swapchain.getImages();
	m_image_views.clear();
	m_image_views.reserve(m_images.size());

	for (const auto& image : m_images) {
		m_image_views.emplace_back(
			m_device.device(),
			vk::ImageViewCreateInfo()
				.setImage(image)
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(m_format.format)
				.setComponents({})
				.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1})

		);
	}
}

void Swapchain::create_framebuffers() {
	if (!m_render_pass) {
		return;
	}

	m_framebuffers.clear();
	m_framebuffers.reserve(m_images.size());

	for (const auto& view : m_image_views) {
		std::array attachments = {*view};
		m_framebuffers.emplace_back(
			m_device.device(),
			vk::FramebufferCreateInfo()
				.setRenderPass(*m_render_pass)
				.setAttachments(attachments)
				.setWidth(m_extent.width)
				.setHeight(m_extent.height)
				.setLayers(1)

		);
	}
}

void Swapchain::choose_format(std::vector<vk::SurfaceFormatKHR>& formats) {
	m_format = formats.front();
	for (const auto& format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			m_format = format;
			break;
		}
	}
}

void Swapchain::choose_present_mode(std::vector<vk::PresentModeKHR>& modes) {
	m_present_mode = vk::PresentModeKHR::eFifo;
	for (const auto& mode : modes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			m_present_mode = mode;
			break;
		}
	}
}

void Swapchain::choose_image_count(const vk::SurfaceCapabilitiesKHR& capabilities) {
	m_image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && m_image_count > capabilities.maxImageCount) {
		m_image_count = capabilities.maxImageCount;
	}
}

bool Swapchain::choose_extent(const vk::SurfaceCapabilitiesKHR& capabilities) {
	const auto old = m_extent;
	if (capabilities.currentExtent.width == std::numeric_limits<u32>::max()) {
		const auto window_extent = m_window.extent();
		m_extent.width =
			std::clamp(window_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		m_extent.height =
			std::clamp(window_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	} else {
		m_extent = capabilities.currentExtent;
	}
	return old != m_extent;
}
