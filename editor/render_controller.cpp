// This is the controller component of the software rasterizer.
//
// If an input is called (either by the user or the computer)
// the model get updated, followed by the view.
//
// NOTE:
// This is an attempt to follow the MVC architecture.

#include "render_controller.h"

float viewHeightRatio = 0.8;

void updateProjectionMatrix() {
	float W = screenConfig.width;
	float H = screenConfig.height;
	float front = projectionConfig.FAR_PLANE;
	float back = projectionConfig.NEAR_PLANE;

	renderState.projMatrix = projectionMatrix(projectionConfig.fov, W / H, front, back);
}

// TODO: Find a way to resize the software rasterizer window
//Done
void initializeWindow() {
	int width = screenConfig.width;
	int height = screenConfig.height;

	renderState.viewportMatrix = viewportMatrix(0, 0, width, height, 0, 1);
	screenConfig.window.create(sf::VideoMode(sf::Vector2u(width, height)), "Editor 3D");
}
void updateWindow() {
	int width = screenConfig.width;
	int height = screenConfig.height;

	renderState.viewportMatrix = viewportMatrix(0, 0, width, height, 0, 1);
	sf::FloatRect visibleArea(sf::Vector2f(0, 0), sf::Vector2f(width, height / viewHeightRatio));
	screenConfig.window.setView(sf::View(visibleArea));
}

// Updates view matrix based on current camera state
void updateViewMatrix() {
	Vec3& camPos = cameraState.camPosition;
	Vec3& camAngle = cameraState.camAngle;

	renderState.viewMatrix = viewMatrix(camPos, camAngle);
}

// Updates size of color buffer
void updateColorBufferSize() {
	if (renderState.colorBuffer != nullptr) {
		delete[] renderState.colorBuffer;
	}

	if (renderState.depthBuffer != nullptr) {
		delete[] renderState.depthBuffer;
	}

	const unsigned int WIDTH = screenConfig.width;
	const unsigned int HEIGHT = screenConfig.height;

	renderState.colorBuffer = new uint8_t[WIDTH * HEIGHT * 4];
	renderState.depthBuffer = new float[WIDTH * HEIGHT * 4];
	renderState.texture.resize({WIDTH, HEIGHT});
}

// Puts given pixel at (x, y) coordinates in the color buffer
void putPixel(int x, int y, Color color) {
	if (renderState.colorBuffer == nullptr) return;

	const unsigned int WIDTH = screenConfig.width;
	const unsigned int HEIGHT = screenConfig.height;

	if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		return;
	}

	int pixelIndex = (WIDTH * y + x) * 4;

	renderState.colorBuffer[pixelIndex + 0] = color.r;
	renderState.colorBuffer[pixelIndex + 1] = color.b;
	renderState.colorBuffer[pixelIndex + 2] = color.g;
	renderState.colorBuffer[pixelIndex + 3] = color.a;
}

// Clears current color buffer with the specified color
void clearColorBuffer(Color color) {
	if (renderState.colorBuffer == nullptr) return;

	const unsigned int WIDTH = screenConfig.width;
	const unsigned int HEIGHT = screenConfig.height;

	for (int index = 0; index < WIDTH * HEIGHT; index++) {
		int pixelIndex = index * 4;

		renderState.colorBuffer[pixelIndex + 0] = color.r;
		renderState.colorBuffer[pixelIndex + 1] = color.b;
		renderState.colorBuffer[pixelIndex + 2] = color.g;
		renderState.colorBuffer[pixelIndex + 3] = color.a;
	}
}

// Calculates MVP given the model matrix
Mat4 calculateMVP(const Mat4& modelMatrix) {
	return prodMatrix(renderState.projMatrix, prodMatrix(renderState.viewMatrix, modelMatrix));
}

void initializeRenderState() {
	updateProjectionMatrix();
	initializeWindow();
	updateViewMatrix();
	updateColorBufferSize();
}

void updateRenderState() {
	updateProjectionMatrix();
	updateWindow();
	updateViewMatrix();
	updateColorBufferSize();
}
