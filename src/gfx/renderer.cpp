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
		m_frame_ready.emplace_back(m_device.device(), vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
		m_image_available.emplace_back(m_device.device(), vk::SemaphoreCreateInfo());
		m_render_finished.emplace_back(m_device.device(), vk::SemaphoreCreateInfo());
	}
}

Renderer::~Renderer() {
	m_device.wait_idle();
}

vk::raii::CommandBuffer* Renderer::begin_frame() {
	m_device.wait_for_fence(m_frame_ready[m_current_frame]);
	if (!m_swapchain.acquire(m_image_available[m_current_frame])) {
		return nullptr;
	}
	m_device.device().resetFences({*m_frame_ready[m_current_frame]});
	auto& cmd = m_commands[m_current_frame];
	cmd.begin(vk::CommandBufferBeginInfo());
	return &cmd;
}

void Renderer::end_frame(const vk::raii::CommandBuffer* cmd) {
	cmd->end();
	constexpr vk::PipelineStageFlags wait_stages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	m_device.graphics_queue().submit(
		vk::SubmitInfo()
			.setCommandBuffers(**cmd)
			.setWaitSemaphores(*m_image_available[m_current_frame])
			.setWaitDstStageMask(wait_stages)
			.setSignalSemaphores(*m_render_finished[m_current_frame]),
		*m_frame_ready[m_current_frame]
	);
	m_swapchain.present(m_render_finished[m_current_frame]);
	m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::handle_events(const SDL_Event& event) {
	switch (event.type) {
		case SDL_EVENT_WINDOW_RESIZED:
			m_swapchain.set_resized();
			break;
		default:;
	}
}
