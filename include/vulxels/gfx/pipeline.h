/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>

#include <memory>
#include <string_view>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Pipeline {
	  public:
		struct Builder {
			explicit Builder(Device& device) : m_device(device) {}
			~Builder() = default;

			Builder(const Builder&) = delete;
			Builder& operator=(const Builder&) = delete;

			Builder& add_shader_stage(
				const vk::ShaderModule module,
				const vk::ShaderStageFlagBits stage,
				const std::string_view entry = "main"
			) {
				vk::PipelineShaderStageCreateInfo shader_stage;
				shader_stage.setStage(stage);
				shader_stage.setModule(module);
				shader_stage.setPName(entry.data());
				shader_stages.push_back(shader_stage);
				return *this;
			}

			Builder&
			add_vertex_binding_description(const u32 binding, const u32 stride, const vk::VertexInputRate rate) {
				vertex_bindings.push_back(
					vk::VertexInputBindingDescription().setBinding(binding).setStride(stride).setInputRate(rate)
				);
				return *this;
			}

			Builder& add_vertex_attribute_description(
				const u32 binding,
				const u32 location,
				const vk::Format format,
				const u32 offset
			) {
				vertex_attributes.push_back(
					vk::VertexInputAttributeDescription()
						.setBinding(binding)
						.setLocation(location)
						.setFormat(format)
						.setOffset(offset)
				);
				return *this;
			}

			Builder& set_topology(const vk::PrimitiveTopology topology) {
				input_assembly_state.setTopology(topology);
				return *this;
			}

			Builder& set_primitive_restart(const bool enable) {
				input_assembly_state.setPrimitiveRestartEnable(enable);
				return *this;
			}

			Builder& set_viewport_count(const u32 count) {
				viewport_state.setViewportCount(count);
				return *this;
			}

			Builder& set_scissor_count(const u32 count) {
				viewport_state.setScissorCount(count);
				return *this;
			}

			Builder& set_viewports(const std::vector<vk::Viewport>& viewports) {
				viewport_state.setViewports(viewports);
				return *this;
			}

			Builder& set_scissors(const std::vector<vk::Rect2D>& scissors) {
				viewport_state.setScissors(scissors);
				return *this;
			}

			Builder& enable_depth_clamp(const bool enable) {
				rasterization_state.setDepthClampEnable(enable);
				return *this;
			}

			Builder& enable_rasterizer_discard(const bool enable) {
				rasterization_state.setRasterizerDiscardEnable(enable);
				return *this;
			}

			Builder& set_polygon_mode(const vk::PolygonMode mode) {
				rasterization_state.setPolygonMode(mode);
				return *this;
			}

			Builder& set_cull_mode(const vk::CullModeFlags mode) {
				rasterization_state.setCullMode(mode);
				return *this;
			}

			Builder& set_front_face(const vk::FrontFace face) {
				rasterization_state.setFrontFace(face);
				return *this;
			}

			Builder& set_depth_bias(const float constant_factor, const float clamp, const float slope_factor) {
				rasterization_state.setDepthBiasEnable(true);
				rasterization_state.setDepthBiasConstantFactor(constant_factor);
				rasterization_state.setDepthBiasClamp(clamp);
				rasterization_state.setDepthBiasSlopeFactor(slope_factor);
				return *this;
			}

			Builder& set_line_width(const float width) {
				rasterization_state.setLineWidth(width);
				return *this;
			}

			Builder& enable_sample_shading(const bool enable) {
				multisample_state.setSampleShadingEnable(enable);
				return *this;
			}

			Builder& set_rasterization_samples(const vk::SampleCountFlagBits samples) {
				multisample_state.setRasterizationSamples(samples);
				return *this;
			}

			Builder& set_min_sample_shading(const float min) {
				multisample_state.setMinSampleShading(min);
				return *this;
			}

			Builder& set_sample_mask(const std::vector<u32>& mask) {
				multisample_state.setPSampleMask(mask.data());
				return *this;
			}

			Builder& enable_alpha_to_coverage(const bool enable) {
				multisample_state.setAlphaToCoverageEnable(enable);
				return *this;
			}

			Builder& enable_alpha_to_one(const bool enable) {
				multisample_state.setAlphaToOneEnable(enable);
				return *this;
			}

			Builder& enable_depth_test(const bool enable) {
				depth_stencil_state.setDepthTestEnable(enable);
				return *this;
			}

			Builder& enable_depth_write(const bool enable) {
				depth_stencil_state.setDepthWriteEnable(enable);
				return *this;
			}

			Builder& set_depth_compare_op(const vk::CompareOp op) {
				depth_stencil_state.setDepthCompareOp(op);
				return *this;
			}

			Builder& enable_depth_bounds_test(const bool enable) {
				depth_stencil_state.setDepthBoundsTestEnable(enable);
				return *this;
			}

			Builder& enable_stencil_test(const bool enable) {
				depth_stencil_state.setStencilTestEnable(enable);
				return *this;
			}

			Builder& set_front_stencil_state(const vk::StencilOpState& state) {
				depth_stencil_state.setFront(state);
				return *this;
			}

			Builder& set_back_stencil_state(const vk::StencilOpState& state) {
				depth_stencil_state.setBack(state);
				return *this;
			}

			Builder& set_min_depth_bounds(const float min) {
				depth_stencil_state.setMinDepthBounds(min);
				return *this;
			}

			Builder& set_max_depth_bounds(const float max) {
				depth_stencil_state.setMaxDepthBounds(max);
				return *this;
			}

			Builder& add_color_blend_attachment(const vk::PipelineColorBlendAttachmentState& attachment) {
				color_blend_attachments.push_back(attachment);
				return *this;
			}

			Builder& set_color_blend_logic_op(const vk::LogicOp op) {
				color_blend_state.setLogicOpEnable(true);
				color_blend_state.setLogicOp(op);
				return *this;
			}

			Builder& set_color_blend_constants(const std::array<f32, 4>& constants) {
				color_blend_state.setBlendConstants(constants);
				return *this;
			}

			Builder& add_dynamic_state(const vk::DynamicState state) {
				dynamic_states.push_back(state);
				return *this;
			}

			Builder& add_descriptor_set_layout(const vk::DescriptorSetLayout layout) {
				descriptor_set_layouts.push_back(layout);
				return *this;
			}

			Builder& add_push_constant_range(const vk::PushConstantRange range) {
				push_constant_ranges.push_back(range);
				return *this;
			}

			Builder& set_render_pass(const std::shared_ptr<vk::raii::RenderPass>& pass) {
				render_pass = pass;
				return *this;
			}

			Builder& use_default();

			Pipeline build() {
				return Pipeline(m_device, *this);
			}

			std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
			std::vector<vk::VertexInputBindingDescription> vertex_bindings;
			std::vector<vk::VertexInputAttributeDescription> vertex_attributes;
			vk::PipelineVertexInputStateCreateInfo vertex_input_state;
			vk::PipelineInputAssemblyStateCreateInfo input_assembly_state;
			vk::PipelineViewportStateCreateInfo viewport_state;
			vk::PipelineRasterizationStateCreateInfo rasterization_state;
			vk::PipelineMultisampleStateCreateInfo multisample_state;
			vk::PipelineDepthStencilStateCreateInfo depth_stencil_state;
			std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments;
			vk::PipelineColorBlendStateCreateInfo color_blend_state;
			std::vector<vk::DynamicState> dynamic_states;
			vk::PipelineDynamicStateCreateInfo dynamic_state;
			std::vector<vk::DescriptorSetLayout> descriptor_set_layouts;
			std::vector<vk::PushConstantRange> push_constant_ranges;
			std::shared_ptr<vk::raii::RenderPass> render_pass;

		  private:
			Device& m_device;
		};

		Pipeline(Device& device, Builder& builder);
		~Pipeline() = default;

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		vk::raii::PipelineLayout& layout() {
			return m_layout;
		}

		vk::raii::Pipeline& pipeline() {
			return m_pipeline;
		}

	  private:
		vk::raii::PipelineLayout m_layout = nullptr;
		vk::raii::Pipeline m_pipeline = nullptr;
	};
} // namespace Vulxels::GFX
