#pragma once

#include "pch.h"
#include "config.h"
#include "state.h"
#include "mesh.h"

void updateProjectionMatrix();
void updateWindow();
void updateViewMatrix();
void updateColorBufferSize();
void putPixel(int x, int y, Color color);
void clearColorBuffer(Color color);
Mat4 calculateMVP(const Mat4& modelMatrix);
void initializeRenderState();