#pragma once

#include "pch.h"
#include "mesh.h"
#include "state.h"
#include "config.h"
#include "matrices.h"
#include "render_view.h"

struct Sphere {
	Vec3 center;
	float radius;
};

struct Scene {
	const static int MAX_MESH_COUNT = 64;
	int meshCount = 0;
	int selectedIndex = -1;
	Mesh meshes[MAX_MESH_COUNT];
};

int addMesh(Scene& scene, Mesh& mesh);
Mesh* getMesh(Scene& scene, int index);
void drawScene(const Scene& scene);
Mesh* selected(Scene& scene);
Sphere computeBoundingSphere(const Mesh& mesh);
void screenToWorldRay(Scene& scene, int mouseX, int mouseY, Vec3& outOrigin, Vec3& outDir);
bool rayIntersectSphere(const Vec3& ro, const Vec3& rd, const Sphere& s, float& outT);
bool rayIntersectTriangle(const Vec3& ro, const Vec3& rd,
	const Vec3& v0, const Vec3& v1, const Vec3& v2, float& outT);
int pickMeshAt(Scene& scene, int mouseX, int mouseY);
