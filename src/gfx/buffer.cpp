/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/buffer.h>

#include <cstring>

using namespace Vulxels::GFX;

Buffer::Buffer(
	Device& device,
	vk::DeviceSize size,
	vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags properties
) :
	m_device(device),
	m_size(size),
	m_usage(usage),
	m_properties(properties) {
	m_buffer = vk::raii::Buffer(
		m_device.device(),
		vk::BufferCreateInfo()
			.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setFlags(vk::BufferCreateFlagBits())
	);

	auto requirements = m_buffer.getMemoryRequirements();
	u32 memory_type = find_memory_type(requirements.memoryTypeBits, properties);

	m_memory = vk::raii::DeviceMemory(
		m_device.device(),
		vk::MemoryAllocateInfo()
			.setAllocationSize(requirements.size)
			.setMemoryTypeIndex(memory_type)
	);

	m_buffer.bindMemory(*m_memory, 0);
}

void* Buffer::map(vk::DeviceSize size, vk::DeviceSize offset) {
	return m_memory.mapMemory(offset, size);
}

void Buffer::unmap() {
	m_memory.unmapMemory();
}

void Buffer::flush(vk::DeviceSize size, vk::DeviceSize offset) {
	m_device.device().flushMappedMemoryRanges(
		{vk::MappedMemoryRange().setMemory(*m_memory).setSize(size).setOffset(offset)}
	);
}

void Buffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset) {
	m_device.device().invalidateMappedMemoryRanges(
		{vk::MappedMemoryRange().setMemory(*m_memory).setSize(size).setOffset(offset)}
	);
}

void Buffer::write(void* data, vk::DeviceSize size, vk::DeviceSize offset) {
	auto ptr = map(size, offset);
	std::memcpy(ptr, data, size);
	unmap();
}

u32 Buffer::find_memory_type(u32 type_filter, vk::MemoryPropertyFlags properties) {
	auto mem_props = m_device.physical_device().getMemoryProperties();
	for (u32 i = 0; i < mem_props.memoryTypeCount; i++) {
		if ((type_filter & (1 << i))
			&& (mem_props.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type");
}
