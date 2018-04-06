#pragma once

#include "stdafx.h"

#define DOUBLE_MAX_DIFFERENCE 0.00001


// Class of a vector with the starting point be the origin in 2D cartesian plane.
// Used to calculate transformations and/or something else.
// Currently unused. Hahahahah.
typedef class _VECTOR2D
{
public:
	_VECTOR2D(double, double) noexcept;
	double GetX() noexcept;
	double GetY() noexcept;
	void SetX(double) noexcept;
	void AddX(double) noexcept;
	void SetY(double) noexcept;
	void AddY(double) noexcept;

	_VECTOR2D operator+(const _VECTOR2D&) noexcept;
	_VECTOR2D operator-(const _VECTOR2D&) noexcept;
	_VECTOR2D operator*(const double&) noexcept;
	double operator*(const _VECTOR2D&) noexcept;
	void operator=(const _VECTOR2D&) noexcept;
	void operator+=(const _VECTOR2D&) noexcept;
	void operator-=(const _VECTOR2D&) noexcept;
	bool operator==(const _VECTOR2D&) noexcept;
	bool operator!=(const _VECTOR2D&) noexcept;

private:
	double x = 0.0;
	double y = 0.0;
}VECTOR2D, *PVECTOR2D;

// Class of a position in 2D cartesian plane.
// Used by the CONTROLLER class to store joystick positions (and can be used to do some other things).
typedef class _POSITION
{
public:
	void	SetPosPolar(float angle, float radius) noexcept;					// Set position with polar coordinate.
	void	SetPosCartesian(float x, float y) noexcept;							// Set position with cartesian coordinate.
	float	GetXPos() noexcept;													// Get the X position.
	float	GetYPos() noexcept;													// Get the Y position.
	float	GetRadius() noexcept;												// Get the radius.
	float	GetAngle() noexcept;												// Get the angle.

private:
	float	fTheta;
	float	fX;
	float	fY;
	float	fR;
}POSITION, *PPOSITION;





#pragma region Definition

// Constructor, initializes endpoint position.
inline _VECTOR2D::_VECTOR2D(double X = 0, double Y = 0) noexcept
{
	x = X;
	y = Y;
}

// Returns X coordinate.
inline double _VECTOR2D::GetX() noexcept
{
	return x;
}

// Returns Y coordinate.
inline double _VECTOR2D::GetY() noexcept
{
	return y;
}

// Set the X coordinate.
// Parameter(s):
//     X (const double): the coordinate to be set.
inline void _VECTOR2D::SetX(const double X) noexcept
{
	x = X;
}

// Add a value to the X coordinate.
// Parameter(s):
//     X (const double): the value to be add.
inline void _VECTOR2D::AddX(const double X) noexcept
{
	x += X;
}

// Set the Y coordinate.
// Parameter(s):
//     Y (const double): the coordinate to be set.
inline void _VECTOR2D::SetY(const double Y) noexcept
{
	y = Y;
}

// Add a value to the X coordinate.
// Parameter(s):
//     X (const double): the value to be add.
inline void _VECTOR2D::AddY(const double Y) noexcept
{
	y += Y;
}

// Defines the addition of vectors.
inline _VECTOR2D _VECTOR2D::operator+(const _VECTOR2D& vec) noexcept
{
	VECTOR2D v(this->x + vec.x, this->y + vec.y);

	return v;
}

// Defines the substraction of vectors.
inline _VECTOR2D _VECTOR2D::operator-(const _VECTOR2D& vec) noexcept
{
	VECTOR2D v(this->x - vec.x, this->y - vec.y);

	return v;
}

// Defines the scalar multiplication of vectors.
inline _VECTOR2D _VECTOR2D::operator*(const double& d) noexcept
{
	VECTOR2D v(this->x * d, this->y * d);

	return v;
}

// Defines the dot multiplication of vectors.
inline double _VECTOR2D::operator*(const _VECTOR2D& vec) noexcept
{
	return this->x * vec.x + this->y * vec.y;
}

// Defines the assigning operation.
inline void _VECTOR2D::operator=(const _VECTOR2D& vec) noexcept
{
	x = vec.x;
	y = vec.y;
}

// Defines the add-by operation.
inline void _VECTOR2D::operator+=(const _VECTOR2D& vec) noexcept
{
	x += vec.x;
	y += vec.y;
}

// Defines the substract-by operation.
inline void _VECTOR2D::operator-=(const _VECTOR2D& vec) noexcept
{
	x -= vec.x;
	y -= vec.y;
}

// Defines the equal comparison of two vectors.
inline bool _VECTOR2D::operator==(const _VECTOR2D& vec) noexcept
{
	if (abs(this->x - vec.x) <= DOUBLE_MAX_DIFFERENCE && abs(this->y - vec.y) <= DOUBLE_MAX_DIFFERENCE)
		return true;

	return false;
}

// Defines the inequal comparison of two vectors.
inline bool _VECTOR2D::operator!=(const _VECTOR2D& vec) noexcept
{
	if (abs(this->x - vec.x) <= DOUBLE_MAX_DIFFERENCE && abs(this->y - vec.y) <= DOUBLE_MAX_DIFFERENCE)
		return false;

	return true;
}

// Set the position with polar coordinate.
inline void _POSITION::SetPosPolar(const float angle, const float radius) noexcept
{
	fTheta = angle;
	fR = radius;

	fX = radius * cos(angle);
	fY = radius * sin(angle);
}

// Set the position with Cartesian coordinate.
inline void _POSITION::SetPosCartesian(const float x, const float y) noexcept
{
	fX = x;
	fY = y;

	fR = sqrt(x * x + y * y);

	if (x != 0.0f && y != 0.0f)
	{
		fTheta = atan(y / x);
	}
	else if (y == 0.0f)
	{
		fTheta = 0.0f;
	}
	else
	{
		fTheta = PI / 2;
	}
}

// Returns the X coordinate.
inline float _POSITION::GetXPos() noexcept
{
	return fX;
}

// Returns the Y coordinate.
inline float _POSITION::GetYPos() noexcept
{
	return fY;
}

// Returns theta value in polar plane.
inline float _POSITION::GetAngle() noexcept
{
	return fTheta;
}

// Returns radius value in polar plane.
inline float _POSITION::GetRadius() noexcept
{
	return fR;
}

#pragma endregion What is going on here?


