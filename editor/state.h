#pragma once

#include "pch.h"

// Stores the state of graphics pipeline matrices and flags
struct RenderState {
	Mat4 viewMatrix;
	Mat4 projMatrix;
	Mat4 viewportMatrix;

	uint8_t* colorBuffer;
	float* depthBuffer;
	sf::Texture texture;

	int editTransform = 0;	// 0 - translate; 1 - rotate; 2 - scale; 3 - vertex movement
};
extern RenderState renderState;

// Stores the camera state of the world camera
struct CameraState {
	Vec3 camPosition = worldForward * 3.0f;
	Vec3 camAngle = zeroVector;
};
extern CameraState cameraState;

// Stores mouse state
struct MouseState {
	int x;
	int y;
	bool mouseLeftClicked;
	bool mouseLeftDown;
	bool mouseRightClicked;
	bool mouseRightDown;
};
extern MouseState mouseState;

// Stores time related values
struct TimeState {
	float deltaTime = 0.0f;
};
extern TimeState timeState;