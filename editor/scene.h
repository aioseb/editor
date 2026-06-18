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
	const static int MAX_NEIGHBOURS_COUNT = 256;
	int meshCount = 0;
	int selectedIndex = -1;
	int selectedVertexIndex = -1;
	int selectedVertexNeighbours[MAX_NEIGHBOURS_COUNT];
	int selectedVertexNeighboursCount = 0;
	Mesh meshes[MAX_MESH_COUNT];
};
extern Scene scene;

struct SceneMode {
	int transformationMode = 0;	// 0 to 2
	int colorMode = 0;	// 0 to 8
};
extern SceneMode sceneMode;

int addMesh(Mesh& mesh);
Mesh* getMesh(int index);
void drawScene();
void drawSelectedVertex();
Mesh* selected();
void translateSelectedBy(const Vec3& translation);
void rotateSelectedBy(const Vec3& rotation);
void scaleSelectedBy(const Vec3& scaling);
void translateVertexBy(const Vec3& translation);
Sphere computeBoundingSphere(const Mesh& mesh);
void screenToWorldRay(int mouseX, int mouseY, Vec3& outOrigin, Vec3& outDir);
bool rayIntersectSphere(const Vec3& ro, const Vec3& rd, const Sphere& s, float& outT);
bool rayIntersectTriangle(const Vec3& ro, const Vec3& rd,
	const Vec3& v0, const Vec3& v1, const Vec3& v2, float& outT);
int pickMeshAt(int mouseX, int mouseY);
int pickVertexAt(int mouseX, int mouseY);
void genSelectedVertexNeighbours();
void changeColorSelected(Color color);
void createMeshInScene(const char* filePath);
void deleteSelected();

