#pragma once

#include "pch.h"
#include "state.h"
#include "config.h"
#include "camera_controller.h"
#include "render_controller.h"
#include "scene.h"

void handleInputs();
void handleKeyboardInput();
void handleVertexSelection();
void handleMeshSelection();
void handleMouseInput();
void handleMouseRotation();
void updateViewMatrix();