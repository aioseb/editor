#include "pch.h"
#include "input_controller.h"
#include "render_controller.h"
#include "camera_controller.h"
#include "render_view.h"
#include "state.h"
#include "config.h"

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

	// Create cube mesh
	Mesh cubeOne = createMesh(verticesCube, 8, facesCube, 12);
	setColor(cubeOne, BLUE);

	// Scale second cube along the X and Y axis
	Mesh cubeTwo = createMesh(verticesCube, 8, facesCube, 12, worldRight * RADIUS, zeroVector, (worldRight + worldUp) * SCALE + worldForward);
	setColor(cubeTwo, RED);
	setTranslation(cubeTwo, worldForward * -4.0f);

	// Create third cube
	Mesh cubeThree = createMesh(verticesCube, 8, facesCube, 12);
	setScaling(cubeThree, oneVector * 3.0f);
	setTranslation(cubeThree, worldForward * -5.0f);
	setRotation(cubeThree, { -PI / 6, PI / 3, 0.0f });
	setColor(cubeThree, GREEN);

	// Creade pyramid mesh
	Mesh pyramid = createMesh(verticesPyramid, 5, facesPyramid, 6);
	setTranslation(pyramid, worldUp * 1.0f);

	// Create triangle mesh
	Mesh triangle = createMesh(verticesTriangle, 3, facesTriangle, 1);
	setTranslation(triangle, worldRight * 4.0f);

	sf::Clock clock;
	
	float x = 0;

	while (screenConfig.window.isOpen()) {
		timeState.deltaTime = clock.restart().asSeconds();

		while (const std::optional event = screenConfig.window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				screenConfig.window.close();
			}
		}

		clearColorBuffer(BLACK);

		// Draw triangle
		drawMesh(triangle);

		// Draw frist cube in 3D space and rotate it
		drawMesh(cubeOne);
		Vec3 rotationOne = { PI / 3, PI / 6, PI / 4 };
		rotateBy(cubeOne, rotationOne * DELTA * timeState.deltaTime);
		// rotateBy(cubeOne, worldUp * DELTA * timeState.deltaTime);

		// Draw second cube in 3D space and move it in circular motion
		drawMesh(cubeTwo);

		// Draw third cube
		drawMesh(cubeThree);

		float s = RADIUS * sin(angle);
		float c = RADIUS * cos(angle);
		// setTranslation(cubeTwo, worldRight * c + worldUp * s);
		// rotateBy(cubeTwo, worldForward * DELTA * timeState.deltaTime);

		// Modify one of the pyarmid's vertices (the tip)
		// NOTE: Modifiyng vertices won't be this 'hacky'
		// TODO: Find a better way to implement vertex modification / update

		// pyramid.vertices[0].y = RADIUS * sin(angle + PI * 0.5f) * 0.5f;
		// pyramid.vertices[0].x = RADIUS * cos(angle + PI * 0.5f) * 1.5f;
		// drawMesh(pyramid);

		// Draw world axis in the center of the screen
		if (renderConfig.drawAxisEnabled) {
			drawAxis();
		}

		// Act upon mouse or keyboard press
		handleInputs();

		angle += DELTA * timeState.deltaTime;
		if (angle > PI * 2) angle = 0.0f;

		render();
	}
}