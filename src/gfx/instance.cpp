/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vulxels/gfx/instance.h>
#include <vulxels/gfx/window.h>
#include <vulxels/log.h>
#include <vulxels/types.h>
#include <vulxels/version.h>

using namespace Vulxels::GFX;

Instance::Instance() {
	const u32 api_version = m_context.enumerateInstanceVersion();

	VX_LOG(
		"Vulkan API version: {}.{}.{}-{}",
		VK_API_VERSION_MAJOR(api_version),
		VK_API_VERSION_MINOR(api_version),
		VK_API_VERSION_PATCH(api_version),
		VK_API_VERSION_VARIANT(api_version)
	);

	if (api_version < VK_API_VERSION_1_2) {
		throw std::runtime_error("Vulkan API version too low");
	}

	m_appinfo.setPApplicationName("Vulxels");
	m_appinfo.setApplicationVersion(VK_MAKE_VERSION(VX_VERSION_MAJOR, VX_VERSION_MINOR, VX_VERSION_PATCH));
	m_appinfo.setPEngineName("Vulxels");
	m_appinfo.setEngineVersion(api_version);
	m_appinfo.setApiVersion(VK_API_VERSION_1_2);

	auto extensions = Window::get_required_extensions();

	// TODO: Check if extensions are supported
	// TODO: Check if validation layers are supported

	m_instance = vk::raii::Instance(
		m_context,
		vk::InstanceCreateInfo()
			.setPApplicationInfo(&m_appinfo)
			.setPEnabledExtensionNames(extensions)
			.setPEnabledLayerNames(VALIDATION_LAYERS)
	);
}
