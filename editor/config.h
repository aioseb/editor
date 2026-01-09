#pragma once
#include "SFML/Graphics.hpp";

// Screen / Viewport configurations
struct ScreenConfig {
	unsigned int width = 1920;
	unsigned int height = 1080;

	// Top-Left coordinates of the rendering area
	const float X = 0.0f;	
	const float Y = 0.0f;

	sf::RenderWindow window;
};
extern ScreenConfig screenConfig;

// Projection configurations
struct ProjectionConfig {
	float fov = 45.0f;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 100.0f;
};
extern ProjectionConfig projectionConfig;

// Movement configurations
struct MovementConfig {
	float speed = 1.0f;
	float sensitivity = 0.01f;
};
extern MovementConfig movementConfig;

// Render configurations
struct RenderConfig {
	bool drawAxisEnabled = true;
	bool hasBeenToggled = false;
};
extern RenderConfig renderConfig;