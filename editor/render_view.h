#pragma once

#include "pch.h"
#include "render_controller.h"
#include "state.h"
#include "config.h"
#include "mesh.h"

void drawAxis();
void drawFace(Vec4 a, Vec4 b, Vec4 c, Vec3 normal, Color color = BLUE);
void drawLine(int ax, int ay, int bx, int by, Color color = WHITE);
void drawLineWorldSpace(Vec4 a, Vec4 b, Color color = WHITE);
void drawTriangle(const Vec4& a, const Vec4& b, const Vec4& c, Color color = WHITE);
void drawTriangleDepth(const Vec4& v0, const Vec4& v1, const Vec4& v2, Color color = WHITE);
void drawMesh(const Mesh& mesh, bool drawEdges = false, Color edgesColor = WHITE);
void render();
bool isInsideDrawingSpace(Vec4 a, Vec4 b, Vec4 c);
bool isInsideTriangle(Vec4 tri0, Vec4 tri1, Vec4 tri2, Vec4 a);