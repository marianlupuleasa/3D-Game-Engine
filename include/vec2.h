#ifndef VEC2_H
#define VEC2_H

// Mesh class
class Vec2 
{
public:
    
    float x, y;

	// Default Constructor (0, 0)
	Vec2() : x(0.0f), y(0.0f) {}

	// Constructor (x, y)
    Vec2(float x, float y) : x(x), y(y) {}

	// Vector operations 
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }

	// Magnitude of vector
	float Magnitude() const 
    { 
        return sqrt(x * x + y * y); 
    }

	// Normalize vector
	Vec2 Normalize() const 
    {
		float mag = Magnitude();
		return Vec2(x / mag, y / mag);
	}
     
	// Clamp between min and max values
    Vec2 Clamp(float minValue, float maxValue) 
    {
        x = (x < minValue) ? minValue : (x > maxValue) ? maxValue : x;
        y = (y < minValue) ? minValue : (y > maxValue) ? maxValue : y;
        return Vec2(x, y);
    }

	// Distance between two vectors
    float Distance(const Vec2& other) const 
    {
        return sqrt((x - other.x) * (x - other.x) +
            (y - other.y) * (y - other.y));
    }

};

#endif
