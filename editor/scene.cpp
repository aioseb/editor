#include "scene.h"

Scene scene;

// Adds a mesh in the given scene and returns its index
int addMesh(Mesh& mesh) {
	if (scene.meshCount >= scene.MAX_MESH_COUNT) return -1;	// Scene is full - return -1
	scene.meshes[scene.meshCount++] = mesh;
	return scene.meshCount - 1;
}

// Returns the mesh at the given index
Mesh* getMesh(int index) {
	if (index < 0 || index >= scene.meshCount) return nullptr;
	return &scene.meshes[index];
}

// Draws all meshes in the scene
void drawScene() {
	for (int meshIndex = 0; meshIndex < scene.meshCount; ++meshIndex) {
		drawMesh(scene.meshes[meshIndex]);
		if (meshIndex == scene.selectedIndex) {
			drawMesh(scene.meshes[meshIndex], true, { 255, 255, 0 });
		}
		else {
			drawMesh(scene.meshes[meshIndex]);
		}
	}
	drawSelectedVertex();

	// Draw a green border if in edit mode
	if (renderConfig.editMode) {
		int x0 = 0, y0 = 0;
		int x1 = screenConfig.width, y1 = screenConfig.height;

		const Color LIME = { 77, 255, 77 };
		for (int k = 0; k < 6; k++) {
			drawLine(x0, y0, x1, y0, LIME);
			drawLine(x0, y0, x0, y1, LIME);
			drawLine(x1, y1, x1, y0, LIME);
			drawLine(x1, y1, x0, y1, LIME);

			++x0, ++y0, --x1, --y1;
		}
	}
}

// Draws selected vertex
void drawSelectedVertex() {
	Mesh* selectedMesh = selected();
	if (selectedMesh == nullptr) return;
	if (scene.selectedVertexIndex < 0 || scene.selectedVertexIndex >= selectedMesh->verticesCount) return;

	// Transform vertex to screen space
	Vec4 selectedVertex = selectedMesh->vertices[scene.selectedVertexIndex];
	selectedVertex = prodMatrixVec(selectedMesh->modelMatrix, selectedVertex);
	selectedVertex = prodMatrixVec(renderState.viewMatrix, selectedVertex);
	selectedVertex = prodMatrixVec(renderState.projMatrix, selectedVertex);
	selectedVertex = toScreen(selectedVertex, renderState.viewportMatrix);

	// If point was sent off-screen by toScreen (behind camera / invalid), skip
	if (selectedVertex.x < -90000.0f || selectedVertex.y < -90000.0f) return;

	// Center and square half-size in pixels
	int cx = static_cast<int>(std::lround(selectedVertex.x));
	int cy = static_cast<int>(std::lround(selectedVertex.y));
	const int half = 16;
	int x0 = cx - half;
	int y0 = cy - half;
	int x1 = cx + half;
	int y1 = cy + half;

	// Draw thick square around selected vertex
	const Color ORANGE = { 255, 165, 0 };
	const Color CRIMSON = { 220, 20, 60 };
	Color boundaryColor = CRIMSON;

	for (int k = 0; k < 6; k++) {
		++x0, ++y0, --x1, --y1;

		if (k >= 3) {
			boundaryColor = ORANGE;
		}

		if (x0 < 0 || y0 < 0 || x0 >= screenConfig.width || y0 >= screenConfig.height) break;
		if (x1 < 0 || y1 < 0 || x1 >= screenConfig.width || y1 >= screenConfig.height) break;

		drawLine(x0, y0, x1, y0, boundaryColor); // top
		drawLine(x1, y0, x1, y1, boundaryColor); // right
		drawLine(x1, y1, x0, y1, boundaryColor); // bottom
		drawLine(x0, y1, x0, y0, boundaryColor); // left
	}
}

// Returns the currently selected mesh
Mesh* selected() {
	if (scene.selectedIndex < 0) return nullptr;
	return &scene.meshes[scene.selectedIndex];
}

