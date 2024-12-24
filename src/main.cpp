/**
 * Copyright (c) 2024, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <SDL2/SDL.h>
#include <getopt.h>
#include <vulxels/app.h>
#include <vulxels/version.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>

int main(int argc, char** argv) {
	int opt;
	while ((opt = getopt(argc, argv, "vh")) != -1) {
		switch (opt) {
			case 'v':
				printf(
					"Vulxels version %d.%d.%d\n",
					VULXELS_VERSION_MAJOR,
					VULXELS_VERSION_MINOR,
					VULXELS_VERSION_PATCH
				);
				return EXIT_SUCCESS;
			case 'h':
			default:
				printf("Usage: %s [-v] [-h]\n", argv[0]);
				printf("  -v: Print the version of the application\n");
				printf("  -h: Print this help message\n");
				return EXIT_SUCCESS;
		}
	}

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
