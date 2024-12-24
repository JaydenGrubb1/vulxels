/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/pipeline.h>

using namespace Vulxels::GFX;

Pipeline::Builder& Pipeline::Builder::use_default() {
	return (*this)
		.set_topology(vk::PrimitiveTopology::eTriangleList)
		.set_primitive_restart(false)
		.set_viewport_count(1) // TODO: Check if this is necessary
		.set_scissor_count(1) // TODO: Check if this is necessary
		.set_polygon_mode(vk::PolygonMode::eFill)
		.set_front_face(vk::FrontFace::eCounterClockwise)
		.set_cull_mode(vk::CullModeFlagBits::eBack)
		.set_cull_mode(vk::CullModeFlagBits::eNone) // TODO: Remove this line
		.set_line_width(1.0f)
		.add_color_blend_attachment(
			vk::PipelineColorBlendAttachmentState()
				.setColorWriteMask(
					vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
					| vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
				)
				.setBlendEnable(false)
		)
		.add_dynamic_state(vk::DynamicState::eViewport)
		.add_dynamic_state(vk::DynamicState::eScissor);
	// TODO: Set default values for other pipeline states
}

Pipeline::Pipeline(Device& device, Builder& builder) {
	builder.color_blend_state.setAttachments(builder.color_blend_attachments);
	builder.dynamic_state.setDynamicStates(builder.dynamic_states);

	// TODO: Only create a new pipeline layout if there are no existing compatible layouts
	m_layout = vk::raii::PipelineLayout(
		device.device(),
		vk::PipelineLayoutCreateInfo()
			.setSetLayouts(builder.descriptor_set_layouts)
			.setPushConstantRanges(builder.push_constant_ranges)
	);

	// TODO: Handle base pipeline handle

	m_pipeline = vk::raii::Pipeline(
		device.device(),
		nullptr,
		vk::GraphicsPipelineCreateInfo()
			.setStages(builder.shader_stages)
			.setPVertexInputState(&builder.vertex_input_state)
			.setPInputAssemblyState(&builder.input_assembly_state)
			.setPViewportState(&builder.viewport_state)
			.setPRasterizationState(&builder.rasterization_state)
			.setPMultisampleState(&builder.multisample_state)
			.setPDepthStencilState(&builder.depth_stencil_state)
			.setPColorBlendState(&builder.color_blend_state)
			.setPDynamicState(&builder.dynamic_state)
			.setLayout(m_layout)
			.setRenderPass(builder.render_pass)
	);
}
