/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/renderer.h>

using namespace Vulxels::GFX;

Renderer::Renderer(Window& window) : m_window(window) {
	m_commands = vk::raii::CommandBuffers(
		m_device.device(),
		vk::CommandBufferAllocateInfo()
			.setCommandPool(m_device.command_pool())
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT)
	);

	m_frame_ready.reserve(MAX_FRAMES_IN_FLIGHT);
	m_image_available.reserve(MAX_FRAMES_IN_FLIGHT);
	m_render_finished.reserve(MAX_FRAMES_IN_FLIGHT);

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		m_frame_ready.push_back(vk::raii::Fence(
			m_device.device(),
			vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled)
		));
		m_image_available.push_back(
			vk::raii::Semaphore(m_device.device(), vk::SemaphoreCreateInfo())
		);
		m_render_finished.push_back(
			vk::raii::Semaphore(m_device.device(), vk::SemaphoreCreateInfo())
		);
	}
}

Renderer::~Renderer() {
	m_device.wait_idle();
}

vk::raii::CommandBuffer* Renderer::begin_frame() {
	(void)m_device.device().waitForFences(
		{*m_frame_ready[m_current_frame]},
		VK_TRUE,
		std::numeric_limits<u64>::max()
	);
	if (!m_swapchain.acquire(m_image_available[m_current_frame])) {
		return nullptr;
	}
	m_device.device().resetFences({*m_frame_ready[m_current_frame]});
	auto& cmd = m_commands[m_current_frame];
	cmd.reset();
	cmd.begin(vk::CommandBufferBeginInfo());
	return &cmd;
}

void Renderer::end_frame(vk::raii::CommandBuffer* cmd) {
	cmd->end();
	vk::PipelineStageFlags wait_stages[] = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};
	m_device.graphics_queue().queue.submit(
		{vk::SubmitInfo()
			 .setCommandBuffers(**cmd)
			 .setWaitSemaphores(*m_image_available[m_current_frame])
			 .setWaitDstStageMask(wait_stages)
			 .setSignalSemaphores(*m_render_finished[m_current_frame])},
		*m_frame_ready[m_current_frame]
	);
	m_swapchain.present(m_render_finished[m_current_frame]);
	m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}
