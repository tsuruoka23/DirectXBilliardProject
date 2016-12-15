#pragma once
#include "stdafx.h"
#include "d3dmath.h"


#define CAL_ERROR 0.000001�@// �v�Z�덷


class PhysicalFeaturs {

public:
	bool isStatic;		// �s�����ǂ���. true�ɂ���ƊO�́i�d�͊܂ށj���󂯂Ȃ�.

	float mass;			// �d��[kg]
	float linearDrag;	// ��C��R
	float angullarDrag;	// ��]���̋�C��R
	float gravity;		// �d��[m/s^2]

	PhysicalFeaturs(float mass = 0, float linDrag = 0, float angDrag = 0, float gravity = 0, bool isStatic = false) :
		mass(mass), linearDrag(linDrag), angullarDrag(angDrag), gravity(gravity), isStatic(isStatic) {};
};


class GameObject;
class SphereCollider;
class BoxCollider;


class Collider {
public:
	enum Shape { Sphere, Box };
	float staticFriction;	// �ÓI���C�W�� [0, 1]�͈̔�
	float dynamicFriction;	// ���I���C�W�� [0, 1]�͈̔�
	float restitution;		// �����W�� [0, 1]�͈̔�

	Collider(float sFriction = 0, float dFriction = 0, float restitution = 0) :
		staticFriction(sFriction), dynamicFriction(dFriction), restitution(restitution){};

	virtual Shape getShape() = 0;
};


class BoxCollider : public Collider{
	
public:
	float width;
	float height;
	float depth;

	BoxCollider(float width, float height, float depth, float sFriction = 0, float dFriction = 0, float restitution = 0) : 
		width(width), height(height), depth(depth), Collider(sFriction, dFriction, restitution) {
	};

	virtual Shape getShape() { return Shape::Box; };
};


class SphereCollider : public Collider {
public:
	float radius;

	SphereCollider(float radius, float sFriction = 0, float dFriction = 0, float restitution = 0) :
		radius(radius), Collider(sFriction, dFriction, restitution) {
	};

	virtual Shape getShape() { return Shape::Sphere; };
};