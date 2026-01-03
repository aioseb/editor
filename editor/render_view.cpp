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

	drawLine(halfWidth + cameraRight.x, halfHeight + cameraRight.y, halfWidth, halfHeight, sf::Color::Red);			// X axis
	drawLine(halfWidth + cameraForward.x, halfHeight + cameraForward.y, halfWidth, halfHeight, sf::Color::Blue);	// Z axis
	drawLine(halfWidth + cameraUp.x, halfHeight + cameraUp.y, halfWidth, halfHeight, sf::Color::Green);				// Y axis
}

// Draw a line on screen, given both coordinates and an optional color
void drawLine(int ax, int ay, int bx, int by, sf::Color color) {
	sf::Vertex line[2];

	line[0].position = sf::Vector2f(ax, ay);
	line[1].position = sf::Vector2f(bx, by);
	line[0].color = color;
	line[1].color = color;

	screenConfig.window.draw(line, 2, sf::PrimitiveType::Lines);
}

// Draw a triangle on the screen, given local coordinates and the MVP
void drawTriangle(Vec4 a, Vec4 b, Vec4 c) {
	drawLine(a.x, a.y, b.x, b.y);
	drawLine(b.x, b.y, c.x, c.y);
	drawLine(c.x, c.y, a.x, a.y);
}

// Draw a filled triangle on the screen, given local coordinates and the MVP
void drawFace(Vec4 a, Vec4 b, Vec4 c) {
	//int lastColor = getcolor();
	//setfillstyle(SOLID_FILL, BLUE);

	//int points[6] = { a.x, a.y, b.x, b.y, c.x, c.y };
	//fillpoly(3, points);

	//setcolor(lastColor);
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
			drawTriangle(clippedTriangle[0], clippedTriangle[i], clippedTriangle[i + 1]);
		}
	}
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