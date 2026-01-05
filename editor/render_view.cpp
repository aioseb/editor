#include "render_view.h"

// Draws the x, y and z axis on the center of the screen
void drawAxis() {
	Mat4& view = renderState.viewMatrix;
	const int halfWidth = screenConfig.width / 2;
	const int halfHeight = screenConfig.height / 2;

	Vec4 cameraRight = { view.m[0][0], view.m[1][0], view.m[2][0] };
	Vec4 cameraUp = { view.m[0][1], view.m[1][1], view.m[2][1] };
	Vec4 cameraForward = { -view.m[0][2], -view.m[1][2], -view.m[2][2] };

	const float scale = 40.0f;
	cameraRight = cameraRight * scale;
	cameraUp = cameraUp * -scale;
	cameraForward = cameraForward * -scale;

	drawLine(halfWidth + cameraRight.x, halfHeight + cameraRight.y, halfWidth, halfHeight, RED);			// X axis
	drawLine(halfWidth + cameraForward.x, halfHeight + cameraForward.y, halfWidth, halfHeight, BLUE);		// Z axis
	drawLine(halfWidth + cameraUp.x, halfHeight + cameraUp.y, halfWidth, halfHeight, GREEN);				// Y axis
}

// Draw a line on screen, given both coordinates and an optional color
// Bresenham's line algorithm
void drawLine(int ax, int ay, int bx, int by, Color color) {
	int dx = std::abs(ax - bx);
	int dy = std::abs(ay - by);

	int sx = (ax < bx) ? 1 : -1;
	int sy = (ay < by) ? 1 : -1;

	int err = dx - dy;

	while (true) {
		putPixel(ax, ay, color);

		if (ax == bx && ay == by) {
			break;
		}

		int e2 = err << 1;

		if (e2 > -dy) {
			err -= dy;
			ax += sx;
		}

		if (e2 < dx) {
			err += dx;
			ay += sy;
		}
	}
}

// Draw a triangle on the screen, given local coordinates and the MVP
void drawTriangle(const Vec4& a, const Vec4& b, const Vec4& c) {
	drawLine(a.x, a.y, b.x, b.y);
	drawLine(b.x, b.y, c.x, c.y);
	drawLine(c.x, c.y, a.x, a.y);
}

int min(int a, int b) {
	if (a < b) return a;
	return b;
}

int max(int a, int b) {
	if (a > b) return a;
	return b;
}

// Draw the top part of a triangle
void drawTopTriangle(Vec4 v0, Vec4 v1, Vec4 v2, Color color) {
	// v0 becomes the top-most, v2 bottom-most and v1 in-between vertically wise
	if (v1.y < v0.y) std::swap(v1, v0);
	if (v2.y < v0.y) std::swap(v2, v0);
	if (v2.y < v1.y) std::swap(v1, v2);

	int x0 = v0.x, y0 = v0.y;
	int x1 = v1.x, y1 = v1.y;
	int x2 = v2.x, y2 = v2.y;

	// Draw the top part of the triangle
	// Pixel steps are calculated using Bresenham's line algorithm
	int x0_t = x0, y0_t = y0;

	int dx01 = std::abs(x0 - x1);
	int dy01 = std::abs(y0 - y1);

	int sx01 = (x0 < x1) ? 1 : -1;
	int sy01 = (y0 < y1) ? 1 : -1;

	int err01 = dx01 - dy01;

	int dx02 = std::abs(x0 - x2);
	int dy02 = std::abs(y0 - y2);

	int sx02 = (x0 < x2) ? 1 : -1;
	int sy02 = (y0 < y2) ? 1 : -1;

	int err02 = dx02 - dy02;

	while (y0 < y1 && y0_t < y1) {
		drawLine(x0, y0, x0_t, y0_t, color);	// Draw a scan line

		// Make sure both pixel steps are on the same height
		// to avoid missing pixels in a face
		while (y0 <= y0_t) {
			int e01 = err01 << 1;

			if (e01 > -dy01) {
				err01 -= dy01;
				x0 += sx01;
			}

			if (e01 < dx01) {
				err01 += dx01;
				y0 += sy01;
			}
		}

		while (y0_t < y0) {
			int e02 = err02 << 1;

			if (e02 > -dy02) {
				err02 -= dy02;
				x0_t += sx02;
			}

			if (e02 < dx02) {
				err02 += dx02;
				y0_t += sy02;
			}
		}
	}
}

// Draw the bottom part of a triangle
void drawBottomTriangle(Vec4 v0, Vec4 v1, Vec4 v2, Color color) {
	// v0 becomes the bottom-most, v2 top-most and v1 in-between vertically wise
	if (v1.y > v0.y) std::swap(v1, v0);
	if (v2.y > v0.y) std::swap(v2, v0);
	if (v2.y > v1.y) std::swap(v1, v2);

	int x0 = v0.x, y0 = v0.y;
	int x1 = v1.x, y1 = v1.y;
	int x2 = v2.x, y2 = v2.y;

	// Draw the top part of the triangle
	// Pixel steps are calculated using Bresenham's line algorithm
	int x0_t = x0, y0_t = y0;

	int dx01 = std::abs(x0 - x1);
	int dy01 = std::abs(y0 - y1);

	int sx01 = (x0 < x1) ? 1 : -1;
	int sy01 = (y0 < y1) ? 1 : -1;

	int err01 = dx01 - dy01;

	int dx02 = std::abs(x0 - x2);
	int dy02 = std::abs(y0 - y2);

	int sx02 = (x0 < x2) ? 1 : -1;
	int sy02 = (y0 < y2) ? 1 : -1;

	int err02 = dx02 - dy02;

	while (y0 > y1 && y0_t > y1) {
		drawLine(x0, y0, x0_t, y0_t, color);	// Draw a scan line

		// Make sure both pixel steps are on the same height
		// to avoid missing pixels in a face
		while (y0 >= y0_t) {
			int e01 = err01 << 1;

			if (e01 > -dy01) {
				err01 -= dy01;
				x0 += sx01;
			}

			if (e01 < dx01) {
				err01 += dx01;
				y0 += sy01;
			}
		}

		while (y0_t > y0) {
			int e02 = err02 << 1;

			if (e02 > -dy02) {
				err02 -= dy02;
				x0_t += sx02;
			}

			if (e02 < dx02) {
				err02 += dx02;
				y0_t += sy02;
			}
		}
	}
	drawLine(x0, y0, x0_t, y0_t, color);
}

