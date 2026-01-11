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

float edgeFunction(const Vec4& a, const Vec4& b, const Vec4& c) {
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void drawTriangleDepth(const Vec4& v0, const Vec4& v1, const Vec4& v2, Color color) {
	float area = edgeFunction(v0, v1, v2);
	if (std::abs(area) < 1e-6f) return;

	int minX = max(0, (int)std::floor(std::min({ v0.x, v1.x, v2.x })));
	int maxX = min(screenConfig.width - 1, (int)std::ceil(std::max({ v0.x, v1.x, v2.x })));
	int minY = max(0, (int)std::floor(std::min({ v0.y, v1.y, v2.y })));
	int maxY = min(screenConfig.height - 1, (int)std::ceil(std::max({ v0.y, v1.y, v2.y })));

	float invArea = 1.0f / area;
	bool areaPositive = (area > 0.0f);

	auto setupEdge = [](const Vec4& a, const Vec4& b, float startX, float startY, float& A, float& B, float& C) -> float {
		A = (b.y - a.y);
		B = (a.x - b.x);
		C = -B * a.y - A * a.x;
		return (startX - a.x) * A + (startY - a.y) * B;
		};

	float A0, B0, C0, A1, B1, C1, A2, B2, C2;
	float sampleX = (float)minX + 0.5f;
	float sampleY = (float)minY + 0.5f;

	float w0_row = setupEdge(v1, v2, sampleX, sampleY, A0, B0, C0);
	float w1_row = setupEdge(v2, v0, sampleX, sampleY, A1, B1, C1);
	float w2_row = setupEdge(v0, v1, sampleX, sampleY, A2, B2, C2);

	float invW0 = 1.0f / v0.w;
	float invW1 = 1.0f / v1.w;
	float invW2 = 1.0f / v2.w;

	float zOverW0 = v0.z * invW0;
	float zOverW1 = v1.z * invW1;
	float zOverW2 = v2.z * invW2;
	
	// Precompute coefficients of p(x, y) = ap * x + bp * y + cp
	float ap = (A0 * zOverW0 + A1 * zOverW1 + A2 * zOverW2) * invArea;
	float bp = (B0 * zOverW0 + B1 * zOverW1 + B2 * zOverW2) * invArea;
	float cp = (C0 * zOverW0 + C1 * zOverW1 + C2 * zOverW2) * invArea;

	// Precompute coefficients of q(x, y) = aq * x + bq * y + cq
	float aq = (A0 * invW0 + A1 * invW1 + A2 * invW2) * invArea;
	float bq = (B0 * invW0 + B1 * invW1 + B2 * invW2) * invArea;
	float cq = (C0 * invW0 + C1 * invW1 + C2 * invW2) * invArea;

	float px_row = ap * sampleX + bp * sampleY + cp;
	float qx_row = aq * sampleX + bq * sampleY + cq;

	for (int y = minY; y <= maxY; ++y) {
		float w0 = w0_row;
		float w1 = w1_row;
		float w2 = w2_row;

		float px = px_row;
		float qx = qx_row;

		for (int x = minX; x <= maxX; ++x) {
			bool inside = false;
			if ((areaPositive && (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)) ||
				(!areaPositive && (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f))) {
				inside = true;
			}

			if (inside) {
				int idx = y * screenConfig.width + x;
				float currentDepth = renderState.depthBuffer[idx];

				if (px > currentDepth * qx) {
					float ndc_z = px / qx; 

					if (ndc_z > renderState.depthBuffer[idx]) {
						renderState.depthBuffer[idx] = ndc_z;

						if (renderConfig.drawDepthMap) {
							unsigned char depthValue = (unsigned char)(255.f * ndc_z);
							if (depthValue > 255) depthValue = 255;
							putPixel(x, y, { depthValue, depthValue, depthValue });
						}
						else {
							putPixel(x, y, color);
						}
					}
				}
			}

			// step in x
			w0 += A0;
			w1 += A1;
			w2 += A2;

			px += ap;
			qx += aq;
		}

		// step in y
		w0_row += B0;
		w1_row += B1;
		w2_row += B2;

		px_row += bp;
		qx_row += bq;
	}
}

// Draw a filled triangle on 2D screen using a scanline algorithm
void drawFace(Vec4 v0, Vec4 v1, Vec4 v2, Vec3 normal, Color color) {
	drawTriangleDepth(v0, v1, v2, color);
}

// Draws mesh in 3D space
void drawMesh(const Mesh& mesh) {
	const Mat4& mvp = calculateMVP(mesh.modelMatrix);
	Mat4 rotationMatrix = modelMatrix(zeroVector, oneVector, mesh.rotation);

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
		Vec4 homogenousNormal = { normal.x, normal.y, normal.z, 0.0f };
		Color color = faces[index].color;

		Vec4 faceVertex = a;
		faceVertex = prodMatrixVec(mesh.modelMatrix, faceVertex);

		Vec4 viewVector = {
			faceVertex.x - cameraState.camPosition.x,
			faceVertex.y - cameraState.camPosition.y,
			faceVertex.z - cameraState.camPosition.z,
			0.0f
		};

		// If triangle is facing away from the camera, skip it
		homogenousNormal = prodMatrixVec(rotationMatrix, homogenousNormal);
		if (dot(homogenousNormal, viewVector) > 0) continue;
		float diff = dot(homogenousNormal, lightConfig.lightDirection);

		if (diff < 0.0f) {
			diff = 0.0f;
		}

		float intensity = diff + lightConfig.ambientIntensity;
		if (intensity > 1.0f) intensity = 1.0f;
		Color shadedColor = { (unsigned char)(color.r * intensity), (unsigned char)(color.g * intensity), (unsigned char)(color.b * intensity) };

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
			drawFace(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1], normal, shadedColor);
		}

		if (renderConfig.drawWireframe) {
			for (int i = 1; i < count - 1; i++) {
				drawTriangle(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1]);
			}
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