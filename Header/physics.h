#pragma once
#include "stdafx.h"
#include "d3dmath.h"


#define CAL_ERROR 0.000001　// 計算誤差


class PhysicalFeaturs {

public:
	bool isStatic;		// 不動かどうか. trueにすると外力（重力含む）を受けない.

	float mass;			// 重さ[kg]
	float linearDrag;	// 移動方向に対する速度減少係数（空気抵抗ではない）
	float angullarDrag;	// 回転時の抵抗
	float gravity;		// 重力[m/s^2]

	PhysicalFeaturs(float mass = 0, float linDrag = 0, float angDrag = 0, float gravity = 0, bool isStatic = false) :
		mass(mass), linearDrag(linDrag), angullarDrag(angDrag), gravity(gravity), isStatic(isStatic) {};
};


class GameObject;
class SphereCollider;
class BoxCollider;


class Collider {
public:
	enum Shape { Sphere, Box };
	float staticFriction;	// 静的摩擦係数 [0, 1]の範囲
	float dynamicFriction;	// 動的摩擦係数 [0, 1]の範囲
	float bounciness;		// 反発係数 [0, 1]の範囲

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