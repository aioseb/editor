#include "input_controller.h";

// Move on keyboard press
void handleKeyboardInput() {
	float units = movementConfig.speed * timeState.deltaTime;
	const float PI = 3.141592f;

	// Handle editTransform mode switching with arrow keys.
	// renderState.editTransform:
	//  0 - translate; 1 - rotate; 2 - scale; 3 - vertex movement
	static bool leftWasDown = false;
	static bool rightWasDown = false;
	static bool numDown = false;

	// Left arrow => previous mode
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		if (!leftWasDown) {
			renderState.editTransform = renderState.editTransform - 1;
			if (renderState.editTransform < 0) {
				renderState.editTransform = 3;
			}

			if (renderState.editTransform == 0) {
				std::cout << "Current mode: TRANSLATION\n";
			}
			else if (renderState.editTransform == 1) {
				std::cout << "Current mode: ROTATION\n";
			}
			else if (renderState.editTransform == 2) {
				std::cout << "Current mode: SCALING\n";
			}
			else if (renderState.editTransform == 3) {
				std::cout << "Current mode: VERTEX MOVEMENT\n";
			}
		}
		leftWasDown = true;
	}
	else {
		leftWasDown = false;
	}

	// Right arrow => next mode
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		if (!rightWasDown) {
			renderState.editTransform = (renderState.editTransform + 1) % 4;

			if (renderState.editTransform == 0) {
				std::cout << "Current mode: TRANSLATION\n";
			}
			else if (renderState.editTransform == 1) {
				std::cout << "Current mode: ROTATION\n";
			}
			else if (renderState.editTransform == 2) {
				std::cout << "Current mode: SCALING\n";
			}
			else if (renderState.editTransform == 3) {
				std::cout << "Current mode: VERTEX MOVEMENT\n";
			}
		}
		rightWasDown = true;
	}
	else {
		rightWasDown = false;
	}

	if (!renderConfig.editMode) {
		// Move camera/vertex upon WASD input
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
	}
	else {
		// Translation mode
		if (renderState.editTransform == 0) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
				translateSelectedBy(worldForward * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				translateSelectedBy(worldForward * -units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				translateSelectedBy(worldRight * -units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				translateSelectedBy(worldRight * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
				translateSelectedBy(worldUp * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
				translateSelectedBy(worldUp * -units);
			}
		}
		// Rotation mode
		else if (renderState.editTransform == 1) {
			// Small angle per second scaled by deltaTime
			Vec3 rotDelta = zeroVector;
			const float rotSpeed = timeState.deltaTime * PI * 0.2;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) rotDelta.x -= rotSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) rotDelta.x += rotSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) rotDelta.y += rotSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) rotDelta.y -= rotSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) rotDelta.z += rotSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) rotDelta.z -= rotSpeed;

			if (rotDelta.x != 0.0f || rotDelta.y != 0.0f || rotDelta.z != 0.0f) {
				rotateSelectedBy(rotDelta);
			}
		}
		// Scaling mode
		else if (renderState.editTransform == 2) {
			Vec3 scaleDelta = zeroVector;
			const float scaleSpeed = timeState.deltaTime; // relative scale per second

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) scaleDelta.z += scaleSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) scaleDelta.z -= scaleSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) scaleDelta.x += scaleSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) scaleDelta.x -= scaleSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) scaleDelta.y += scaleSpeed;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) scaleDelta.y -= scaleSpeed;

			if (scaleDelta.x != 0.0f || scaleDelta.y != 0.0f || scaleDelta.z != 0.0f) {
				scaleSelectedBy(scaleDelta);
			}
		}
		// Vertex movement mode
		else if (renderState.editTransform == 3) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
				translateVertexBy(worldForward * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				translateVertexBy(worldForward * -units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				translateVertexBy(worldRight * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				translateVertexBy(worldRight * -units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
				translateVertexBy(worldUp * units);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
				translateVertexBy(worldUp * -units);
			}
		}
	}

	// Create objects upon number press
	// 1 - cube
	// 2 - sphere
	// 3 - pyramid
	// 4 - cylinder
	// 5 - cone
	bool anyNumDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) && !numDown) {
		createMeshInScene("cube.obj");
		numDown = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) && !numDown) {
		createMeshInScene("sphere.obj");
		numDown = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) && !numDown) {
		createMeshInScene("pyramid.obj");
		numDown = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) && !numDown) {
		createMeshInScene("cylinder.obj");
		numDown = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5) && !numDown) {
		createMeshInScene("cone.obj");
		numDown = true;
	}
	if (!anyNumDown) {
		numDown = false;
	}

	// Change color of the selected mesh upon pressing a number from 6 to 0 (on keyboard)
	// 6 - white
	// 7 - red
	// 8 - blue
	// 9 - green
	// 0 - yellow
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6)) {
		changeColorSelected(WHITE);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7)) {
		changeColorSelected(RED);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8)) {
		changeColorSelected(BLUE);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9)) {
		changeColorSelected(GREEN);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0)) {
		changeColorSelected({255, 255, 0});
	}

	// Delete currently selected mesh
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
		deleteSelected();
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

	// Close window on ESC
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
		screenConfig.window.close();
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