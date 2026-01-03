#include "input_controller.h";

// Move on keyboard press
void handleKeyboardInput() {
	float units = movementConfig.speed * timeState.deltaTime;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		cameraShift(worldForward * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		cameraShift(worldForward * -units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		cameraShift(worldRight * -units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		cameraShift(worldRight * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
		cameraShift(worldUp * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		cameraShift(worldUp * -units);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && !renderConfig.hasBeenToggled) {
		renderConfig.drawAxisEnabled = !renderConfig.drawAxisEnabled;
		renderConfig.hasBeenToggled = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) {
		renderConfig.hasBeenToggled = false;
	}
}

// Rotate on mouse move
void handleMouseInput() {
	// Mouse look
	int mx = sf::Mouse::getPosition().x;
	int my = sf::Mouse::getPosition().y;

	// Calculate offset from last frame
	int offsetX = mouseState.x - mx;
	int offsetY = my - mouseState.y;

	// Apply sensitivity
	cameraState.camAngle.y += offsetX * movementConfig.sensitivity * timeState.deltaTime; // yaw
	cameraState.camAngle.x += offsetY * movementConfig.sensitivity * timeState.deltaTime; // pitch

	// Clamp pitch (prevent flipping)
	if (cameraState.camAngle.x > 1.5f) cameraState.camAngle.x = 1.5f;
	if (cameraState.camAngle.x < -1.5f) cameraState.camAngle.x = -1.5f;

	// Save new mouse position
	mouseState.x = mx;
	mouseState.y = my;
}

// Handle all existing inputs
void handleInputs() {
	handleKeyboardInput();
	handleMouseInput();
	updateViewMatrix();
}