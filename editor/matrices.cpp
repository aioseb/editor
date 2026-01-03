// This file implements a series of matrix and vector operations
// needed for a software rasterizer. 
// 
// All math follows OpenGL conventions,
// but without using any OpenGL libraries.
// 
// This file is for educational purposes. 
// Expect many bugs and questionable decisions :)
// 
// Conventions inspired from OpenGL include:
// - Column-major matrix layout
// - Post multiplication of column vectors (v' = M * v)
// - Right-handed system (-Z is the forward vector
// - Visible points are in NDC in the range [-1, 1]
// 
// Resources used:
// https://www.songho.ca/opengl/index.html
// https://learnopengl.com/
// https://www.opengl-tutorial.org/

#include "pch.h";

// Return magnitude of a vector
float magnitude(const Vec3& v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Return normalized vector
Vec3 normalize(const Vec3& v) {
	float mag = magnitude(v);
	Vec3 result = v;

	if (mag != 0) {
		result.x /= mag;
		result.y /= mag;
		result.z /= mag;
	}
	else {
		return { 0.0f, 0.0f, -1.0f };
	}

	return result;
}

// Return cross product of two 3D vectors
Vec3 cross(const Vec3& v, const Vec3& u) {
	float x = v.y * u.z - v.z * u.y;
	float y = v.z * u.x - v.x * u.z;
	float z = v.x * u.y - v.y * u.x;

	return { x, y, z };
}

// Returns the intersection between a line and a plane in 3D space
// Plane's vector must be normalized (aside from W, which is distance)
Vec4 intersection(const Vec4& v, const Vec4& u, const Vec4& plane) {
	float firstDistance = plane.x * v.x + plane.y * v.y + plane.z * v.z - plane.w * v.w;
	float secondDistance = plane.x * u.x + plane.y * u.y + plane.z * u.z - plane.w * u.w;

	return ((v * secondDistance) - (u * firstDistance)) / (secondDistance - firstDistance);
}

// Check if a vertex is inside a plane
bool insidePlane(const Vec4& v, const Vec4& plane) {
	return plane.x * v.x + plane.y * v.y + plane.z * v.z - plane.w * v.w <= 0;
}

// Clips given triangle, stores vertices in the given array and returns the number of available triangles
// Sutherland-Hodgman algorithm
int clipTriangle(Vec4 clippedTriangle[6]) {
	int count = 3, updatedCount = 0;
	Vec4 inputList[6];

	for (int planeIndex = 0; planeIndex < 6; ++planeIndex) {
		if (count <= 0) {
			return 0;
		}

		const Vec4& plane = planes[planeIndex];
		updatedCount = 0;

		for (int index = 0; index < count; index++) {
			inputList[index] = clippedTriangle[index];
		}
		
		for (int vertexIndex = 0; vertexIndex < count; ++vertexIndex) {
			Vec4 currentPoint = inputList[vertexIndex];
			Vec4 prevPoint = inputList[(vertexIndex + count - 1) % count];

			// Compute intersection only when needed
			bool currentInside = insidePlane(currentPoint, plane);
			bool prevInside = insidePlane(prevPoint, plane);

			if (updatedCount < 6) {
				if (currentInside) {
					if (!prevInside) {
						clippedTriangle[updatedCount++] = intersection(prevPoint, currentPoint, plane);
					}
					if (updatedCount < 6) {
						clippedTriangle[updatedCount++] = currentPoint;
					}
				}
				else if (prevInside) {
					clippedTriangle[updatedCount++] = intersection(prevPoint, currentPoint, plane);
				}
			}
		}

		count = updatedCount;
	}

	return count;
}

// Returns the dot product of two 3D vectors
float dot(const Vec3& v, const Vec3& u) {
	float x = v.x * u.x;
	float y = v.y * u.y;
	float z = v.z * u.z;

	return x + y + z;
}

// Returns the dot product of two 4D vectors
float dot(const Vec4& v, const Vec4& u) {
	float x = v.x * u.x;
	float y = v.y * u.y;
	float z = v.z * u.z;
	float w = v.w * u.w;

	return x + y + z + w;
}

// Prints the given 3D vector
void printVector(const Vec3& v) {
	std::cout << v.x << ' ' << v.y << ' ' << v.z << '\n';
}

// Prints the given 4D vector
void printVector(const Vec4& v) {
	std::cout << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << '\n';
}

// -------------------------------------------------------------------------------------
// --------------------------------- MATRIX OPERATIONS ---------------------------------
// -------------------------------------------------------------------------------------

// Calculate the product between a 4D Matrix and a 4D Vector
Vec4 prodMatrixVec(const Mat4& mat, const Vec4& v) {
	return {
		v.x * mat.m[0][0] + v.y * mat.m[0][1] + v.z * mat.m[0][2] + v.w * mat.m[0][3],
		v.x * mat.m[1][0] + v.y * mat.m[1][1] + v.z * mat.m[1][2] + v.w * mat.m[1][3],
		v.x * mat.m[2][0] + v.y * mat.m[2][1] + v.z * mat.m[2][2] + v.w * mat.m[2][3],
		v.x * mat.m[3][0] + v.y * mat.m[3][1] + v.z * mat.m[3][2] + v.w * mat.m[3][3],
	};
}

// Multiply two 4D Matrices
Mat4 prodMatrix(const Mat4& a, const Mat4& b) {
	Mat4 c;

	for (int i = 0; i < 4; i++) {
		float d0 = a.m[i][0];
		float d1 = a.m[i][1];
		float d2 = a.m[i][2];
		float d3 = a.m[i][3];

		c.m[i][0] = d0 * b.m[0][0] + d1 * b.m[1][0] + d2 * b.m[2][0] + d3 * b.m[3][0];
		c.m[i][1] = d0 * b.m[0][1] + d1 * b.m[1][1] + d2 * b.m[2][1] + d3 * b.m[3][1];
		c.m[i][2] = d0 * b.m[0][2] + d1 * b.m[1][2] + d2 * b.m[2][2] + d3 * b.m[3][2];
		c.m[i][3] = d0 * b.m[0][3] + d1 * b.m[1][3] + d2 * b.m[2][3] + d3 * b.m[3][3];
	}

	return c;
}

// Generate 4D identity matrix
Mat4 identityMatrix() {
	Mat4 mat;

	mat.m[0][0] = 1.0f, mat.m[0][1] = 0.0f, mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = 1.0f, mat.m[1][2] = 0.0f, mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = 1.0f, mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f, mat.m[3][2] = 0.0f, mat.m[3][3] = 1.0f;

	return mat;
}

// -------------------------------------------------------------------------------------
// ----------------------- TRANSFORMATION MATRICES (COLUMN-MAJOR)-----------------------
// -------------------------------------------------------------------------------------

// Generate translation matrix
Mat4 translationMatrix(const Vec3& translate) {
	Mat4 mat = identityMatrix();

	mat.m[0][3] = translate.x;
	mat.m[1][3] = translate.y;
	mat.m[2][3] = translate.z;
	mat.m[3][3] = 1;

	return mat;
}

// Generate scaling matrix
Mat4 scalingMatrix(const Vec3& scale) {
	Mat4 mat = identityMatrix();

	mat.m[0][0] = scale.x;
	mat.m[1][1] = scale.y;
	mat.m[2][2] = scale.z;
	mat.m[3][3] = 1;

	return mat;
}

// Rotation matrix around X-axis
Mat4 rotationMatrix_X(float angle) {
	Mat4 mat = identityMatrix();

	float s = sin(angle);
	float c = cos(angle);

	mat.m[1][1] = c;
	mat.m[1][2] = -s;
	mat.m[2][1] = s;
	mat.m[2][2] = c;

	return mat;
}

// Rotation matrix around Y-axis
Mat4 rotationMatrix_Y(float angle) {
	Mat4 mat = identityMatrix();

	float s = sin(angle);
	float c = cos(angle);

	mat.m[0][0] = c;
	mat.m[0][2] = s;
	mat.m[2][0] = -s;
	mat.m[2][2] = c;

	return mat;
}

// Rotation matrix around Z-axis
Mat4 rotationMatrix_Z(float angle) {
	Mat4 mat = identityMatrix();

	float s = sin(angle);
	float c = cos(angle);

	mat.m[0][0] = c;
	mat.m[0][1] = -s;
	mat.m[1][0] = s;
	mat.m[1][1] = c;

	return mat;
}

// Prints the given matrix
void printMatrix(const Mat4& mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << mat.m[i][j] << ' ';
		}
		std::cout << '\n';
	}
}

