// This is the controller component of the software rasterizer.
//
// If an input is called (either by the user or the computer)
// the model get updated, followed by the view.
//
// NOTE:
// We are not referring to the matrices in the MVP!!
// This is an attempt to follow the MVC architecture.

#include "render_controller.h"

void updateProjectionMatrix() {
	float W = screenConfig.width;
	float H = screenConfig.height;
	float front = projectionConfig.FAR_PLANE;
	float back = projectionConfig.NEAR_PLANE;

	renderState.projMatrix = projectionMatrix(projectionConfig.fov, W / H, front, back);
}

// TODO: Find a way to resize the software rasterizer window
void updateWindow() {
	int width = screenConfig.width;
	int height = screenConfig.height;

	renderState.viewportMatrix = viewportMatrix(0, 0, width, height, 0, 1);
	screenConfig.window.create(sf::VideoMode(sf::Vector2u(width, height)), "Test");
}

// Updates view matrix based on current camera state
void updateViewMatrix() {
	Vec3& camPos = cameraState.camPosition;
	Vec3& camAngle = cameraState.camAngle;

	renderState.viewMatrix = viewMatrix(camPos, camAngle);
}

// Calculates MVP given the model matrix
Mat4 calculateMVP(const Mat4& modelMatrix) {
	return prodMatrix(renderState.projMatrix, prodMatrix(renderState.viewMatrix, modelMatrix));
}

void initializeRenderState() {
	updateProjectionMatrix();
	updateWindow();
	updateViewMatrix();
}
