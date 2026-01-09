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

// Return linear interpolation of screen space depth
float linearizeDepth(float depth) {
	float z = depth * 2.0f - 1.0f;
	float near = projectionConfig.NEAR_PLANE;
	float far = projectionConfig.FAR_PLANE;
	return (2.0f * near * far) / (far + near - z * (far - near));
}