// Translated selected object by given amount
void translateSelectedBy(const Vec3& translation) {
	Mesh* selectedMesh = selected();

	if (selectedMesh == nullptr) return;
	translateBy(*selectedMesh, translation);
}

// Rotate selected object by given amount 
void rotateSelectedBy(const Vec3& rotation) {
	Mesh* selectedMesh = selected();

	if (selectedMesh == nullptr) return;
	rotateBy(*selectedMesh, rotation);
}

// Scale selected object by given amount
void scaleSelectedBy(const Vec3& scaling) {
	Mesh* selectedMesh = selected();

	if (selectedMesh == nullptr) return;
	scaleBy(*selectedMesh, scaling);
}

// Translate selected vertex by translation relative to camera direction
void translateVertexBy(const Vec3& translation) {
	Mesh* selectedMesh = selected();
	if (selectedMesh == nullptr) return;
	int idx = scene.selectedVertexIndex;
	if (idx < 0 || idx >= selectedMesh->verticesCount) return;

	const Mat4& view = renderState.viewMatrix;
	Vec3 cameraRight = { view.m[0][0], view.m[0][1], view.m[0][2] };
	Vec3 cameraUp = { view.m[1][0], view.m[1][1], view.m[1][2] };
	Vec3 cameraForward = { -view.m[2][0], -view.m[2][1], -view.m[2][2] };

	// Interpret the incoming translation along
	// cameraRight (x), cameraUp (y) and cameraForward (z).
	Vec3 worldDelta = cameraRight * translation.x;
	worldDelta = worldDelta + (cameraUp * translation.y);
	worldDelta = worldDelta + (cameraForward * translation.z);

	// Convert the selected vertex to world space, apply world-space translation,
	// then transform back to model/local space and store.
	Vec4 localPos = selectedMesh->vertices[idx];
	Vec4 worldPos = prodMatrixVec(selectedMesh->modelMatrix, localPos);

	Vec4 worldMoved = { worldPos.x + worldDelta.x, worldPos.y + worldDelta.y, worldPos.z + worldDelta.z, 1.0f };

	Mat4 invModel = inverseMatrix(selectedMesh->modelMatrix);
	Vec4 newLocal = prodMatrixVec(invModel, worldMoved);
	selectedMesh->vertices[idx] = newLocal;

	const float EPS = 1e-6f;
	for (int f = 0; f < selectedMesh->facesCount; ++f) {
		Face& face = selectedMesh->faces[f];
		int a = face.indices[0];
		int b = face.indices[1];
		int c = face.indices[2];

		if (a == idx || b == idx || c == idx) {
			// Get positions in model/local space (Vec4 -> Vec3)
			Vec4 A4 = selectedMesh->vertices[a];
			Vec4 B4 = selectedMesh->vertices[b];
			Vec4 C4 = selectedMesh->vertices[c];

			Vec3 A = { A4.x, A4.y, A4.z };
			Vec3 B = { B4.x, B4.y, B4.z };
			Vec3 C = { C4.x, C4.y, C4.z };

			Vec3 e1 = B - A;
			Vec3 e2 = C - A;
			Vec3 n = cross(e1, e2);
			float nlen2 = dot(n, n);
			if (nlen2 > EPS) {
				Vec3 newN = normalize(n);

				// Preserve original normal direction (sign).
				// We do this because triangles can either be clokwise or counter-clockwise oriented
				float origLen2 = dot(face.normal, face.normal);
				if (origLen2 > EPS) {
					if (dot(newN, face.normal) < 0.0f) {
						newN = newN * -1.0f;
					}
				}

				face.normal = newN;
			}
		}
	}
}

