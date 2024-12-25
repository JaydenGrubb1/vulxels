/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/app.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

#include <cstdio>
#include <glm/glm.hpp>
#include <stdexcept>
#include <thread>

using namespace Vulxels;

App::App() {
	auto vert = m_renderer.create_shader("simple.vert.spv");
	auto frag = m_renderer.create_shader("simple.frag.spv");

	vk::raii::RenderPass pass(
		m_renderer.device().device(),
		vk::RenderPassCreateInfo()
			.setAttachments({vk::AttachmentDescription()
								 .setFormat(m_renderer.swapchain().format().format)
								 .setSamples(vk::SampleCountFlagBits::e1)
								 .setLoadOp(vk::AttachmentLoadOp::eClear)
								 .setStoreOp(vk::AttachmentStoreOp::eStore)
								 .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
								 .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
								 .setInitialLayout(vk::ImageLayout::eUndefined)
								 .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)})
			.setSubpasses({vk::SubpassDescription()
							   .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
							   .setColorAttachments(
								   {vk::AttachmentReference().setAttachment(0).setLayout(
									   vk::ImageLayout::eColorAttachmentOptimal
								   )}
							   )})
			.setDependencies(
				{vk::SubpassDependency()
					 .setSrcSubpass(VK_SUBPASS_EXTERNAL)
					 .setDstSubpass(0)
					 .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
					 .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
					 .setSrcAccessMask(vk::AccessFlagBits::eNoneKHR)
					 .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
					 .setDependencyFlags(vk::DependencyFlagBits::eByRegion)}
			)
	);

	struct Vert {
		glm::vec2 pos;
		glm::vec3 color;
	};

	auto pipeline =
		m_renderer.create_pipeline()
			.use_default()
			.add_shader_stage(vert.module(), vk::ShaderStageFlagBits::eVertex)
			.add_shader_stage(frag.module(), vk::ShaderStageFlagBits::eFragment)
			.add_vertex_binding_description(0, sizeof(Vert), vk::VertexInputRate::eVertex)
			.add_vertex_attribute_description(
				0,
				0,
				vk::Format::eR32G32Sfloat,
				offsetof(Vert, pos)
			)
			.add_vertex_attribute_description(
				0,
				1,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vert, color)
			)
			.set_render_pass(std::move(pass))
			.build();
}

App::~App() {
	// TODO
}

static void fixed_update() {
	// Placeholder for fixed update
}

static void render_update(float delta) {
	// Placeholder for render update
	(void)delta;
}

static void late_update(float delta) {
	// Placeholder for late update
	(void)delta;
}

void App::run() {
	auto fixed_loop = std::thread([&]() {
		while (m_running) {
			fixed_update();
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	});

	SDL_Event e;
	f32 last_time = static_cast<f32>(SDL_GetTicks());

	while (m_running) {
		f32 current_time = static_cast<f32>(SDL_GetTicks());
		f32 delta = current_time - last_time;
		last_time = current_time;

		render_update(delta);
		late_update(delta);

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				m_running = false;
			}
		}
	}

	fixed_loop.join();
}
