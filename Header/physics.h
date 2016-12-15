#pragma once
#include "stdafx.h"
#include "d3dmath.h"


#define CAL_ERROR 0.000001@// ŒvZŒë·


class PhysicalFeaturs {

public:
	bool isStatic;		// •s“®‚©‚Ç‚¤‚©. true‚É‚·‚é‚ÆŠO—Íid—ÍŠÜ‚Şj‚ğó‚¯‚È‚¢.

	float mass;			// d‚³[kg]
	float linearDrag;	// ˆÚ“®•ûŒü‚É‘Î‚·‚é‘¬“xŒ¸­ŒW”i‹ó‹C’ïR‚Å‚Í‚È‚¢j
	float angullarDrag;	// ‰ñ“]‚Ì’ïR
	float gravity;		// d—Í[m/s^2]

	PhysicalFeaturs(float mass = 0, float linDrag = 0, float angDrag = 0, float gravity = 0, bool isStatic = false) :
		mass(mass), linearDrag(linDrag), angullarDrag(angDrag), gravity(gravity), isStatic(isStatic) {};
};


class GameObject;
class SphereCollider;
class BoxCollider;


class Collider {
public:
	enum Shape { Sphere, Box };
	float staticFriction;	// Ã“I–€CŒW” [0, 1]‚Ì”ÍˆÍ
	float dynamicFriction;	// “®“I–€CŒW” [0, 1]‚Ì”ÍˆÍ
	float bounciness;		// ”½”­ŒW” [0, 1]‚Ì”ÍˆÍ

	Collider(float sFriction = 0, float dFriction = 0, float bounciness = 0) :
		staticFriction(sFriction), dynamicFriction(dFriction), bounciness(bounciness){};

	virtual Shape getShape() = 0;
};


class BoxCollider : public Collider{
	
public:
	float width;
	float height;
	float depth;

	BoxCollider(float width, float height, float depth, float sFriction = 0, float dFriction = 0, float bounciness = 0) : 
		width(width), height(height), depth(depth), Collider(sFriction, dFriction, bounciness) {
	};

	virtual Shape getShape() { return Shape::Box; };
};


class SphereCollider : public Collider {
public:
	float radius;

	SphereCollider(float radius, float sFriction = 0, float dFriction = 0, float bounciness = 0) :
		radius(radius), Collider(sFriction, dFriction, bounciness) {
	};

	virtual Shape getShape() { return Shape::Sphere; };
};