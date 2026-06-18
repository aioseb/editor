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

// Print a list of instructions in the terminal
void printInstructions() {
	std::cout << "This is a 3D editor / visualizer\n\n";
	std::cout << "Use WASD keys to move around\n";
	std::cout << "Look around with the camera with by holding RIGHT CLICK and moving the MOUSE\n";
	std::cout << "CLICK on an object to SELECT it\n";
	std::cout << "Press a number from 1 to 5 on your keyboard to SPAWN a PRIMITIVE MESH in front of you\n";
	std::cout << "Press Z to delete the currently selected mesh\n";
	std::cout << "1 - CUBE | 2 - SPHERE | 3 - PYRAMID | 4 - CYLINDER | 5 - CONE\n\n";
	std::cout << "Press a number from 6 to 0 on your keyboard to CHANGE COLOR of SELECTED MESH\n";
	std::cout << "6 - WHITE | 7 - RED | 8 - BLUE | 9 - GREEN | 0 - YELLOW\n\n";
	std::cout << "Press SPACE to toggle EDIT MODE\n";
	std::cout << "While in EDIT MODE, your camera cannot move but you can edit with your WASD keys\n";
	std::cout << "Also, the window will be surrounded by a green border.\n";
	std::cout << "Press your ARROW KEYS to go through various modes while in EDIT MODE\n\n";
	std::cout << "LEFT ARROW ( <- ) - previous mode | RIGHT ARROW ( -> ) - next mode\n\n";
	std::cout << "FIRST MODE: TRANSLATION - Move selected object in space along world axes (X, Y, Z)\n";
	std::cout << "SECOND MODE: ROTATION - Rotate selected object along world axes (X, Y, Z)\n";
	std::cout << "THIRD MODE: SCALING - Scale selected object along world axes (X, Y, Z)\n";
	std::cout << "FOURTH MODE: VERTEX MOVEMENT - Move selected vertex of a mesh relative to your camera direction\n\n";
	std::cout << "Other togglabes:\n";
	std::cout << "Press X to toggle axes drawn on the center of the screen\n";
	std::cout << "Press C to toggle WIREFRAME VIEW (edges of triangles are drawn)\n";
	std::cout << "Press V to toggle DEPTH MAP (the whiter the pixels, the closer they are to the screen)\n\n";
}