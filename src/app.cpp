/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulxels/app.h>
#include <vulxels/gfx/buffer.h>
#include <vulxels/gfx/descriptors.h>
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
static std::shared_ptr<GFX::DescriptorPool> s_imgui_pool;

static std::vector<Vertex> s_vertices = {
	{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
	{{-1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
	{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
	{{1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
};
static std::vector<u32> s_indices = {0, 1, 2, 2, 3, 0};

static void check_vk_result(VkResult err) {
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

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

	s_imgui_pool = std::make_shared<GFX::DescriptorPool>(m_renderer.device());
	s_imgui_pool->set_max_sets(1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eSampler, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eCombinedImageSampler, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eSampledImage, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eStorageImage, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eUniformTexelBuffer, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eStorageTexelBuffer, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eUniformBuffer, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eStorageBuffer, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eUniformBufferDynamic, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eStorageBufferDynamic, 1000);
	s_imgui_pool->add_pool_size(vk::DescriptorType::eInputAttachment, 1000);
	s_imgui_pool->create();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForVulkan(m_window.window());
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = *m_renderer.instance().instance();
	init_info.PhysicalDevice = *m_renderer.device().physical_device();
	init_info.Device = *m_renderer.device().device();
	init_info.QueueFamily = m_renderer.device().graphics_queue().index();
	init_info.Queue = *m_renderer.device().graphics_queue().queue();
	init_info.DescriptorPool = *s_imgui_pool->pool();
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = m_renderer.swapchain().image_count();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.RenderPass = **s_render_pass;
	init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info);
}

static void draw_gui() {
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::Begin("Statistics")) {
		ImGui::Text(
			"Vulxels %d.%d.%d",
			VULXELS_VERSION_MAJOR,
			VULXELS_VERSION_MINOR,
			VULXELS_VERSION_PATCH
		);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	}
	ImGui::End();
}

static void draw(GFX::Renderer& renderer) {
	auto cmd = renderer.begin_frame();
	if (!cmd) {
		return;
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	draw_gui();

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

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), **cmd);

	cmd->endRenderPass();
	renderer.end_frame(cmd);
}

App::~App() {
	m_renderer.device().wait_idle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	s_imgui_pool.reset();
	s_index_buffer.reset();
	s_vertex_buffer.reset();
	s_pipeline.reset();
	s_render_pass.reset();
}

void App::run() {
	SDL_Event e;

	while (m_running) {
		while (SDL_PollEvent(&e) != 0) {
			ImGui_ImplSDL2_ProcessEvent(&e);
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
