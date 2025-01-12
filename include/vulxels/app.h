/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <vulxels/gfx/renderer.h>
#include <vulxels/gfx/window.h>

namespace Vulxels {
	class App {
	  public:
		App();
		~App();
		void run();

	  private:
		bool m_running = true;
		GFX::Window m_window {"Vulxels", 1600, 900};
		GFX::Renderer m_renderer {m_window};
	};
} // namespace Vulxels
