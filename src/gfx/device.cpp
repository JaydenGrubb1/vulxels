/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/device.h>

#include <cstdio>
#include <set>
#include <stdexcept>
#include <vector>

using namespace Vulxels::GFX;

Device::Device(Instance& instance, Window& window) : m_instance(instance) {
	m_surface = window.create_surface(m_instance.get());

	pick_physical_device();
	find_queue_families();
	create_logical_device();

	m_graphics_queue.queue = m_device.getQueue(m_graphics_queue.index.value(), 0);
	m_present_queue.queue = m_device.getQueue(m_present_queue.index.value(), 0);
}

void Device::pick_physical_device() {
	m_physical_device = m_instance.get().enumeratePhysicalDevices().front();
	// TODO: Select the best physical device

	printf(
		"Using physical device: %s\n",
		m_physical_device.getProperties().deviceName.data()
	);
}

void Device::find_queue_families() {
	auto queue_families = m_physical_device.getQueueFamilyProperties();
	for (u32 i = 0; i < queue_families.size(); i++) {
		if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			m_graphics_queue.index = i;
		}
		if (m_physical_device.getSurfaceSupportKHR(i, m_surface)) {
			m_present_queue.index = i;
		}
		if (m_graphics_queue.index == m_present_queue.index) {
			break;
		}
	}

	if (!m_graphics_queue.index || !m_present_queue.index) {
		throw std::runtime_error("Failed to find suitable queue families");
	}
}

void Device::create_logical_device() {
	std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
	std::set<u32> unique_queue_families = {
		m_graphics_queue.index.value(),
		m_present_queue.index.value()
	};

	f32 queue_priority = 1.0f;
	for (u32 queue_family : unique_queue_families) {
		queue_create_infos.push_back(vk::DeviceQueueCreateInfo()
										 .setQueueFamilyIndex(queue_family)
										 .setQueueCount(1)
										 .setPQueuePriorities(&queue_priority));
	}

	vk::PhysicalDeviceFeatures device_features;
	device_features.setSamplerAnisotropy(true);

	m_device = vk::raii::Device(
		m_physical_device,
		vk::DeviceCreateInfo()
			.setPEnabledLayerNames(nullptr)
			.setPEnabledExtensionNames(nullptr)
			.setQueueCreateInfos(queue_create_infos)
			.setPEnabledFeatures(&device_features)
	);
}
