#include "mesh.h"

// --------------------------------------------------------------------------
// ----------------------- MESH INITIALIZATIONS -----------------------------
// --------------------------------------------------------------------------

Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount) {
	Mesh mesh;
	
	mesh.vertices = new Vec4[verticesCount];
	mesh.verticesCount = verticesCount;

	mesh.faces = new Face[facesCount];
	mesh.facesCount = facesCount;
	
	mesh.translation = zeroVector;
	mesh.rotation = zeroVector;
	mesh.scaling = oneVector;

	for (int i = 0; i < verticesCount; i++) {
		mesh.vertices[i] = vertices[i];
	}

	for (int i = 0; i < facesCount; i++) {
		mesh.faces[i].indices[0] = faces[i].indices[0];
		mesh.faces[i].indices[1] = faces[i].indices[1];
		mesh.faces[i].indices[2] = faces[i].indices[2];
		mesh.faces[i].normal = faces[i].normal;
		mesh.faces[i].color = faces[i].color;
	}

	mesh.modelMatrix = identityMatrix();

	return mesh;
}

Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount, Vec3 translation, Vec3 rotation, Vec3 scaling) {
	Mesh mesh;

	mesh.vertices = new Vec4[verticesCount];
	mesh.verticesCount = verticesCount;

	mesh.faces = new Face[facesCount];
	mesh.facesCount = facesCount;

	for (int i = 0; i < verticesCount; i++) {
		mesh.vertices[i] = vertices[i];
	}

	for (int i = 0; i < facesCount; i++) {
		mesh.faces[i].indices[0] = faces[i].indices[0];
		mesh.faces[i].indices[1] = faces[i].indices[1];
		mesh.faces[i].indices[2] = faces[i].indices[2];
		mesh.faces[i].normal = faces[i].normal;
		mesh.faces[i].color = faces[i].color;
	}

	mesh.translation = translation;
	mesh.rotation = rotation;
	mesh.scaling = scaling;

	updateModelMatrix(mesh);

	return mesh;
}

// --------------------------------------------------------------------------
// ----------------------- MESH CUSTOMIZATIONS ------------------------------
// --------------------------------------------------------------------------
void setColor(Mesh& mesh, Color color) {
	std::swap(color.b, color.g);
	for (int i = 0; i < mesh.facesCount; i++) {
		mesh.faces[i].color = color;
	}
}

// --------------------------------------------------------------------------
// ----------------------- MESH TRANSFORMATIONS -----------------------------
// --------------------------------------------------------------------------
void setTranslation(Mesh& mesh, Vec3 translation) {
	mesh.translation = translation;
	updateModelMatrix(mesh);
}

void setRotation(Mesh& mesh, Vec3 rotation) {
	mesh.rotation = rotation;
	updateModelMatrix(mesh);
}

void setScaling(Mesh& mesh, Vec3 scaling) {
	mesh.scaling = scaling;
	updateModelMatrix(mesh);
}

void translateBy(Mesh& mesh, Vec3 translation) {
	mesh.translation = mesh.translation + translation;
	updateModelMatrix(mesh);
}

void rotateBy(Mesh& mesh, Vec3 rotation) {
	mesh.rotation = mesh.rotation + rotation;
	updateModelMatrix(mesh);
}

void scaleBy(Mesh& mesh, Vec3 scaling) {
	mesh.scaling = mesh.scaling + scaling;
	updateModelMatrix(mesh);
}

void updateModelMatrix(Mesh& mesh) {
	mesh.modelMatrix = modelMatrix(mesh.translation, mesh.scaling, mesh.rotation);
}
