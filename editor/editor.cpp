#include "pch.h"
#include "input_controller.h"
#include "render_controller.h"
#include "camera_controller.h"
#include "render_view.h"
#include "state.h"
#include "config.h"
#include "scene.h"
#include "utils.h"

int main() {
	printInstructions();

	// Initialize render state, window and clock
	initializeRenderState();
	sf::Clock clock;

	Color BG_COLOR = { 32, 32, 32 };

	// Create initial cube
	const char* filePath = "\cube.obj";
	createMeshInScene(filePath);

	while (screenConfig.window.isOpen()) {
		timeState.deltaTime = clock.restart().asSeconds();

		while (const std::optional event = screenConfig.window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				screenConfig.window.close();
			}
		}

		clearColorBuffer(BG_COLOR);

		handleInputs();
		drawScene();

		// Draw world axis in the center of the screen
		if (renderConfig.drawAxisEnabled) {
			drawAxis();
		}	

		render();
	}
}