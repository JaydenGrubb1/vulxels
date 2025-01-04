/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/device.h>
#include <vulxels/gfx/queue.h>

#include <optional>

using namespace Vulxels::GFX;

Queue::Queue(Device& device, u32 index) : m_index(index) {
	m_queue = device.device().getQueue(index, 0);
}

std::tuple<u32, u32>
Queue::find_families(vk::raii::PhysicalDevice& device, vk::raii::SurfaceKHR& surface) {
	std::optional<u32> graphics;
	std::optional<u32> present;

	auto queue_families = device.getQueueFamilyProperties();
	for (u32 i = 0; i < queue_families.size(); i++) {
		if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			graphics = i;
		}
		if (device.getSurfaceSupportKHR(i, surface)) {
			present = i;
		}
		if (graphics == present) {
			break;
		}
	}

	if (!graphics || !present) {
		throw std::runtime_error("Failed to find suitable queue families");
	}

	return {graphics.value(), present.value()};
}
