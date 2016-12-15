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


struct Matrix: public D3DXMATRIX {
public:
	Matrix() : D3DXMATRIX(){};
	Matrix(CONST FLOAT * f) : D3DXMATRIX(f) {};
	Matrix(CONST D3DMATRIX& m) : D3DXMATRIX(m) {};
	Matrix(CONST D3DXFLOAT16 *f16) : D3DXMATRIX(f16) {};
	Matrix(FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14,
		FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
		FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
		FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44) : D3DXMATRIX(_11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44) {};

	static Matrix IdentityMatrix();
	static Matrix makeRotationMatrixXYZ(const float x, const float y, const float z);
	static Matrix makeRotationMatrixXYZ(const Vector3& rotation);

	void show();
};

void showMatrix(D3DXMATRIX & m);
D3DXVECTOR3 multiple(D3DXVECTOR3 v, D3DXMATRIX m);