// Generates a sphere with its origin at the center of weight of the mesh
// and the radius equal to the longest distance from the center to any vertex
Sphere computeBoundingSphere(const Mesh& mesh) {
	Vec3 center = zeroVector;
	float radius = 0.0f;
	int n = mesh.verticesCount;

	for (int vertexIndex = 0; vertexIndex < n; vertexIndex++) {
		center.x += mesh.vertices[vertexIndex].x;
		center.y += mesh.vertices[vertexIndex].y;
		center.z += mesh.vertices[vertexIndex].z;
	}

	center.x /= n;
	center.y /= n;
	center.z /= n;

	for (int vertexIndex = 0; vertexIndex < n; vertexIndex++) {
		Vec3 d;
		d.x = mesh.vertices[vertexIndex].x - center.x;
		d.y = mesh.vertices[vertexIndex].y - center.y;
		d.z = mesh.vertices[vertexIndex].z - center.z;

		float dist2 = dot(d, d);
		if (radius < dist2) {
			radius = dist2;
		}
	}

	return { center, sqrtf(radius) };
}

// Cast a 3D world-space ray from screen to mouse position
void screenToWorldRay(int mouseX, int mouseY, Vec3& outOrigin, Vec3& outDir) {
	const Mat4& view = renderState.viewMatrix;
	const Mat4& proj = renderState.projMatrix;
	const float W = (float)screenConfig.width;
	const float H = (float)screenConfig.height;

	Mat4 invViewProj = inverseMatrix(prodMatrix(proj, view));

	// NDC
	float ndcX = (2.0f * (float)mouseX / W) - 1.0f;
	float ndcY = 1.0f - (2.0f * (float)mouseY / H);

	// Unproject far clip-space points, then do perspective divide
	Vec4 clipFar = { ndcX, ndcY,  1.0f, 1.0f };
	Vec4 worldFar = prodMatrixVec(invViewProj, clipFar);

	if (fabs(worldFar.w) > 1e-6f) {
		worldFar.x /= worldFar.w; worldFar.y /= worldFar.w; worldFar.z /= worldFar.w;
	}

	Vec3 pFar = { worldFar.x, worldFar.y, worldFar.z };

	outOrigin = cameraState.camPosition;
	outDir = normalize(pFar - outOrigin);
}

// Ray-sphere intersection (returns true and t if hit, t is distance along ray)
bool rayIntersectSphere(const Vec3& ro, const Vec3& rd, const Sphere& s, float& outT) {
	Vec3 L = s.center - ro;		// L - vector pointing towards sphere center
	float tca = dot(L, rd);		// Distance from ro to the closest point to sphere center
	float d2 = dot(L, L) - tca * tca;	// Squared distance from ray to sphere center
	float r2 = s.radius * s.radius;		// Squared center radius

	if (d2 > r2) return false;

	float thc = sqrtf(r2 - d2);	// Distance along ray from closest point to the sphere to each intersection point

	float t0 = tca - thc;	// Entry point
	float t1 = tca + thc;	// Exit point

	if (t0 < 0 && t1 < 0) return false;

	outT = (t0 > 0) ? t0 : t1;
	return true;
}

// Moller–Trumbore ray/triangle intersection (returns hit and t)
bool rayIntersectTriangle(const Vec3& ro, const Vec3& rd,
	const Vec3& v0, const Vec3& v1, const Vec3& v2, float& outT) {

	const float EPS = 1e-6f;
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;

	Vec3 p = cross(rd, e2);
	float det = dot(e1, p);
	if (fabs(det) < EPS) return false;

	float invDet = 1.0f / det;

	Vec3 t = ro - v0;
	float u = dot(t, p) * invDet;
	if (u < 0.0f || u > 1.0f) return false;

	Vec3 q = cross(t, e1);
	float v = dot(rd, q) * invDet;
	if (v < 0.0f || (u + v) > 1.0f) return false;

	float tt = dot(e2, q) * invDet;
	if (tt > EPS) {
		outT = tt;
		return true;
	}
	return false;
}

