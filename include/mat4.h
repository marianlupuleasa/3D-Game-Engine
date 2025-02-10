#ifndef MAT4_H
#define MAT4_H

#include <vec3.h>

class Mat4 
{
public:

    float data[4][4];

    // Identity matrix constructor
    Mat4() 
	{
		data[0][0] = 1, data[0][1] = 0, data[0][2] = 0, data[0][3] = 0,
		data[1][0] = 0, data[1][1] = 1, data[1][2] = 0, data[1][3] = 0,
		data[2][0] = 0, data[2][1] = 0, data[2][2] = 1, data[2][3] = 0,
		data[3][0] = 0, data[3][1] = 0, data[3][2] = 0, data[3][3] = 1;
	}

	Mat4(float x00, float x01, float x02, float x03,
		float x10, float x11, float x12, float x13,
		float x20, float x21, float x22, float x23,
		float x30, float x31, float x32, float x33)
	{
		data[0][0] = x00, data[0][1] = x01, data[0][2] = x02, data[0][3] = x03,
		data[1][0] = x10, data[1][1] = x11, data[1][2] = x12, data[1][3] = x13,
		data[2][0] = x20, data[2][1] = x21, data[2][2] = x22, data[2][3] = x23,
		data[3][0] = x30, data[3][1] = x31, data[3][2] = x32, data[3][3] = x33;
	}

    // Matrix multiplication
    Mat4 operator*(const Mat4& other) const
	{
        Mat4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.data[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.data[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

	// Transpose matrix
	Mat4 Transpose() const 
	{
		Mat4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.data[i][j] = data[j][i];
			}
		}
		return result;
	}

	// Translation matrix
	static Mat4 Translate(float x, float y, float z)
	{
		return Mat4(1, 0, 0, x,
					0, 1, 0, y,
					0, 0, 1, z,
					0, 0, 0, 1).Transpose();
	}

	// Scale matrix
	static Mat4 Scale(float x, float y, float z) 
	{
		return Mat4(x, 0, 0, 0,
					0, y, 0, 0,
					0, 0, z, 0,
					0, 0, 0, 1);
	}

	// Rotation matrix around Z axis
	static Mat4 RotateZ(float radians) 
	{
		return Mat4(cos(radians), -sin(radians), 0, 0,
					sin(radians), cos(radians), 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1).Transpose();
	}

	// Rotation matrix around Y axis
	static Mat4 RotateY(float radians) 
	{
		return Mat4(cos(radians), 0, sin(radians), 0,
					0, 1, 0, 0,
					-sin(radians), 0, cos(radians), 0,
					0, 0, 0, 1).Transpose();
	}

	// Rotation matrix around X axis
	static Mat4 RotateX(float radians) 
	{
		return Mat4(1, 0, 0, 0,
					0, cos(radians), -sin(radians), 0,
					0, sin(radians), cos(radians), 0,
					0, 0, 0, 1).Transpose();
	}

	// Perspective projection matrix
	static Mat4 Perspective(float fov, float aspect, float near, float far) 
	{
		return Mat4(1.0f / (aspect * tan(fov / 2.0f)), 0, 0, 0,
					0, 1.0f / tan(fov / 2.0f), 0, 0,
					0, 0, (far + near) / (near - far), 2.0f * far * near / (near - far),
					0, 0, -1, 0).Transpose();
	}

	// View matrix
	static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) 
	{
		Vec3 f = (center - eye).Normalize();
		Vec3 r = f.Cross(up).Normalize();
		Vec3 u = r.Cross(f);

		return Mat4(
			r.x, u.x, -f.x, 0.0f,
			r.y, u.y, -f.y, 0.0f,
			r.z, u.z, -f.z, 0.0f,
			-r.Dot(eye), -u.Dot(eye), f.Dot(eye), 1.0f
		);
	}

	float* value_ptr() 
	{
		return &data[0][0];
	}

};

#endif
