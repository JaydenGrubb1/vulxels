/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/swapchain.h>

#include <algorithm>
#include <array>
#include <limits>

using namespace Vulxels::GFX;

void Swapchain::choose_format(std::vector<vk::SurfaceFormatKHR>& formats) {
	m_format = formats.front();
	for (const auto& format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Srgb
			&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
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

void Swapchain::choose_extent(
	vk::SurfaceCapabilitiesKHR& capabilities,
	vk::Extent2D extent
) {
	if (capabilities.currentExtent.width == std::numeric_limits<u32>::max()) {
		m_extent.width = std::clamp(
			extent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width
		);
		m_extent.height = std::clamp(
			extent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);
	} else {
		m_extent = capabilities.currentExtent;
	}
}

void Swapchain::choose_image_count(vk::SurfaceCapabilitiesKHR& capabilities) {
	m_image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && m_image_count > capabilities.maxImageCount) {
		m_image_count = capabilities.maxImageCount;
	}
}

void Swapchain::create(vk::Extent2D new_extent) {
	auto support = m_device.query_swapchain_support();
	choose_format(support.formats);
	choose_present_mode(support.present_modes);
	choose_extent(support.capabilities, new_extent);
	choose_image_count(support.capabilities);

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

	// TODO: Handle old swapchain

	std::array queue_indices = {
		m_device.graphics_queue().index.value(),
		m_device.present_queue().index.value()
	};

	if (m_device.graphics_queue().index != m_device.present_queue().index) {
		create.imageSharingMode = vk::SharingMode::eConcurrent;
		create.queueFamilyIndexCount = queue_indices.size();
		create.pQueueFamilyIndices = queue_indices.data();
	} else {
		create.imageSharingMode = vk::SharingMode::eExclusive;
	}

	printf("Creating swapchain (%dx%d)\n", m_extent.width, m_extent.height);

	m_swapchain = vk::raii::SwapchainKHR(m_device.device(), create);
	m_images = m_swapchain.getImages();
	m_image_views.reserve(m_images.size());
	for (const auto& image : m_images) {
		m_image_views.push_back(vk::raii::ImageView(
			m_device.device(),
			vk::ImageViewCreateInfo()
				.setImage(image)
				.setViewType(vk::ImageViewType::e2D)
				.setFormat(m_format.format)
				.setComponents({})
				.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1})
		));
	}
}
