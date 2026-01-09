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

// Draw a line on screen, taking depth into consideration
void drawLineDepth(Vec4 v0, Vec4 v1, Color color) {
	int ax = v0.x;
	int ay = v0.y;
	int bx = v1.x;
	int by = v1.y;

	float az = v0.z;
	float bz = v1.z;

	int dx = std::abs(ax - bx);
	int dy = std::abs(ay - by);

	int sx = (ax < bx) ? 1 : -1;
	int sy = (ay < by) ? 1 : -1;

	int err = dx - dy;

	// Precompute values for depth interpolation
	const float EPS = 1e-6f;

	float w0 = az;
	float w1 = bz;
	int steps = std::max(dx, dy);

	float dw = (w1 - w0) / float(steps);

	float w = w0;

	if (steps == 0) {
		int idx = ay * screenConfig.width + ax;
		if (w0 > renderState.depthBuffer[idx]) {
			renderState.depthBuffer[idx] = w0;
			putPixel(ax, ay, color);
		}
		return;
	}

	while (true) {
		int bufferIndex = ay * screenConfig.width + ax;
		if (renderState.depthBuffer[bufferIndex] < 1 / w) {
			float temp = 1 / w;
			putPixel(ax, ay, {(unsigned char)(255 * temp), (unsigned char)(255 * temp), (unsigned char)(255 * temp)});
			renderState.depthBuffer[bufferIndex] = 1 / w;

		}

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

		w += dw;
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
	// Assume v0 is top-most, v1 is to the left of v2 (for the span) and v1.y == v2.y
	float y_start = v0.y;
	float y_end = v1.y;
	float dy = y_end - y_start;
	if (dy <= 0) return;	// The triangle is pointed downards

	float dx_left = (v1.x - v0.x) / dy;
	float dx_right = (v2.x - v0.x) / dy;

	float dz_left = (v1.z - v0.z) / dy;
	float dz_right = (v2.z - v0.z) / dy;

	float cr_x_left = v0.x;
	float cr_x_right = v0.x;
	float cr_z_left = v0.z;
	float cr_z_right = v0.z;

	for (int y = y_start; y <= y_end; ++y) {
		int span_x_start = (int)cr_x_left;
		int span_x_end = (int)cr_x_right;

		float span_z_start = cr_z_left;
		float span_z_end = cr_z_right;

		// Calculate 1/z change per pixel along this specific horizontal span
		float span_width = (float)(span_x_end - span_x_start);
		float dz_span = 0;
		if (span_width > 0) {
			dz_span = (span_z_end - span_z_start) / (float)span_width;
		}

		float cz_w = span_z_start;

		for (int x = span_x_start; x <= span_x_end; x++) {
			if (x >= 0 && x < screenConfig.width && y >= 0 && y < screenConfig.height) {
				int idx = y * screenConfig.width + x;
				// Standard 1/z test: Higher is closer
				if (1 / cz_w > renderState.depthBuffer[idx]) {
					renderState.depthBuffer[idx] = 1 / cz_w;
					putPixel(x, y, color);
				}
			}
			cz_w += dz_span;
		}

		float current_w = span_z_start;

		cr_x_left += dx_left;
		cr_x_right += dx_right;
		cr_z_left += dz_left;
		cr_z_right += dz_right;
	}
}

// Draw the bottom part of a triangle
void drawBottomTriangle(Vec4 v0, Vec4 v1, Vec4 v2, Color color) {
	// v0 becomes the bottom-most, v2 top-most and v1 in-between vertically wise
	if (v1.y > v0.y) std::swap(v1, v0);
	if (v2.y > v0.y) std::swap(v2, v0);
	if (v2.y > v1.y) std::swap(v1, v2);

	int x0 = v0.x, y0 = v0.y, z0 = v0.z;
	int x1 = v1.x, y1 = v1.y, z1 = v1.z;
	int x2 = v2.x, y2 = v2.y, z2 = v2.z;

	// Fill possible missing gaps on edges
	drawLineDepth(v0, v1, color);
	drawLineDepth(v0, v2, color);
	drawLineDepth(v2, v1, color);

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

	int steps1 = max(dx01, dy01);
	int steps2 = max(dx02, dy02);

	float dw01 = (z1 - z0) / (float)steps1;
	float dw02 = (z2 - z0) / (float)steps2;

	while (y0 > y1 && y0_t > y1) {
		drawLineDepth(v0, v1, color);

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

		v0.x = x0, v0.y = y0;
		v1.x = x0_t, v1.y = y0_t;
		//v0.z += dw01, v1.z += dw02;
	}
	drawLineDepth(v0, v1, color);
}

float edgeFunction(const Vec4& a, const Vec4& b, const Vec4& c) {
	// Standard 2D cross product (E(p) where p == c)
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void drawTriangleOptimized(Vec4 v0, Vec4 v1, Vec4 v2, Color color) {
	int minX = max(0, (int)std::floor(std::min({ v0.x, v1.x, v2.x })));
	int maxX = min(screenConfig.width - 1, (int)std::ceil(std::max({ v0.x, v1.x, v2.x })));
	int minY = max(0, (int)std::floor(std::min({ v0.y, v1.y, v2.y })));
	int maxY = min(screenConfig.height - 1, (int)std::ceil(std::max({ v0.y, v1.y, v2.y })));

	float area = edgeFunction(v0, v1, v2);
	if (std::abs(area) < 1e-6f) return;
	float invArea = 1.0f / area;
	bool areaPositive = (area > 0.0f);

	// Setup edge constants (A, B) and initial edge values at pixel center (minX+0.5, minY+0.5)
	auto setupEdge = [](const Vec4& a, const Vec4& b, float startX, float startY, float& A, float& B) -> float {
		A = (b.y - a.y);     // change when x += 1
		B = (a.x - b.x);     // change when y += 1
		return (startX - a.x) * (b.y - a.y) - (startY - a.y) * (b.x - a.x);
		};

	float A0, B0, A1, B1, A2, B2;
	float sampleX = (float)minX + 0.5f;
	float sampleY = (float)minY + 0.5f;

	// edges: w0 = edge(v1, v2, p), w1 = edge(v2, v0, p), w2 = edge(v0, v1, p)
	float w0_row = setupEdge(v1, v2, sampleX, sampleY, A0, B0);
	float w1_row = setupEdge(v2, v0, sampleX, sampleY, A1, B1);
	float w2_row = setupEdge(v0, v1, sampleX, sampleY, A2, B2);

	// perspective 1/z per vertex (for interpolation)
	float invZ0 = 1.0f / v0.z;
	float invZ1 = 1.0f / v1.z;
	float invZ2 = 1.0f / v2.z;

	for (int y = minY; y <= maxY; ++y) {
		float w0 = w0_row;
		float w1 = w1_row;
		float w2 = w2_row;

		for (int x = minX; x <= maxX; ++x) {
			// inside test depends on triangle winding
			bool inside = areaPositive
				? (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
				: (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);

			if (inside) {
				// barycentric coordinates
				float l0 = w0 * invArea;
				float l1 = w1 * invArea;
				float l2 = w2 * invArea;

				float interpInvZ = l0 * invZ0 + l1 * invZ1 + l2 * invZ2;
				if (interpInvZ > 0.0f) {
					float curr_z = 1.0f / interpInvZ;
					int idx = y * screenConfig.width + x;
					if (curr_z > renderState.depthBuffer[idx]) {
						renderState.depthBuffer[idx] = curr_z;
						putPixel(x, y, color);
					}

					//putPixel(x, y, {(unsigned char)(255.f * curr_z), (unsigned char)(255.f * curr_z), (unsigned char)(255.f * curr_z) });
				}
			}

			// step in x
			w0 += A0;
			w1 += A1;
			w2 += A2;
		}

		// step in y
		w0_row += B0;
		w1_row += B1;
		w2_row += B2;
	}
}

// Draw a filled triangle on 2D screen using a scanline algorithm
void drawFace(Vec4 v0, Vec4 v1, Vec4 v2, Vec3 normal, Color color)
{
	Mat4& view = renderState.viewMatrix;

	Vec3 cameraLeft = { -view.m[0][0], -view.m[0][1], -view.m[0][2] };
	Vec3 cameraUp = { view.m[1][0], view.m[1][1], view.m[1][2] };
	Vec3 cameraForward = { -view.m[2][0], -view.m[2][1], -view.m[2][2] };

	Vec3 LOS = normalize(cameraState.camPosition - cameraForward);
	// std::cout << dot(normalize(cameraState.camPosition), normal) << '\n';

	drawTriangleOptimized(v0, v1, v2, color);

	// v0 becomes the top point
	// v1 the mid point, and v2 the bottom point


	// drawTopTriangle(v0, v1, v2, color);
	// drawBottomTriangle(v0, v1, v2, color);
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
		Vec3 normal = faces[index].normal;
		Color color = faces[index].color;

		a = prodMatrixVec(mvp, a);
		b = prodMatrixVec(mvp, b);
		c = prodMatrixVec(mvp, c);

		// Clip resulting triangle
		Vec4 clippedTriangle[6] = { a, b, c };
		int count = clipTriangle(clippedTriangle);

		for (int i = 0; i < count; i++) {
			clippedTriangle[i] = toScreen(clippedTriangle[i], renderState.viewportMatrix);
			// clippedTriangle[i].z = 1 / clippedTriangle[i].z;
		}

		for (int i = 1; i < count - 1; i++) {
			drawFace(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1], normal, color);
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
	memset(renderState.depthBuffer, 0.0f, screenConfig.width * screenConfig.height * sizeof(float));
	screenConfig.window.draw(sprite);
	screenConfig.window.display();
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