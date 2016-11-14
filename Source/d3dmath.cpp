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
	Vector3 vec = v / v.length();
	return vec; 
}

void Vector3::normalize() {
	float len = length();
	x /= len; y /= len; z /= len;
}

float Vector3::length() const { return std::sqrt(lengthSq()); }
float Vector3::lengthSq() const { return (x * x) + (y * y) + (z * z); }
float Vector3::square() const { return lengthSq(); }

std::string Vector3::toString() {
	std::stringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ")";
	return ss.str();
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