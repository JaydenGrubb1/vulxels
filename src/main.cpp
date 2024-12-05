#include <cstdio>
#include <cstdlib>

int main(int argc, char **argv) {
	std::printf("Hello, World!\n");

	for (int i = 0; i < argc; i++) {
		std::printf("argv[%d] = %s\n", i, argv[i]);
	}

	return EXIT_SUCCESS;
}