// -------------------------------------------------------------------------------------
// --------------------------- GRAPHICS PIPELINE MATRICES ------------------------------
// -------------------------------------------------------------------------------------

// Generate model matrix by given parameters
Mat4 modelMatrix(const Vec3& translate, const Vec3& scale, const Vec3& rotation) {
	float sx = scale.x, sy = scale.y, sz = scale.z;

	float cx = cos(rotation.x), sx_ = sin(rotation.x);
	float cy = cos(rotation.y), sy_ = sin(rotation.y);
	float cz = cos(rotation.z), sz_ = sin(rotation.z);

	// Rotation R = Rz * Ry * Rx
	// Components r_ij (row i, col j)
	float r00 = cy * cz;
	float r01 = -cy * sz_;
	float r02 = sy_;
	float r10 = sx_ * sy_ * cz + cx * sz_;
	float r11 = -sx_ * sy_ * sz_ + cx * cz;
	float r12 = -sx_ * cy;
	float r20 = -cx * sy_ * cz + sx_ * sz_;
	float r21 = cx * sy_ * sz_ + sx_ * cz;
	float r22 = cx * cy;

	// Multiply R by scale (R * diag(sx,sy,sz)): scale columns of R
	float m00 = r00 * sx;  float m01 = r01 * sy;  float m02 = r02 * sz;
	float m10 = r10 * sx;  float m11 = r11 * sy;  float m12 = r12 * sz;
	float m20 = r20 * sx;  float m21 = r21 * sy;  float m22 = r22 * sz;

	// Translation column is just translation (since M = T * (R*S))
	float tx = translate.x;
	float ty = translate.y;
	float tz = translate.z;

	Mat4 M = {}; // zero-init
	M.m[0][0] = m00; M.m[0][1] = m01; M.m[0][2] = m02; M.m[0][3] = tx;
	M.m[1][0] = m10; M.m[1][1] = m11; M.m[1][2] = m12; M.m[1][3] = ty;
	M.m[2][0] = m20; M.m[2][1] = m21; M.m[2][2] = m22; M.m[2][3] = tz;
	M.m[3][0] = 0.0f; M.m[3][1] = 0.0f; M.m[3][2] = 0.0f; M.m[3][3] = 1.0f;

	return M;

	// Legacy code:
	//Mat4 translation = translationMatrix(translate);
	//Mat4 scaling = scalingMatrix(scale);
	//Mat4 rotationX = rotationMatrix_X(rotation.x);
	//Mat4 rotationY = rotationMatrix_Y(rotation.y);
	//Mat4 rotationZ = rotationMatrix_Z(rotation.z);
	//model = prodMatrix(translation, prodMatrix(rotationZ, prodMatrix(rotationY, prodMatrix(rotationX, scaling))));
}

