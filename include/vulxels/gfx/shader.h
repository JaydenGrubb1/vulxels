/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/device.h>

#include <string_view>
#include <vulkan/vulkan_raii.hpp>

namespace Vulxels::GFX {
	class Shader {
	  public:
		explicit Shader(Device& device) : m_device(device) {}
		Shader(Device& device, const std::string_view path) : m_device(device) {
			load_spirv(path);
		}
		~Shader() = default;

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		void load_spirv(std::string_view path);
		// void load_glsl(std::string_view path); // TODO

		vk::raii::ShaderModule& module() {
			return m_module;
		}

	  private:
		Device& m_device;
		vk::raii::ShaderModule m_module = nullptr;
	};
} // namespace Vulxels::GFX
