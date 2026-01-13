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

// Draw a line in the scene, given world space coordinates
void drawLineWorldSpace(Vec4 a, Vec4 b, Color color) {
	Mat4& view = renderState.viewMatrix;
	Mat4& proj = renderState.projMatrix;
	Mat4 viewProj = prodMatrix(proj, view);

	a = prodMatrixVec(viewProj, a);
	b = prodMatrixVec(viewProj, b);

	// Clip line against the 6 planes of NDC cube
	for (int planeIndex = 0; planeIndex < 6; ++planeIndex) {
		const Vec4& plane = planes[planeIndex];
		Vec4 intersectedPoint = intersection(a, b, plane);

		bool insidePlaneA = insidePlane(a, plane);
		bool insidePlaneB = insidePlane(b, plane);

		// If both are outside the plane, discard the line
		if (!insidePlaneA && !insidePlaneB) {
			return;
		}

		if (!insidePlaneA) {
			a = intersectedPoint;
		}
		else if (!insidePlaneB) {
			b = intersectedPoint;
		}
	}

	Mat4& viewport = renderState.viewportMatrix;
	a = toScreen(a, viewport);
	b = toScreen(b, viewport);

	drawLine(a.x, a.y, b.x, b.y, color);
}

// Draw a triangle on the screen, given local coordinates and the MVP
void drawTriangle(const Vec4& a, const Vec4& b, const Vec4& c, Color color) {
	drawLine(a.x, a.y, b.x, b.y, color);
	drawLine(b.x, b.y, c.x, c.y, color);
	drawLine(c.x, c.y, a.x, a.y, color);
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

float setupEdge(const Vec4 & a, const Vec4 & b, float startX, float startY, float& A, float& B, float& C){
	A = (b.y - a.y);
	B = (a.x - b.x);
	C = -B * a.y - A * a.x;
	return (startX - a.x) * A + (startY - a.y) * B;
};

// Check if a tile is fully inside an edge
inline bool tileInside(float w, float A, float B, bool areaPositive)
{
	float w0 = w;                  // top-left
	float w1 = w + A - 1;          // top-right
	float w2 = w + B - 1;          // bottom-left
	float w3 = w + A + B - 2;      // bottom-right

	if (areaPositive) {
		return (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f && w3 >= 0.0f);
	}
	else {
		return (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f && w3 <= 0.0f);
	}
}

// Check if a tile is fully outside an edge
// Check if a tile is fully inside an edge
inline bool tileOutside(float w, float A, float B, bool areaPositive)
{
	float w0 = w;                  // top-left
	float w1 = w + A - 1;          // top-right
	float w2 = w + B - 1;          // bottom-left
	float w3 = w + A + B - 2;      // bottom-right

	if (areaPositive) {
		return (w0 < 0.0f && w1 < 0.0f && w2 < 0.0f && w3 < 0.0f);
	}
	else {
		return (w0 > 0.0f && w1 > 0.0f && w2 > 0.0f && w3 > 0.0f);
	}
}

void drawTriangleDepth(const Vec4& v0, const Vec4& v1, const Vec4& v2, Color color) {
	float area = edgeFunction(v0, v1, v2);
	if (std::abs(area) < 1e-6f) return;

	const float TILE_SIZE = 32.0f;

	int minX = max(0, (int)std::floor(std::min({ v0.x, v1.x, v2.x })));
	int maxX = min(screenConfig.width - 1, (int)std::ceil(std::max({ v0.x, v1.x, v2.x })));
	int minY = max(0, (int)std::floor(std::min({ v0.y, v1.y, v2.y })));
	int maxY = min(screenConfig.height - 1, (int)std::ceil(std::max({ v0.y, v1.y, v2.y })));

	float invArea = 1.0f / area;
	bool areaPositive = (area > 0.0f);

	float A0, B0, C0, A1, B1, C1, A2, B2, C2;
	float sampleX = (float)minX + 0.5f;
	float sampleY = (float)minY + 0.5f;

	float w0_row = setupEdge(v1, v2, sampleX, sampleY, A0, B0, C0);
	float w1_row = setupEdge(v2, v0, sampleX, sampleY, A1, B1, C1);
	float w2_row = setupEdge(v0, v1, sampleX, sampleY, A2, B2, C2);

	float z0 = v0.z;
	float z1 = v1.z;
	float z2 = v2.z;
	
	// Precompute coefficients of p(x, y) = ap * x + bp * y + cp
	float ap = (A0 * z0 + A1 * z1 + A2 * z2) * invArea;
	float bp = (B0 * z0 + B1 * z1 + B2 * z2) * invArea;
	float cp = (C0 * z0 + C1 * z1 + C2 * z2) * invArea;

	// Precompute coefficients of q(x, y) = aq * x + bq * y + cq
	float aq = (A0 + A1 + A2) * invArea;
	float bq = (B0 + B1 + B2) * invArea;
	float cq = (C0 + C1 + C2) * invArea;

	float px_row = ap * sampleX + bp * sampleY + cp;
	float qx_row = aq * sampleX + bq * sampleY + cq;

	float tileA0, tileA1, tileA2, tileB0, tileB1, tileB2;
	tileA0 = A0 * TILE_SIZE;
	tileA1 = A1 * TILE_SIZE;
	tileA2 = A2 * TILE_SIZE;
	tileB0 = B0 * TILE_SIZE;
	tileB1 = B1 * TILE_SIZE;
	tileB2 = B2 * TILE_SIZE;

	for (int y = minY; y <= maxY; y += (int)TILE_SIZE) {
		float w0 = w0_row;
		float w1 = w1_row;
		float w2 = w2_row;

		float px = px_row;
		float qx = qx_row;

		for (int x = minX; x <= maxX; x += (int)TILE_SIZE) {
			bool tileInside0 = tileInside(w0, tileA0, tileB0, areaPositive);
			bool tileInside1 = tileInside(w1, tileA1, tileB1, areaPositive);
			bool tileInside2 = tileInside(w2, tileA2, tileB2, areaPositive);
			bool allInside = tileInside0 && tileInside1 && tileInside2;

			bool tileOutside0 = tileOutside(w0, tileA0, tileB0, areaPositive);
			bool tileOutside1 = tileOutside(w1, tileA1, tileB1, areaPositive);
			bool tileOutside2 = tileOutside(w2, tileA2, tileB2, areaPositive);
			bool oneOutside = tileOutside0 || tileOutside1 || tileOutside2;

			// Skip if there is a tile fully outside
			if (oneOutside) {
				w0 += tileA0;
				w1 += tileA1;
				w2 += tileA2;

				px += ap * TILE_SIZE;
				qx += aq * TILE_SIZE;
				continue;
			}

			// If all tiles are fully inside, fill tile without
			// taking into account barycentric weights (w0, w1, w2)
			if (allInside) {
				for (int row = 0; row < (int)TILE_SIZE; ++row) {
					float _px = px + (float)row * bp;
					float _qx = qx + (float)row * bq;

					for (int col = 0; col < (int)TILE_SIZE; ++col){
						int idx = (y + row) * screenConfig.width + (x + col);
						float currentDepth = renderState.depthBuffer[idx];

						if (_px > currentDepth * _qx) {
							float ndc_z = _px / _qx;
							renderState.depthBuffer[idx] = ndc_z;

							if (renderConfig.drawDepthMap) {
								unsigned char depthValue = (unsigned char)(255.f * ndc_z);
								if (depthValue > 255) depthValue = 255;
								putPixel(x + col, y + row, { depthValue, depthValue, depthValue });
							}
							else {
								putPixel(x + col, y + row, color);
							}
						}

						_px += ap;
						_qx += aq;
					}
				}
			}
			else {
				// The given tile is partially inside the edge
				// Take baryocentric weights into consideration
				for (int row = 0; row < (int)TILE_SIZE; ++row) {
					float _px = px + (float)row * bp;
					float _qx = qx + (float)row * bq;

					float _w0 = w0 + (float)row * B0;
					float _w1 = w1 + (float)row * B1;
					float _w2 = w2 + (float)row * B2;

					for (int col = 0; col < (int)TILE_SIZE; ++col) {
						bool inside = false;
						if ((areaPositive && (_w0 >= 0.0f && _w1 >= 0.0f && _w2 >= 0.0f)) ||
							(!areaPositive && (_w0 <= 0.0f && _w1 <= 0.0f && _w2 <= 0.0f))) {
							inside = true;
						}

						if (inside) {
							int idx = (y + row) * screenConfig.width + (x + col);
							float currentDepth = renderState.depthBuffer[idx];

							if (_px > currentDepth * _qx) {
								float ndc_z = _px / _qx;

								if (ndc_z > renderState.depthBuffer[idx]) {
									renderState.depthBuffer[idx] = ndc_z;

									if (renderConfig.drawDepthMap) {
										unsigned char depthValue = (unsigned char)(255.f * ndc_z);
										if (depthValue > 255) depthValue = 255;
										putPixel(x + col, y + row, { depthValue, depthValue, depthValue });
									}
									else {
										putPixel(x + col, y + row, color);
									}
								}
							}
						}

						_w0 += A0;
						_w1 += A1;
						_w2 += A2;

						_px += ap;
						_qx += aq;
					}
				}
			}

			// step in x
			w0 += tileA0;
			w1 += tileA1;
			w2 += tileA2;

			px += ap * TILE_SIZE;
			qx += aq * TILE_SIZE;
		}

		// step in y
		w0_row += tileB0;
		w1_row += tileB1;
		w2_row += tileB2;

		px_row += bp * TILE_SIZE;
		qx_row += bq * TILE_SIZE;
	}
}

// Draw a filled triangle on 2D screen using a scanline algorithm
void drawFace(Vec4 v0, Vec4 v1, Vec4 v2, Vec3 normal, Color color) {
	drawTriangleDepth(v0, v1, v2, color);
}

// Draws mesh in 3D space
void drawMesh(const Mesh& mesh, bool drawEdges, Color edgesColor) {
	const Mat4& mvp = calculateMVP(mesh.modelMatrix);
	Mat4 rotationMatrix = modelMatrix(zeroVector, oneVector, mesh.rotation);

	int verticesCount = mesh.verticesCount;
	int facesCount = mesh.facesCount;

	Vec4* vertices = mesh.vertices;
	Face* faces = mesh.faces;

	bool selectedVertexDrawn = false;

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

		if (renderConfig.drawWireframe || drawEdges) {
			for (int i = 1; i < count - 1; i++) {
				drawTriangle(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1], edgesColor);
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