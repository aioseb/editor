#include "scene.h"

// Adds a mesh in the given scene and returns its index
int addMesh(Scene& scene, Mesh& mesh) {
	if (scene.meshCount >= scene.MAX_MESH_COUNT) return -1;	// Scene is full - return -1
	scene.meshes[scene.meshCount++] = mesh;
	return scene.meshCount - 1;
}

// Returns the mesh at the given index
Mesh* getMesh(Scene& scene, int index) {
	if (index < 0 || index >= scene.meshCount) return nullptr;
	return &scene.meshes[index];
}

// Draws all meshes in the scene
void drawScene(const Scene& scene) {
	for (int meshIndex = 0; meshIndex < scene.meshCount; ++meshIndex) {
		drawMesh(scene.meshes[meshIndex]);
		if (meshIndex == scene.selectedIndex) {
			drawMesh(scene.meshes[meshIndex], true, {255, 255, 0});
		}
		else {
			drawMesh(scene.meshes[meshIndex]);
		}
	}
}

// Returns the currently selected mesh
Mesh* selected(Scene& scene) {
	if (scene.selectedIndex < 0) return nullptr;
	return &scene.meshes[scene.selectedIndex];
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

		float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
		if (radius < dist2) {
			radius = dist2;
		}
	}

	return { center, sqrtf(radius) };
}

// Cast a 3D world-space ray from screen to mouse position
void screenToWorldRay(Scene& scene, int mouseX, int mouseY, Vec3& outOrigin, Vec3& outDir) {
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
	Vec3 L = { s.center.x - ro.x, s.center.y - ro.y, s.center.z - ro.z };
	float tca = L.x * rd.x + L.y * rd.y + L.z * rd.z;
	float d2 = (L.x * L.x + L.y * L.y + L.z * L.z) - tca * tca;
	float r2 = s.radius * s.radius;
	if (d2 > r2) return false;
	float thc = sqrtf(r2 - d2);
	float t0 = tca - thc;
	float t1 = tca + thc;
	if (t0 < 0 && t1 < 0) return false;
	outT = (t0 > 0) ? t0 : t1;
	return true;
}

// Möller–Trumbore ray/triangle intersection (returns hit and t)
bool rayIntersectTriangle(const Vec3& ro, const Vec3& rd,
	const Vec3& v0, const Vec3& v1, const Vec3& v2, float& outT) {
	const float EPS = 1e-6f;
	Vec3 e1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	Vec3 e2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
	Vec3 p = cross(rd, e2);
	float det = e1.x * p.x + e1.y * p.y + e1.z * p.z;
	if (fabs(det) < EPS) return false;
	float invDet = 1.0f / det;
	Vec3 t = { ro.x - v0.x, ro.y - v0.y, ro.z - v0.z };
	float u = (t.x * p.x + t.y * p.y + t.z * p.z) * invDet;
	if (u < 0.0f || u > 1.0f) return false;
	Vec3 q = cross(t, e1);
	float v = (rd.x * q.x + rd.y * q.y + rd.z * q.z) * invDet;
	if (v < 0.0f || (u + v) > 1.0f) return false;
	float tt = (e2.x * q.x + e2.y * q.y + e2.z * q.z) * invDet;
	if (tt > EPS) {
		outT = tt;
		return true;
	}
	return false;
}

// Select mesh under mouse (returns selected index or -1). If multiple hits pick nearest.
int pickMeshAt(Scene& scene, int mouseX, int mouseY) {
	Vec3 ro, rd;
	screenToWorldRay(scene, mouseX, mouseY, ro, rd);

	float bestT = FLT_MAX;
	int bestIndex = -1;

	for (int i = 0; i < scene.meshCount; ++i) {
		Mesh& m = scene.meshes[i];
		// bounding sphere in model space
		Sphere s = computeBoundingSphere(m);

		// transform sphere center by model matrix (approximate scaling by max scale)
		Vec4 centerLocal = { s.center.x, s.center.y, s.center.z, 1.0f };
		Vec4 centerWorld4 = prodMatrixVec(m.modelMatrix, centerLocal);
		Vec3 centerWorld = { centerWorld4.x, centerWorld4.y, centerWorld4.z };

		// conservative radius scaling: compute scale from modelMatrix columns (max column length)
		float sx = sqrtf(m.modelMatrix.m[0][0] * m.modelMatrix.m[0][0] + m.modelMatrix.m[0][1] * m.modelMatrix.m[0][1] + m.modelMatrix.m[0][2] * m.modelMatrix.m[0][2]);
		float sy = sqrtf(m.modelMatrix.m[1][0] * m.modelMatrix.m[1][0] + m.modelMatrix.m[1][1] * m.modelMatrix.m[1][1] + m.modelMatrix.m[1][2] * m.modelMatrix.m[1][2]);
		float sz = sqrtf(m.modelMatrix.m[2][0] * m.modelMatrix.m[2][0] + m.modelMatrix.m[2][1] * m.modelMatrix.m[2][1] + m.modelMatrix.m[2][2] * m.modelMatrix.m[2][2]);
		float maxScale = std::max({ sx, sy, sz });
		Sphere worldSphere = { centerWorld, s.radius * maxScale };

		float tSphere;
		if (!rayIntersectSphere(ro, rd, worldSphere, tSphere)) continue; // reject quickly

		// precise test: transform each triangle and test ray/triangle
		// find nearest triangle hit (if any)
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