// Return look at matrix
Mat4 lookAt(const Vec3& eye, const Vec3& target) {
	Vec3 forward = normalize(eye - target);
	Vec3 left = normalize(cross(worldUp, forward));
	Vec3 up = cross(forward, left);

	// Build camera direction matrix
	Mat4 cameraDirection = identityMatrix();
	cameraDirection.m[0][0] = left.x;
	cameraDirection.m[0][1] = left.y;
	cameraDirection.m[0][2] = left.z;

	cameraDirection.m[1][0] = up.x;
	cameraDirection.m[1][1] = up.y;
	cameraDirection.m[1][2] = up.z;

	cameraDirection.m[2][0] = forward.x;
	cameraDirection.m[2][1] = forward.y;
	cameraDirection.m[2][2] = forward.z;

	// Build translation part
	cameraDirection.m[0][3] = -left.x * eye.x - left.y * eye.y - left.z * eye.z;
	cameraDirection.m[1][3] = -up.x * eye.x - up.y * eye.y - up.z * eye.z;
	cameraDirection.m[2][3] = -forward.x * eye.x - forward.y * eye.y - forward.z * eye.z;

	return cameraDirection;
}

// Return view matrix
Mat4 viewMatrix(const Vec3& camPos, const Vec3& camAngle)
{
	float sx = sin(camAngle.x), cx = cos(camAngle.x);
	float sy = sin(-camAngle.y), cy = cos(-camAngle.y);
	float sz = sin(camAngle.z), cz = cos(camAngle.z);

	// Rotation submatrix (R = Rx * Ry(-) * Rz)
	float r00 = cy * cz;
	float r01 = cy * -sz;
	float r02 = sy;

	float r10 = sx * sy * cz + cx * sz;
	float r11 = -sx * sy * sz + cx * cz;
	float r12 = -sx * cy;

	float r20 = -cx * sy * cz + sx * sz;
	float r21 = cx * sy * sz + sx * cz;
	float r22 = cx * cy;

	// Translation column = -R * pos
	float tx = -(r00 * camPos.x + r01 * camPos.y + r02 * camPos.z);
	float ty = -(r10 * camPos.x + r11 * camPos.y + r12 * camPos.z);
	float tz = -(r20 * camPos.x + r21 * camPos.y + r22 * camPos.z);

	Mat4 M = identityMatrix();

	M.m[0][0] = r00;  M.m[0][1] = r01;  M.m[0][2] = r02;  M.m[0][3] = tx;
	M.m[1][0] = r10;  M.m[1][1] = r11;  M.m[1][2] = r12;  M.m[1][3] = ty;
	M.m[2][0] = r20;  M.m[2][1] = r21;  M.m[2][2] = r22;  M.m[2][3] = tz;
	M.m[3][0] = 0.0f; M.m[3][1] = 0.0f; M.m[3][2] = 0.0f; M.m[3][3] = 1.0f;

	// Legacy code:
	// Mat4 mat = translationMatrix(camPos * -1);
	// mat = prodMatrix(rotationMatrix_Z(camAngle.z), mat);
	// mat = prodMatrix(rotationMatrix_Y(-camAngle.y), mat);
	// mat = prodMatrix(rotationMatrix_X(camAngle.x), mat);

	return M;
}

