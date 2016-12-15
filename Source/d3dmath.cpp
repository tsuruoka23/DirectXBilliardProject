#include "../Header/stdafx.h"
#include "../Header/d3dmath.h"

Vector3 Vector3::operator+() const {
	return *this;
}
Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}
Vector3 Vector3::operator+(const Vector3 &v) const {
	return Vector3(x + v.x, y + v.y, z + v.z);
}
Vector3 Vector3::operator - (const Vector3&v) const {
	return Vector3(x - v.x, y - v.y, z - v.z);
}
Vector3 Vector3::operator * (float s) const {
	return Vector3(x * s, y * s, z * s);
}
Vector3 Vector3::operator / (float s) const {
	return Vector3(x / s, y / s, z / s);
}

Vector3 & Vector3::operator*=(const D3DXMATRIX & m) {
	float tx = x, ty = y, tz = z;
	x = m._14 + tx * m._11 + ty * m._21 + tz * m._31;
	y = m._24 + tx * m._12 + ty * m._22 + tz * m._32;
	z = m._34 + tx * m._13 + ty * m._23 + tz * m._33;
	return *this;
}

Vector3 Vector3::operator*(const D3DXMATRIX & m) const {
	Vector3 v;
	v.x = m._14 + (x * m._11) + (y * m._21) + (z * m._31);
	v.y = m._24 + (x * m._12) + (y * m._22) + (z * m._32);
	v.z = m._34 + (x * m._13) + (y * m._23) + (z * m._33);
	return v;
}

// 点p1からp2方向の単位ベクトルを生成する
Vector3 Vector3::makeUnitVector(const Vector3 &p1, const Vector3 &p2) {
	Vector3 v = p2 - p1;
	v.normalize();
	return v;
}

float Vector3::dot(const Vector3 &v1, const Vector3 &v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

Vector3 Vector3::cross(const Vector3 &v1, const Vector3 &v2) {
	return{ (v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x) };
}

Vector3 Vector3::normalize(const Vector3 & v) {
	float len = v.length();
	if (len == 0) return v;
	return v / len; 
}

void Vector3::normalize() {
	float len = length();
	if (len == 0) return;
	x /= len; y /= len; z /= len;
}

Vector3 Vector3::getNorm() const{
	return Vector3::normalize(*this);
}

float Vector3::getMax() const {
	return max(x, max(y, z));
}

float Vector3::getMin() const {
	return min(x, min(y, z));
}

float Vector3::length() const { return std::sqrt(lengthSq()); }
float Vector3::lengthSq() const { return (x * x) + (y * y) + (z * z); }
float Vector3::square() const { return lengthSq(); }

std::string Vector3::toString() const{
	std::stringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ")";
	return ss.str();
}

const char* Vector3::toC_Str() const {
	return toString().c_str();
}



// Matrix -------------------------------------------------------------------------------

Matrix Matrix::IdentityMatrix() {
	return Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

// 左からx y z軸回転行列をかけていく
Matrix Matrix::makeRotationMatrixXYZ(const float x, const float y, const float z) {
	return makeRotationMatrixXYZ({ x, y, z });
}

// 左からx y z軸回転行列をかけていく
Matrix Matrix::makeRotationMatrixXYZ(const Vector3 & rotation) {
	D3DXMATRIX rx, ry, rz;
	D3DXMatrixRotationX(&rx, rotation.x);
	D3DXMatrixRotationY(&ry, rotation.y);
	D3DXMatrixRotationZ(&rz, rotation.z);
	return rx * ry * rz;
}

void Matrix::show() {
	showMatrix(*this);
}



void showMatrix(D3DXMATRIX & m) {
	printf("matrix\n"
		"%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n",
		m._11, m._12, m._13, m._14, m._21, m._22, m._23, m._24, m._31, m._32, m._33, m._34, m._41, m._42, m._43, m._44);
};

D3DXVECTOR3 multiple(D3DXVECTOR3 v, D3DXMATRIX m) {
	D3DXVECTOR3 t = v;
	v.x = m._14 + t.x * m._11 + t.y * m._21 + t.z * m._31;
	v.y = m._24 + t.x * m._12 + t.y * m._22 + t.z * m._32;
	v.z = m._34 + t.x * m._13 + t.y * m._23 + t.z * m._33;
	return v;
}