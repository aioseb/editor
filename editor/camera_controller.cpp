#include "camera_controller.h"

// Shift camera by given camShift
void cameraShift(const Vec3& camShift) {
	float deltaLeft = -camShift.x;
	float deltaUp = camShift.y;
	float deltaForward = -camShift.z;

	Mat4& view = renderState.viewMatrix;

	Vec3 cameraLeft = { -view.m[0][0], -view.m[0][1], -view.m[0][2] };
	Vec3 cameraUp = { view.m[1][0], view.m[1][1], view.m[1][2] };
	Vec3 cameraForward = { -view.m[2][0], -view.m[2][1], -view.m[2][2] };

	Vec3 deltaMovement = cameraLeft * -deltaLeft;
	deltaMovement = deltaMovement + cameraUp * deltaUp;
	deltaMovement = deltaMovement + cameraForward * -deltaForward;

	cameraState.camPosition = cameraState.camPosition + deltaMovement;
}

// Rotate camera by given values
void cameraRotate(float x, float y) {
	// Apply sensitivity
	cameraState.camAngle.y += x; // yaw
	cameraState.camAngle.x += y; // pitch

	// Clamp pitch (prevent flipping)
	if (cameraState.camAngle.x > 1.5f) cameraState.camAngle.x = 1.5f;
	if (cameraState.camAngle.x < -1.5f) cameraState.camAngle.x = -1.5f;
}