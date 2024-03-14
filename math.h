#pragma once
#include <iostream>


double inline __declspec (naked) __fastcall FastSQRT(double n)
{
	_asm fld qword ptr[esp + 4]
	_asm fsqrt
	_asm ret 8
}

class Matrix4x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float m[4][4];
		float mm[16];
	};

	inline void Transpose()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m[i][j] = m[j][i];
			}
		}
	}

	/*inline Vector3& GetAxis(int i)
	{
		return *(Vector3*)&m[i][0];
	}*/
};

class Matrix3x4
{
public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
		};

		float m[3][4];
		float mm[12];
	};

	/*inline Vector3& GetAxis(int i)
	{
		return *(Vector3*)&m[i][0];
	}*/
};

class Vector3
{
public:
	float x, y, z;
	
	// Constructor/Destructor
	
	Vector3(float xPos, float yPos, float zPos) :
		x(xPos), y(yPos), z(zPos)
	{
	}

	Vector3()
	{
		x = y = z = 0.0f;
	}

	~Vector3()
	{
	}

	// Operator overloading
	
	inline Vector3& operator=(const Vector3& v)
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}

	inline Vector3& operator=(const float* v)
	{
		x = v[0]; y = v[1]; z = v[2]; return *this;
	}

	inline float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	inline float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	inline Vector3& operator+=(const Vector3& v)
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	inline Vector3& operator-=(const Vector3& v)
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	inline Vector3& operator*=(const Vector3& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	inline Vector3& operator/=(const Vector3& v)
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	inline Vector3& operator+=(float v)
	{
		x += v; y += v; z += v; return *this;
	}

	inline Vector3& operator-=(float v)
	{
		x -= v; y -= v; z -= v; return *this;
	}

	inline Vector3& operator*=(float v)
	{
		x *= v; y *= v; z *= v; return *this;
	}

	inline Vector3& operator/=(float v)
	{
		x /= v; y /= v; z /= v; return *this;
	}

	inline Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	inline Vector3 operator+(const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	inline Vector3 operator-(const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline Vector3 operator*(const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	inline Vector3 operator/(const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	inline Vector3 operator+(float v) const
	{
		return Vector3(x + v, y + v, z + v);
	}

	inline Vector3 operator-(float v) const
	{
		return Vector3(x - v, y - v, z - v);
	}

	inline Vector3 operator*(float v) const
	{
		return Vector3(x * v, y * v, z * v);
	}

	inline Vector3 operator/(float v) const
	{
		return Vector3(x / v, y / v, z / v);
	}

	inline bool IsZero() const
	{
		return (x > -0.01f && x < 0.01f
			&&	y > -0.01f && y < 0.01f
			&&	z > -0.01f && z < 0.01f);
	}

	// Functions

	float GetX() { return x; }
	float GetY() { return y; }
	float GetZ() { return z; }

	void Print()
	{
		//std::cout << x << ' ' << y << ' ' << z << "\n";
	};

	float DotProduct(Vector3 &v1, float* v2)
	{
		return v1.x*v2[0] + v1.y*v2[1] + v1.z*v2[2];
	}

	float Dot(const Vector3 &v1, Vector3 &v2)
	{
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}

	float Length(Vector3& vec)
	{
		return FastSQRT(vec.x *  vec.x + vec.y *  vec.y + vec.z *  vec.z);
	}

	float Distance(Vector3& fVec1, Vector3& fVec2)
	{
		return FastSQRT(pow(fVec1.x - fVec2.x, 2) + pow(fVec1.y - fVec2.y, 2) + pow(fVec1.z - fVec2.z, 2));
	}

	void Normalize(Vector3& v)
	{
		float l = Length(v);
		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			v.x = v.y = 0.0f; v.z = 1.0f;
		}
	}

};