#pragma once

#include "pch.h"

// A mesh is described by its vertices, the way those vertices are connected
// in a triangle, and its local model matrix
//
// TODO: Find a better data structure to store vertices and indices,
// suitable for adding and removing data

struct Face {
	int indices[3];
	Vec3 normal = worldUp;
	Color color = WHITE;
};

struct Mesh {
	Vec4* vertices;
	int verticesCount;

	Face* faces;
	int facesCount;

	Vec3 translation;
	Vec3 rotation;
	Vec3 scaling;

	Mat4 modelMatrix = identityMatrix();
};

// Mesh initializations
Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount);
Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount, Vec3 translation, Vec3 rotation, Vec3 scaling);

// Mesh customization
void setColor(Mesh& mesh, Color color);

// Mesh transformations
void setTranslation(Mesh& mesh, Vec3 translation);
void setRotation(Mesh& mesh, Vec3 rotation);
void setScaling(Mesh& mesh, Vec3 scaling);

void translateBy(Mesh& mesh, Vec3 translation);
void rotateBy(Mesh& mesh, Vec3 rotation);
void scaleBy(Mesh& mesh, Vec3 scaling);

void updateModelMatrix(Mesh& mesh);