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

	try {
		Vulxels::App app;
		app.run();
	} catch (const std::exception& e) {
		fprintf(stderr, "\u001b[31mUnhandled exception: %s\u001b[0m\n", e.what());
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Unhandled exception",
			e.what(),
			nullptr
		);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
