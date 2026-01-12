#pragma once

#include <iostream>

// ------------------------------------------------------------------------------------
// ----------------------------------- STRUCTURES -------------------------------------
// ------------------------------------------------------------------------------------

struct Mat3 { float m[3][3]; };
struct Mat4 { float m[4][4]; };

struct Vec3 {
	float x, y, z;

	// Vector x Vector operations
	Vec3 operator+(Vec3 const& rhs) const {
		return { x + rhs.x, y + rhs.y, z + rhs.z };
	}

	Vec3 operator-(Vec3 const& rhs) const {
		return { x - rhs.x, y - rhs.y, z - rhs.z };
	}

	// Vector x Scalar operations
	Vec3 operator*(float const& rhs) const {
		return { x * rhs, y * rhs, z * rhs };
	}

	Vec3 operator/(float const& rhs) const {
		return { x / rhs, y / rhs, z / rhs };
	}
};

struct Vec4 {
	float x, y, z, w = 1.0f;

	// Vector x Vector operations
	Vec4 operator+(Vec4 const& rhs) const {
		return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
	}

	Vec4 operator-(Vec4 const& rhs) const {
		return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
	}

	// Vector x Scalar operations
	Vec4 operator*(float const& rhs) const {
		return { x * rhs, y * rhs, z * rhs, w * rhs };
	}

	Vec4 operator/(float const& rhs) const {
		return { x / rhs, y / rhs, z / rhs, w / rhs };
	}
};

constexpr Vec3 zeroVector = { 0.0f, 0.0f, 0.0f };
constexpr Vec3 oneVector = { 1.0f, 1.0f, 1.0f };

constexpr Vec3 worldRight = { 1.0f, 0.0f, 0.0f };
constexpr Vec3 worldUp = { 0.0f, 1.0f, 0.0f };
constexpr Vec3 worldForward = { 0.0f, 0.0f, 1.0f };

constexpr Vec4 rightPlane = { 1.0f, 0.0f, 0.0f, 1.0f };
constexpr Vec4 leftPlane = { -1.0f, 0.0f, 0.0f, 1.0f };
constexpr Vec4 topPlane = { 0.0f, 1.0f, 0.0f, 1.0f };
constexpr Vec4 bottomPlane = { 0.0f, -1.0f, 0.0f, 1.0f };
constexpr Vec4 farPlane = { 0.0f, 0.0f, 1.0f, 1.0f };
constexpr Vec4 nearPlane = { 0.0f, 0.0f, -1.0f, 1.0f };
constexpr Vec4 planes[6] = { rightPlane, leftPlane, topPlane, bottomPlane, farPlane, nearPlane };

// -------------------------------------------------------------------------------------
// --------------------------------- VECTOR OPERATIONS ---------------------------------
// -------------------------------------------------------------------------------------

float magnitude(const Vec3& v);
Vec3 normalize(const Vec3& v);
Vec3 cross(const Vec3& v, const Vec3& u);
Vec4 intersection(const Vec4& v, const Vec4& u, const Vec4& plane);
bool insidePlane(const Vec4& v, const Vec4& plane);
int clipTriangle(Vec4 clippedTriangle[6]);
float dot(const Vec3& v, const Vec3& u);
float dot(const Vec4& v, const Vec4& u);
void printVector(const Vec3& v);
void printVector(const Vec4& v);

// -------------------------------------------------------------------------------------
// --------------------------------- MATRIX OPERATIONS ---------------------------------
// -------------------------------------------------------------------------------------

Vec4 prodMatrixVec(const Mat4& mat, const Vec4& v);
Mat4 prodMatrix(const Mat4& a, const Mat4& b);
Mat4 inverseMatrix(const Mat4& mat);
Mat4 identityMatrix();	

// -------------------------------------------------------------------------------------
// ----------------------- TRANSFORMATION MATRICES (COLUMN-MAJOR)-----------------------
// -------------------------------------------------------------------------------------

Mat4 translationMatrix(const Vec3& translate = zeroVector);
Mat4 scalingMatrix(const Vec3& scale = oneVector);
Mat4 rotationMatrix_X(float angle);
Mat4 rotationMatrix_Y(float angle);
Mat4 rotationMatrix_Z(float angle);
void printMatrix(const Mat4& mat);

// -------------------------------------------------------------------------------------
// --------------------------- GRAPHICS PIPELINE MATRICES ------------------------------
// -------------------------------------------------------------------------------------

Mat4 modelMatrix(const Vec3& translate = zeroVector, const Vec3& scale = oneVector, const Vec3& rotation = zeroVector);
Mat4 lookAt(const Vec3& eye, const Vec3& target);
Mat4 viewMatrix(const Vec3& camPos, const Vec3& camAngle);
Mat4 projectionMatrix(float fovY, float aspectRatio, float front, float back);
Mat4 viewportMatrix(float x, float y, float w, float h, float n = 0.0f, float f = 1.0f);
Vec4 clipCoords(const Vec4& v, const Mat4& model, const Mat4& view, const Mat4& projection);
Vec4 toScreen(Vec4 v, const Mat4& viewport);