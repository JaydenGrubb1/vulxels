/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vulxels/app.h>
#include <vulxels/version.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>

int main(int, char** argv) {
	// TODO: Parse command line arguments
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

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
