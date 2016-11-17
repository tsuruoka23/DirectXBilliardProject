#pragma once
#include "stdafx.h"

struct Vector3: public D3DXVECTOR3 {
	Vector3() : D3DXVECTOR3() {};
	Vector3(CONST FLOAT *f) : D3DXVECTOR3(f) {};
	Vector3(CONST D3DVECTOR& v) : D3DXVECTOR3(v) {};
	Vector3(CONST D3DXFLOAT16 *f) : D3DXVECTOR3(f) {};
	Vector3(FLOAT x, FLOAT y, FLOAT z) : D3DXVECTOR3(x, y, z) {};

	// unary operators
	Vector3 operator + () const;
	Vector3 operator - () const;

	// binary operators
	Vector3 operator + (const Vector3&) const;
	Vector3 operator - (const Vector3&) const;
	Vector3 operator * (float) const;
	Vector3 operator / (float) const;

	Vector3& operator *= (const D3DXMATRIX& m);
	Vector3 operator * (const D3DXMATRIX& m) const;

	static Vector3 makeUnitVector(const Vector3 &p1, const Vector3 &p2);
	static float dot(const Vector3 &v1, const Vector3 &v2);
	static Vector3 cross(const Vector3 &v1, const Vector3 &v2);
	static Vector3 normalize(const Vector3 &v);
	
	void normalize();
	Vector3 getNorm() const;
	float getMax() const;
	float getMin() const;

	float length() const;
	float lengthSq() const;
	float square() const;

	std::string toString() const;
	const char* toC_Str() const;
};


void showMatrix(D3DXMATRIX & m);
D3DXVECTOR3 multiple(D3DXVECTOR3 v, D3DXMATRIX m);