// Return perspective projection matrix
Mat4 projectionMatrix(float fovY, float aspectRatio, float front, float back) {
	const float DEG2RAD = acos(-1.0f) / 180;

	float tangent = tan(fovY / 2 * DEG2RAD);
	float top = front * tangent;
	float right = top * aspectRatio;

	Mat4 mat = identityMatrix();
	mat.m[0][0] = front / right;
	mat.m[1][1] = front / top;
	mat.m[2][2] = -(back + front) / (back - front);
	mat.m[2][3] = -(2.0f * back * front) / (back - front);
	mat.m[3][2] = -1.0f;
	mat.m[3][3] = 0.0f;

	return mat;
}

// Generate view port matrix
Mat4 viewportMatrix(float x, float y, float w, float h, float n, float f) {
	Mat4 mat = identityMatrix();

	mat.m[0][0] = w * 0.5;
	mat.m[1][1] = -h * 0.5;
	mat.m[2][2] = (f - n) * 0.5;
	mat.m[0][3] = x + w * 0.5;
	mat.m[1][3] = y + h * 0.5;
	mat.m[2][3] = (f + n) * 0.5;

	return mat;
}

// Calculate clip coordinates of a vector
Vec4 clipCoords(const Vec4& v, const Mat4& model, const Mat4& view, const Mat4& projection) {
	Mat4 mvp = prodMatrix(projection, prodMatrix(view, model));
	return prodMatrixVec(mvp, v);
}

// Calculate screen coordinates
Vec4 toScreen(Vec4 v, const Mat4& viewport) {
	// check for NaN / Inf early
	if (std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z) || std::isnan(v.w) ||
		std::isinf(v.x) || std::isinf(v.y) || std::isinf(v.z) || std::isinf(v.w)) {
		v.x = v.y = -100000.0f; v.w = 1.0f; // send off-screen safely
		return prodMatrixVec(viewport, v);
	}

	if (fabs(v.w) > 1e-6f) {   // safe threshold
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
		v.w = 1.0f;
	}
	else {
		// point at or behind the camera, send off-screen
		v.x = v.y = -100000.0f;
		v.z = 0.0f;
		v.w = 1.0f;
		return prodMatrixVec(viewport, v);
	}

	// NDC -> screen
	return prodMatrixVec(viewport, v);
}