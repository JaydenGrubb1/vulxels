/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef _WIN32
	#include <windows.h>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vulxels/app.h>
#include <vulxels/log.h>
#include <vulxels/version.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>

int main(int, char** argv) {
	// TODO: Parse command line arguments
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

#ifdef _WIN32
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		std::freopen("CONOUT$", "w", stdout);
		std::freopen("CONOUT$", "w", stderr);
	}
#endif

	VX_LOG("Vulxels v{}.{}.{}", VX_VERSION_MAJOR, VX_VERSION_MINOR, VX_VERSION_PATCH);
#ifndef NDEBUG
	VX_WARN("Running in debug mode");
	spdlog::set_level(spdlog::level::debug);
#endif

	try {
		Vulxels::App app;
		app.run();
	} catch (const std::exception& e) {
		VX_CRIT("Unhandled exception: {}", e.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unhandled exception", e.what(), nullptr);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
