#include "input_controller.h";

// Move on keyboard press
void handleKeyboardInput() {
	float units = movementConfig.speed * timeState.deltaTime;

	// Move camera/vertex upon WASD input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		renderConfig.editMode ? translateVertexBy(worldForward * timeState.deltaTime) : cameraShift(worldForward * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		renderConfig.editMode ? translateVertexBy(worldForward * -timeState.deltaTime) : cameraShift(worldForward * -units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		renderConfig.editMode ? translateVertexBy(worldRight * timeState.deltaTime) : cameraShift(worldRight * -units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		renderConfig.editMode ? translateVertexBy(worldRight * -timeState.deltaTime) : cameraShift(worldRight * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
		renderConfig.editMode ? translateVertexBy(worldUp * timeState.deltaTime) : cameraShift(worldUp * units);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		renderConfig.editMode ? translateVertexBy(worldUp * -timeState.deltaTime) : cameraShift(worldUp * -units);
	}

	// Toggle various rendering configurations
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && !renderConfig.axisHasBeenToggled) {
		renderConfig.drawAxisEnabled = !renderConfig.drawAxisEnabled;
		renderConfig.axisHasBeenToggled = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) {
		renderConfig.axisHasBeenToggled = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C) && !renderConfig.wireframeHasBeenToggled) {
		renderConfig.drawWireframe = !renderConfig.drawWireframe;
		renderConfig.wireframeHasBeenToggled = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
		renderConfig.wireframeHasBeenToggled = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V) && !renderConfig.depthHasBeenToggled) {
		renderConfig.drawDepthMap = !renderConfig.drawDepthMap;
		renderConfig.depthHasBeenToggled = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V)) {
		renderConfig.depthHasBeenToggled = false;
	}

	// Toggle editing mode
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !renderConfig.editModeHasBeenToggled) {
		renderConfig.editMode = !renderConfig.editMode;
		renderConfig.editModeHasBeenToggled = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		renderConfig.editModeHasBeenToggled = false;
	}
}

// Handle mouse input
void handleMouseInput() {
	// Mouse click
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		if (!mouseState.mouseLeftDown) {
			mouseState.mouseLeftClicked = true;
		}
		else {
			mouseState.mouseLeftClicked = false;
		}

		mouseState.mouseLeftDown = true;
	}
	else {
		mouseState.mouseLeftClicked = false;
		mouseState.mouseLeftDown = false;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		if (!mouseState.mouseRightDown) {
			mouseState.mouseRightClicked = true;
		}
		else {
			mouseState.mouseRightClicked = false;
		}

		mouseState.mouseRightDown = true;
	}
	else {
		mouseState.mouseRightClicked = false;
		mouseState.mouseRightDown = false;
	}
}

// Handles mouse rotation
void handleMouseRotation() {
	// Mouse look
	int mx = sf::Mouse::getPosition(screenConfig.window).x;
	int my = sf::Mouse::getPosition(screenConfig.window).y;

	// Calculate offset from last frame
	int offsetX = mouseState.x - mx;
	int offsetY = my - mouseState.y;

	if (mouseState.mouseRightDown) {
		// Apply sensitivity
		cameraState.camAngle.y += offsetX * movementConfig.sensitivity; // yaw
		cameraState.camAngle.x += offsetY * movementConfig.sensitivity; // pitch

		// Clamp pitch (prevent flipping)
		if (cameraState.camAngle.x > 1.5f) cameraState.camAngle.x = 1.5f;
		if (cameraState.camAngle.x < -1.5f) cameraState.camAngle.x = -1.5f;
	}

	// Save new mouse position
	mouseState.x = mx;
	mouseState.y = my;
}

// Handles mesh selection
void handleMeshSelection() {
	if (mouseState.mouseLeftClicked) {
		pickMeshAt(mouseState.x, mouseState.y);
	}
}

// Handles vertex selection
void handleVertexSelection() {
	if (mouseState.mouseLeftClicked) {
		pickVertexAt(mouseState.x, mouseState.y);
	}
}

// Handle all existing inputs
void handleInputs() {
	handleKeyboardInput();
	handleMouseInput();
	handleMeshSelection();
	handleVertexSelection();
	handleMouseRotation();
	updateViewMatrix();
}