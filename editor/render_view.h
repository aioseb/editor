#pragma once

#include "pch.h"
#include "render_controller.h"
#include "state.h"
#include "config.h"
#include "mesh.h"

void drawAxis();
void drawLine(int ax, int ay, int bx, int by, sf::Color color = sf::Color::White);
void drawFace(Vec4 a, Vec4 b, Vec4 c);
void drawTriangle(Vec4 a, Vec4 b, Vec4 c);
void drawMesh(const Mesh& mesh);
bool isInsideDrawingSpace(Vec4 a, Vec4 b, Vec4 c);