/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/shader.h>

#include <fstream>
#include <vector>

using namespace Vulxels::GFX;

void Shader::load_spirv(std::string_view path) {
	std::ifstream file(path.data(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open shader file");
	}

	auto file_size = file.tellg();
	std::vector<u32> code(file_size / sizeof(u32));
	file.seekg(0);
	file.read(reinterpret_cast<char*>(code.data()), file_size);
	file.close();

	printf("Loaded shader: %s (%zu bytes)\n", path.data(), code.size());

	m_module = vk::raii::ShaderModule(
		m_device.device(),
		vk::ShaderModuleCreateInfo().setCode(code)
	);
}