// Select mesh under mouse (returns selected index or -1). If multiple hits pick nearest.
int pickMeshAt(int mouseX, int mouseY) {
	Vec3 ro, rd;
	screenToWorldRay(mouseX, mouseY, ro, rd);

	float bestT = FLT_MAX;
	int bestIndex = -1;

	for (int i = 0; i < scene.meshCount; ++i) {
		Mesh& m = scene.meshes[i];
		// Bounding sphere in model space
		Sphere s = computeBoundingSphere(m);

		// Transform sphere center by model matrix (approximate scaling by max scale)
		Vec4 centerLocal = { s.center.x, s.center.y, s.center.z, 1.0f };
		Vec4 centerWorld4 = prodMatrixVec(m.modelMatrix, centerLocal);
		Vec3 centerWorld = { centerWorld4.x, centerWorld4.y, centerWorld4.z };

		// Conservative radius scaling: compute scale from modelMatrix columns (max column length)
		// We do this because the sphere's radius changes upon mesh transformation
		float sx = sqrtf(m.modelMatrix.m[0][0] * m.modelMatrix.m[0][0] + m.modelMatrix.m[0][1] * m.modelMatrix.m[0][1] + m.modelMatrix.m[0][2] * m.modelMatrix.m[0][2]);
		float sy = sqrtf(m.modelMatrix.m[1][0] * m.modelMatrix.m[1][0] + m.modelMatrix.m[1][1] * m.modelMatrix.m[1][1] + m.modelMatrix.m[1][2] * m.modelMatrix.m[1][2]);
		float sz = sqrtf(m.modelMatrix.m[2][0] * m.modelMatrix.m[2][0] + m.modelMatrix.m[2][1] * m.modelMatrix.m[2][1] + m.modelMatrix.m[2][2] * m.modelMatrix.m[2][2]);
		float maxScale = std::max({ sx, sy, sz });
		Sphere worldSphere = { centerWorld, s.radius * maxScale };

		float tSphere;
		if (!rayIntersectSphere(ro, rd, worldSphere, tSphere)) continue; // reject quickly

		// Precise test: transform each triangle and test ray/triangle
		// Find nearest triangle hit (if any)
		float nearestTForMesh = FLT_MAX;
		bool hit = false;
		for (int f = 0; f < m.facesCount; ++f) {
			Face& face = m.faces[f];
			Vec4 A4 = prodMatrixVec(m.modelMatrix, m.vertices[face.indices[0]]);
			Vec4 B4 = prodMatrixVec(m.modelMatrix, m.vertices[face.indices[1]]);
			Vec4 C4 = prodMatrixVec(m.modelMatrix, m.vertices[face.indices[2]]);
			Vec3 A = { A4.x, A4.y, A4.z };
			Vec3 B = { B4.x, B4.y, B4.z };
			Vec3 C = { C4.x, C4.y, C4.z };
			float tTri;
			if (rayIntersectTriangle(ro, rd, A, B, C, tTri)) {
				if (tTri < nearestTForMesh) nearestTForMesh = tTri;
				hit = true;
			}
		}

		if (hit && nearestTForMesh < bestT) {
			bestT = nearestTForMesh;
			bestIndex = i;
		}
	}

	scene.selectedIndex = bestIndex;
	return scene.selectedIndex;
}

