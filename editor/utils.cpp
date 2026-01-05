#include "utils.h"

// Generate a random RGBA color
Color randomColor() {	
	static bool seeded = false;
	if (!seeded) {
		srand(time(nullptr));
		seeded = true;
	}

	Color color;
	color.r = rand() % 256;
	color.b = rand() % 256;
	color.g = rand() % 256;

	return color;
}