/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/app.h>
#include <vulxels/gfx/buffer.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

#include <cstdio>
#include <glm/glm.hpp>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace Vulxels;

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

static std::shared_ptr<vk::raii::RenderPass> s_render_pass;
static std::shared_ptr<GFX::Pipeline> s_pipeline;
static std::shared_ptr<GFX::Buffer> s_vertex_buffer;
static std::shared_ptr<GFX::Buffer> s_index_buffer;

static std::vector<Vertex> s_vertices = {
	{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
	{{-1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
	{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
	{{1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
};
static std::vector<u32> s_indices = {0, 1, 2, 2, 3, 0};

App::App() {
	auto vert = m_renderer.create_shader("simple.vert.spv");
	auto frag = m_renderer.create_shader("simple.frag.spv");

	s_render_pass = std::make_shared<vk::raii::RenderPass>(
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

	m_renderer.swapchain().set_render_pass(s_render_pass);

	s_pipeline = std::make_shared<GFX::Pipeline>(
		m_renderer.device(),
		m_renderer.create_pipeline()
			.use_default()
			.add_shader_stage(vert.module(), vk::ShaderStageFlagBits::eVertex)
			.add_shader_stage(frag.module(), vk::ShaderStageFlagBits::eFragment)
			.add_vertex_binding_description(
				0,
				sizeof(Vertex),
				vk::VertexInputRate::eVertex
			)
			.add_vertex_attribute_description(
				0,
				0,
				vk::Format::eR32G32Sfloat,
				offsetof(Vertex, pos)
			)
			.add_vertex_attribute_description(
				0,
				1,
				vk::Format::eR32G32B32Sfloat,
				offsetof(Vertex, color)
			)
			.set_render_pass(s_render_pass)
	);

	s_vertex_buffer = std::make_shared<GFX::Buffer>(
		m_renderer.device(),
		sizeof(Vertex) * s_vertices.size(),
		vk::BufferUsageFlagBits::eVertexBuffer
	);
	s_index_buffer = std::make_shared<GFX::Buffer>(
		m_renderer.device(),
		sizeof(u32) * s_indices.size(),
		vk::BufferUsageFlagBits::eIndexBuffer
	);

	s_vertex_buffer->write(s_vertices);
	s_index_buffer->write(s_indices);
}

static void draw(GFX::Renderer& renderer) {
	auto cmd = renderer.begin_frame();
	if (!cmd) {
		return;
	}

	cmd->beginRenderPass(
		vk::RenderPassBeginInfo()
			.setRenderPass(*s_render_pass)
			.setFramebuffer(renderer.swapchain().framebuffer())
			.setRenderArea({{0, 0}, renderer.swapchain().extent()})
			.setClearValues({vk::ClearValue().setColor({0.0f, 0.0f, 0.0f, 1.0f})}),
		vk::SubpassContents::eInline
	);

	cmd->setViewport(
		0,
		{vk::Viewport()
			 .setX(0.0f)
			 .setY(0.0f)
			 .setWidth(static_cast<f32>(renderer.swapchain().extent().width))
			 .setHeight(static_cast<f32>(renderer.swapchain().extent().height))
			 .setMinDepth(0.0f)
			 .setMaxDepth(1.0f)}
	);
	cmd->setScissor(
		0,
		{vk::Rect2D().setOffset({0, 0}).setExtent(renderer.swapchain().extent())}
	);

	cmd->bindVertexBuffers(0, {s_vertex_buffer->buffer()}, {0});
	cmd->bindIndexBuffer(s_index_buffer->buffer(), 0, vk::IndexType::eUint32);
	cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, s_pipeline->pipeline());
	cmd->drawIndexed(s_indices.size(), 1, 0, 0, 0);

	cmd->endRenderPass();

	renderer.end_frame(cmd);
}

App::~App() {
	m_renderer.device().wait_idle();
	s_index_buffer.reset();
	s_vertex_buffer.reset();
	s_pipeline.reset();
	s_render_pass.reset();
}

void App::run() {
	SDL_Event e;

	while (m_running) {
		while (SDL_PollEvent(&e) != 0) {
			m_renderer.handle_events(e);
			if (e.type == SDL_QUIT) {
				m_running = false;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
				m_running = false;
			}
		}

		draw(m_renderer);
	}
}