// Draw a filled triangle on 2D screen using a scanline algorithm
void drawFace(Vec4 v0, Vec4 v1, Vec4 v2, Color color)
{
	drawTopTriangle(v0, v1, v2, color);
	drawBottomTriangle(v0, v1, v2, color);

	// Legacy code
	//// Bounding box
	//int minX = min(v0.x, min(v1.x, v2.x));
	//int maxX = max(v0.x, max(v1.x, v2.x));
	//int minY = min(v0.y, min(v1.y, v2.y));
	//int maxY = max(v0.y, max(v1.y, v2.y));

	//// Edge coefficients
	//int A0 = v1.y - v0.y;
	//int B0 = v0.x - v1.x;
	//int C0 = v1.x * v0.y - v0.x * v1.y;

	//int A1 = v2.y - v1.y;
	//int B1 = v1.x - v2.x;
	//int C1 = v2.x * v1.y - v1.x * v2.y;

	//int A2 = v0.y - v2.y;
	//int B2 = v2.x - v0.x;
	//int C2 = v0.x * v2.y - v2.x * v0.y;

	//for (int y = minY; y <= maxY; ++y)
	//{
	//	int e0 = A0 * minX + B0 * y + C0;
	//	int e1 = A1 * minX + B1 * y + C1;
	//	int e2 = A2 * minX + B2 * y + C2;

	//	for (int x = minX; x <= maxX; ++x)
	//	{
	//		if ((e0 >= 0 && e1 >= 0 && e2 >= 0) ||
	//			(e0 <= 0 && e1 <= 0 && e2 <= 0)) // branch-light inside test
	//			putPixel(x, y, color);

	//		e0 += A0;
	//		e1 += A1;
	//		e2 += A2;
	//	}
	//}
}

// Temp funciton (trying to respect DRY principles)
// Draws mesh in 3D space
void drawMesh(const Mesh& mesh) {
	const Mat4& mvp = calculateMVP(mesh.modelMatrix);

	int verticesCount = mesh.verticesCount;
	int facesCount = mesh.facesCount;

	Vec4* vertices = mesh.vertices;
	Face* faces = mesh.faces;

	// Extract each index / face of the mesh and draw it on screen
	for (int index = 0; index < facesCount; index++) {
		Vec4 a = vertices[faces[index].indices[0]];
		Vec4 b = vertices[faces[index].indices[1]];
		Vec4 c = vertices[faces[index].indices[2]];

		a = prodMatrixVec(mvp, a);
		b = prodMatrixVec(mvp, b);
		c = prodMatrixVec(mvp, c);

		// Clip resulting triangle
		Vec4 clippedTriangle[6] = { a, b, c };
		int count = clipTriangle(clippedTriangle);

		for (int i = 0; i < count; i++) {
			clippedTriangle[i] = toScreen(clippedTriangle[i], renderState.viewportMatrix);
		}

		for (int i = 1; i < count - 1; i++) {
			// drawTriangle(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1]);
		}

		for (int i = 1; i < count - 1; i++) {
			drawFace(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1]);
		}

		for (int i = 1; i < count - 1; i++) {
			// drawTriangle(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1]);
		}
	}
}

// Renders everything that has been drawn on the color buffer so far
void render() {
	renderState.texture.update(renderState.colorBuffer);
	sf::Sprite sprite(renderState.texture);

	screenConfig.window.draw(sprite);
	screenConfig.window.display();
	screenConfig.window.clear();
}

// Checks if a triangle is inside drawing space given its vertices
bool isInsideDrawingSpace(Vec4 a, Vec4 b, Vec4 c) {
	if (a.z + a.w < 0 && b.z + b.w < 0 && c.z + c.w < 0) return false;
	if (a.y + a.w < 0 && b.y + b.w < 0 && c.y + c.w < 0) return false;
	if (a.x + a.w < 0 && b.x + b.w < 0 && c.x + c.w < 0) return false;
	if (a.y - a.w > 0 && b.y - b.w > 0 && c.y - c.w > 0) return false;
	if (a.x - a.w > 0 && b.x - b.w > 0 && c.x - c.w > 0) return false;
	return true;
}

// Checks if C is on the left of line AB
bool isLeft(Vec4 a, Vec4 b, Vec4 c) {
	return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) < 0;
}

// Checks if point A is inside a triangle in 2D space
bool isInsideTriangle(Vec4 tri0, Vec4 tri1, Vec4 tri2, Vec4 a) {
	return isLeft(tri0, tri1, a) && isLeft(tri1, tri2, a) && isLeft(tri2, tri0, a);
}