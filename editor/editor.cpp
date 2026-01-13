#include "pch.h"
#include "input_controller.h"
#include "render_controller.h"
#include "camera_controller.h"
#include "render_view.h"
#include "state.h"
#include "config.h"
#include "scene.h"

int main() {
	// Initialize render state and window
	initializeRenderState();

	// Define the mesh for a cube
	Vec4 verticesCube[8] = {
		{ 0.5,  0.5,  0.5},	// 0 //	  3 ------ 0
		{ 0.5,  0.5, -0.5},	// 1 //	 /┆       /|
		{-0.5,  0.5, -0.5},	// 2 //	2 ------ 1 |
		{-0.5,  0.5,  0.5},	// 3 // | ┆		 | |
		{ 0.5, -0.5,  0.5},	// 4 // | 7 ------ 4
		{ 0.5, -0.5, -0.5},	// 5 // |/       |/
		{-0.5, -0.5, -0.5},	// 6 // 6 ------ 5 
		{-0.5, -0.5,  0.5},	// 7 //
	};

	Face facesCube[12] = {
		{{0, 1, 2}, worldUp},
		{{0, 3, 2}, worldUp},
		{{4, 5, 6}, worldUp * -1.0f},
		{{4, 7, 6}, worldUp * -1.0f},
		{{0, 1, 5}, worldRight},
		{{0, 4, 5}, worldRight},
		{{3, 2, 6}, worldRight * -1.0f},
		{{3, 7, 6}, worldRight * -1.0f},
		{{3, 0, 4}, worldForward},
		{{3, 7, 4}, worldForward},
		{{2, 1, 5}, worldForward * -1.0f},
		{{2, 6, 5}, worldForward * -1.0f},
	};

	// Define the mesh for a pyramid
	Vec4 verticesPyramid[5] = {
		{ 0.0,  0.5,  0.0},	// 0 //			0 \			 /
		{ 0.5, -0.5,  0.5}, // 1 //		   / \ 	\		/
		{ 0.5, -0.5, -0.5}, // 2 //       /4 -\-- 1    /
		{-0.5, -0.5, -0.5}, // 3 //      //    \ /    /
		{-0.5, -0.5,  0.5}  // 4 //     3 ----- 2    /
	};

	Face facesPyramid[6] = {
		{1, 2, 3},
		{1, 4, 3},
		{0, 2, 1},
		{0, 2, 3},
		{0, 3, 4},
		{0, 4, 1}
	};

	// Define the mesh for a triangle
	Vec4 verticesTriangle[3] = {
		{  0.0f, 0.5f, 0.0f},
		{  0.0f, 0.5f, 0.5f},
		{ -0.5f, 0.5f, 0.0f}
	};

	Face facesTriangle[1] = {
		{0, 1, 2, worldUp}
	};

	const float RADIUS = 3.0f;
	const float SCALE = 2.0f;
	const float PI = 3.141592f;
	const float DELTA = 0.5f * PI;

	float angle = 0.0f;

	Scene scene;

	// Create cube mesh
	Mesh cubeOne = createMesh(verticesCube, 8, facesCube, 12);
	setColor(cubeOne, BLUE);
	int cubeOneIndex = addMesh(cubeOne);

	// Scale second cube along the X and Y axis
	Mesh cubeTwo = createMesh(verticesCube, 8, facesCube, 12, worldRight * RADIUS, zeroVector, (worldRight + worldUp) * SCALE + worldForward);
	setColor(cubeTwo, RED);
	setTranslation(cubeTwo, worldForward * -4.0f);
	int cubeTwoIndex = addMesh(cubeTwo);

	// Create third cube
	Mesh cubeThree = createMesh(verticesCube, 8, facesCube, 12);
	setScaling(cubeThree, oneVector * 3.0f);
	setTranslation(cubeThree, worldForward * -5.0f);
	setRotation(cubeThree, { -PI / 6, PI / 3, 0.0f });
	setColor(cubeThree, GREEN);
	int cubeThreeIndex = addMesh(cubeThree);

	// Creade pyramid mesh
	Mesh pyramid = createMesh(verticesPyramid, 5, facesPyramid, 6);
	setTranslation(pyramid, worldUp * 1.0f);

	// Create triangle mesh
	Mesh triangle = createMesh(verticesTriangle, 3, facesTriangle, 1);
	setTranslation(triangle, worldRight * 4.0f);

	sf::Clock clock;

	while (screenConfig.window.isOpen()) {
		timeState.deltaTime = clock.restart().asSeconds();

		while (const std::optional event = screenConfig.window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				screenConfig.window.close();
			}
		}

		handleInputs();
		clearColorBuffer({64, 64, 64});

		// Draw triangle
		// drawMesh(triangle);

		// Draw frist cube in 3D space and rotate it
		// drawMesh(cubeOne);
		Vec3 rotationOne = { PI / 3, PI / 6, PI / 4 };
		rotateBy(*getMesh(cubeOneIndex), rotationOne* DELTA* timeState.deltaTime);
		// rotateBy(cubeOne, worldUp * DELTA * timeState.deltaTime);

		// translateSelectedBy({ 0.1f, 0.0f, 0.0f });

		// Draw second cube in 3D space and move it in circular motion
		// drawMesh(cubeTwo);

		drawScene();
		// Draw world axis in the center of the screen
		if (renderConfig.drawAxisEnabled) {
			drawAxis();
		}

		drawScene();
		render();
	}
}