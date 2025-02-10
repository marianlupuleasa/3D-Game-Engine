#ifndef VEC3_H
#define VEC3_H

// Mesh class
class Vec3 
{
public:
    
    float x, y, z;

	// Default Constructor (0, 0, 0)
	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

	// Constructor (x, y, z)
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	// Vector operations 
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

	// Dot product of two vectors
    float Dot(const Vec3& other) const 
    { 
        return x * other.x + y * other.y + z * other.z; 
    }

	// Cross product of two vectors
    Vec3 Cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

	// Magnitude of vector
	float Magnitude() const 
    { 
        return sqrt(x * x + y * y + z * z); 
    }

	// Normalize vector
	Vec3 Normalize() const 
    {
		float mag = Magnitude();
		return Vec3(x / mag, y / mag, z / mag);
	}
     
	// Clamp between min and max values
    Vec3 Clamp(float minValue, float maxValue) 
    {
        x = (x < minValue) ? minValue : (x > maxValue) ? maxValue : x;
        y = (y < minValue) ? minValue : (y > maxValue) ? maxValue : y;
        z = (z < minValue) ? minValue : (z > maxValue) ? maxValue : z;
        return Vec3(x, y, z);
    }

	// Distance between two vectors
    float Distance(const Vec3& other) const 
    {
        return sqrt((x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y) +
            (z - other.z) * (z - other.z));
    }

    // Pointer for Uniforms sent to Shaders
    float* value_ptr() 
    {
        return &x;
    }



};

#endif
