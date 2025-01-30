/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/device.h>
#include <vulxels/log.h>

#include <cstdio>
#include <stdexcept>
#include <vector>

using namespace Vulxels::GFX;

static constexpr std::array DEVICE_EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

Device::Device(Instance& instance, const Window& window) : m_instance(instance) {
	m_surface = window.create_surface(m_instance.instance());

	pick_physical_device();

	auto [graphics_idx, present_idx] = Queue::find_families(m_physical_device, m_surface);

	create_logical_device({graphics_idx, present_idx});

	m_graphics_queue = Queue(*this, graphics_idx);
	m_present_queue = Queue(*this, present_idx);

	create_command_pool();
}

vk::raii::CommandBuffer* Device::begin_one_time_command() const {
	vk::raii::CommandBuffers cmds(
		m_device,
		vk::CommandBufferAllocateInfo()
			.setCommandPool(m_secondary_pool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1)
	);
	const auto cmd = new vk::raii::CommandBuffer(std::move(cmds.front()));
	cmd->begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
	return cmd;
}

void Device::end_one_time_command(const vk::raii::CommandBuffer* cmd) const {
	cmd->end();
	m_graphics_queue.submit(vk::SubmitInfo().setCommandBuffers({**cmd}));
	m_graphics_queue.wait_idle();
	delete cmd;
}

void Device::pick_physical_device() {
	m_physical_device = m_instance.instance().enumeratePhysicalDevices().front();
	// TODO: Select the best physical device
	VX_LOG("Using physical device: {}", m_physical_device.getProperties().deviceName.data());
}

void Device::create_logical_device(const std::set<u32>& queues) {
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;

	constexpr f32 queue_priority = 1.0f;
	for (const u32 idx : queues) {
		queue_create_infos.push_back(
			vk::DeviceQueueCreateInfo().setQueueFamilyIndex(idx).setQueueCount(1).setPQueuePriorities(&queue_priority)
		);
	}

	vk::PhysicalDeviceFeatures device_features;
	device_features.setSamplerAnisotropy(true);

	m_device = vk::raii::Device(
		m_physical_device,
		vk::DeviceCreateInfo()
			.setPEnabledLayerNames(VALIDATION_LAYERS)
			.setPEnabledExtensionNames(DEVICE_EXTENSIONS)
			.setQueueCreateInfos(queue_create_infos)
			.setPEnabledFeatures(&device_features)
	);
}

void Device::create_command_pool() {
	m_primary_pool = vk::raii::CommandPool(
		m_device,
		vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(m_graphics_queue.index())
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
	);
	m_secondary_pool = vk::raii::CommandPool(
		m_device,
		vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(m_graphics_queue.index())
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient)
	);
}

SwapchainSupportDetails Device::query_swapchain_support() const {
	SwapchainSupportDetails details;
	details.capabilities = m_physical_device.getSurfaceCapabilitiesKHR(m_surface);
	details.formats = m_physical_device.getSurfaceFormatsKHR(m_surface);
	details.present_modes = m_physical_device.getSurfacePresentModesKHR(m_surface);
	return details;
}
