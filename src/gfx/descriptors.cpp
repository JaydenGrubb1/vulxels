/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/descriptors.h>

using namespace Vulxels::GFX;

void DescriptorLayout::add_binding(
	const u32 binding,
	const vk::DescriptorType type,
	const vk::ShaderStageFlags stages,
	const u32 count,
	const vk::Sampler* samplers
) {
	m_bindings.push_back(
		vk::DescriptorSetLayoutBinding()
			.setBinding(binding)
			.setDescriptorType(type)
			.setDescriptorCount(count)
			.setStageFlags(stages)
			.setPImmutableSamplers(samplers)
	);
}

void DescriptorLayout::create() {
	m_layout =
		vk::raii::DescriptorSetLayout(m_device.device(), vk::DescriptorSetLayoutCreateInfo().setBindings(m_bindings));
}

void DescriptorPool::create() {
	m_pool = vk::raii::DescriptorPool(
		m_device.device(),
		vk::DescriptorPoolCreateInfo()
			.setMaxSets(m_max_sets)
			.setPoolSizes(m_sizes)
			.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
	);
}

DescriptorSet DescriptorPool::allocate(DescriptorLayout& layout) const {
	auto sets = vk::raii::DescriptorSets(
		m_device.device(),
		vk::DescriptorSetAllocateInfo().setDescriptorPool(m_pool).setSetLayouts(*layout.layout())
	);
	return DescriptorSet {std::move(sets.front())};
}

void DescriptorSet::bind_buffer(
	const u32 binding,
	const vk::raii::Buffer& buffer,
	const vk::DeviceSize range,
	const vk::DeviceSize offset
) {
	m_buffer_infos.push_back(vk::DescriptorBufferInfo().setBuffer(buffer).setOffset(offset).setRange(range));
	m_writes.push_back(
		vk::WriteDescriptorSet()
			.setDstSet(m_set)
			.setDstBinding(binding)
			.setDstArrayElement(0)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setBufferInfo(m_buffer_infos.back())
	);
}

void DescriptorSet::write() const {
	m_set.getDevice().updateDescriptorSets(m_writes, {});
}
