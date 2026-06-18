#pragma once

#include "pch.h"
#include "number_reader.h"

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
    Vec4* vertices = NULL;
    int verticesCount = 0;

    Face* faces = NULL;
    int facesCount = 0;

    Vec3 translation;
    Vec3 rotation;
    Vec3 scaling;

    Mat4 modelMatrix = identityMatrix();

    bool failedLoading = false;

    void copyMesh(Mesh const& rhs) {
        verticesCount = rhs.verticesCount;
        facesCount = rhs.facesCount;

        vertices = new Vec4[verticesCount];
        faces = new Face[facesCount];

        for (int i = 0; i < verticesCount; i++) {
            vertices[i] = rhs.vertices[i];
        }
        for (int i = 0; i < facesCount; i++) {
            faces[i] = rhs.faces[i];
        }

        translation = rhs.translation;
        rotation = rhs.rotation;
        scaling = rhs.scaling;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                modelMatrix.m[i][j] = rhs.modelMatrix.m[i][j];
            }
        }

        failedLoading = rhs.failedLoading;
    }

    Mesh() {}
    Mesh(Mesh const& rhs) { copyMesh(rhs); }
    void operator=(Mesh const& rhs) { copyMesh(rhs); }

    ~Mesh() {
        if (vertices) { delete[] vertices; }
        if (faces) { delete[] faces; }
    }
};

// Mesh initializations
Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount);
Mesh createMesh(Vec4* vertices, int verticesCount, Face* faces, int facesCount, Vec3 translation, Vec3 rotation, Vec3 scaling);
Mesh createMesh(const char* filePath);
void recalculateFaceNormals(Mesh& mesh);

// Mesh transformations
void setTranslation(Mesh& mesh, Vec3 translation);
void setRotation(Mesh& mesh, Vec3 rotation);
void setScaling(Mesh& mesh, Vec3 scaling);

void translateBy(Mesh& mesh, Vec3 translation);
void rotateBy(Mesh& mesh, Vec3 rotation);
void scaleBy(Mesh& mesh, Vec3 scaling);

void updateModelMatrix(Mesh& mesh);

// Mesh customization
void setColor(Mesh& mesh, Color color);