int pickVertexAt(int mouseX, int mouseY) {
	// Must have a selected mesh
	if (scene.selectedIndex < 0 || scene.selectedIndex >= scene.meshCount) return -1;
	Mesh& m = scene.meshes[scene.selectedIndex];
	if (m.verticesCount <= 0) return -1;

	const Mat4 mvp = prodMatrix(renderState.projMatrix, renderState.viewMatrix);
	const float W = (float)screenConfig.width;
	const float H = (float)screenConfig.height;

	const float pixelThreshold = 32.0f; // max allowed distance in pixels
	const float pixelThreshold2 = pixelThreshold * pixelThreshold;

	int bestIdx = -1;
	float bestDist2 = FLT_MAX;

	for (int i = 0; i < m.verticesCount; ++i) {
		// Transform vertex to world then to clip space
		Vec4 world4 = prodMatrixVec(m.modelMatrix, m.vertices[i]);
		Vec4 clip = prodMatrixVec(mvp, world4);

		// If behind camera or w nearly zero skip
		if (clip.w <= 1e-6f && clip.w >= -1e-6f) continue;
		if (clip.w <= 0.0f) continue;

		// Perspective divide -> NDC
		float ndcX = clip.x / clip.w;
		float ndcY = clip.y / clip.w;

		// NDC -> screen coords
		float sx = (ndcX + 1.0f) * 0.5f * W;
		float sy = (1.0f - ndcY) * 0.5f * H;

		// Calculate squared distance between vertex's pixel and mouse
		const float dx = sx - (float)mouseX;
		const float dy = sy - (float)mouseY;
		const float d2 = dx * dx + dy * dy;

		if (d2 < bestDist2) {
			bestDist2 = d2;
			bestIdx = i;
		}
	}

	if (bestIdx != -1 && bestDist2 <= pixelThreshold2) {
		scene.selectedVertexIndex = bestIdx;
		genSelectedVertexNeighbours();
		return bestIdx;
	}

	scene.selectedVertexIndex = -1;
	return -1;
}

// Builds in scene.selectedVertexNeighoubrs a list of indices adjacent to selected vertex
void genSelectedVertexNeighbours() {
	// Reset neighbours list every time
	scene.selectedVertexNeighboursCount = 0;

	// Validate selected mesh & vertex
	if (scene.selectedIndex < 0 || scene.selectedIndex >= scene.meshCount) return;
	Mesh& m = scene.meshes[scene.selectedIndex];
	int sel = scene.selectedVertexIndex;
	if (sel < 0 || sel >= m.verticesCount) return;

	// Iterate faces and collect vertices that share a face with the selected vertex.
	for (int f = 0; f < m.facesCount; ++f) {
		Face& face = m.faces[f];
		int a = face.indices[0];
		int b = face.indices[1];
		int c = face.indices[2];

		// If the face contains the selected vertex, add the other two vertices
		if (a == sel || b == sel || c == sel) {
			int candidates[2];
			int cnt = 0;
			if (a == sel) { candidates[cnt++] = b; candidates[cnt++] = c; }
			else if (b == sel) { candidates[cnt++] = a; candidates[cnt++] = c; }
			else { candidates[cnt++] = a; candidates[cnt++] = b; }

			for (int k = 0; k < cnt; ++k) {
				int idx = candidates[k];
				// Validate index and avoid duplicates
				if (idx < 0 || idx >= m.verticesCount) continue;

				bool exists = false;
				for (int n = 0; n < scene.selectedVertexNeighboursCount; ++n) {
					if (scene.selectedVertexNeighbours[n] == idx) { exists = true; break; }
				}
				if (exists) continue;

				// Store neighbour if space permits
				if (scene.selectedVertexNeighboursCount < Scene::MAX_NEIGHBOURS_COUNT) {
					scene.selectedVertexNeighbours[scene.selectedVertexNeighboursCount++] = idx;
				}
			}
		}
	}
}

void changeColorSelected(Color color) {
	Mesh* mesh = selected();
	if (mesh == nullptr) return;
	setColor(*mesh, color);
}

void createMeshInScene(const char* filePath) {
	Mesh mesh = createMesh(filePath);
	const float SPAWN_DISTANCE = 4.0f;

	const Mat4& view = renderState.viewMatrix;
	Vec3 forward = { -view.m[2][0], -view.m[2][1], -view.m[2][2] };
	forward = normalize(forward);
	Vec3 spawnPos = cameraState.camPosition + forward * SPAWN_DISTANCE;
	setTranslation(mesh, spawnPos);

	addMesh(mesh);
}

void deleteSelected() {
	int idx = scene.selectedIndex;
	if (idx == -1) return;

	int count = scene.meshCount;

	std::swap(scene.meshes[count - 1], scene.meshes[idx]);
	--scene.meshCount;
	scene.selectedIndex = -1;
	scene.selectedVertexIndex = -1;
}