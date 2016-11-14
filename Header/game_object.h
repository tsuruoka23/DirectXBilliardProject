#pragma once
#include "stdafx.h"
#include "physics.h"
#include "d3dmath.h"

class GameObject {
protected:
	PhysicalFeaturs *physicalFeatures;
	Collider *collider;
	Vector3 velocity;	// 速度ベクトル[m/s]
	Vector3 location;	// 位置ベクトル[メートル]
	Vector3 rotation;	// 回転ベクトル[度]
	Vector3 extraForce;	// オブジェクト内で解決できない外からの力．重力や空気抵抗などは含まない．[kg*m/s^2]

	static UINT32 objectId;
	UINT32 id;
	UINT32 updateCount;	// updateが呼び出された回数

	void updatePhysics(UINT fps);
	//bool detectCollision(GameObject *obj);
	//void collide();

	static bool detectCollision(GameObject &obj1, GameObject &obj2);
	static bool detectCollision_Sphere_Sphere(GameObject &obj1, GameObject &obj2);
	static bool detectCollision_Box_Box(GameObject &obj1, GameObject &obj2);
	static bool detectCollision_Sphere_Box(GameObject &sphere, GameObject &box);

public:
	bool enable = true;
	bool visible = true;

	GameObject();
	~GameObject();

	UINT32 getId() { return id; }

	virtual void release() = 0;
	virtual void update(UINT fps) = 0;
	virtual void draw(IDirect3DDevice9*) const = 0;

	void setVelocity(FLOAT vx, FLOAT vy, FLOAT vz) { setVelocity(Vector3{ vx, vy, vz }); };
	void setVelocity(Vector3 v) { velocity = v; };
	void setLocation(FLOAT x, FLOAT y, FLOAT z) { setLocation(Vector3{ x, y, z }); };
	void setLocation(Vector3 l) { location = l; };
	void setRotation(FLOAT x, FLOAT y, FLOAT z) { setRotation(Vector3{ x, y, z }); };
	void setRotation(Vector3 r) { rotation = r; };
	Vector3 getVelocity() const { return velocity; }
	Vector3 getLocation() const { return location; }
	Vector3 getRocation() const { return rotation; }
	
 
	void setPhysicalFeatures(PhysicalFeaturs *pf) { physicalFeatures = pf; };
	void setCollider(Collider *c) { collider = c; }
	void addForce(FLOAT x, FLOAT y, FLOAT z) { addForce(Vector3{ x, y, z }); }
	void addForce(Vector3 f) { extraForce += f; }
	Vector3 getMomentum();
	float getKineticEnergy();

	void Collision(GameObject *obj);

};



class MeshObject : public GameObject{
private :
	struct VERTEX {
		float x, y, z;
		DWORD color;
		float u, v;
	};

	ID3DXMesh *mesh;
	D3DXMATERIAL *materials;
	DWORD materialNum;

	const D3DMATERIAL9 DEFAULT_MATERIAL = {};

public:
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

private:


public:

	MeshObject(ID3DXMesh * mesh, D3DMATERIAL9 material = {});
	MeshObject(IDirect3DDevice9*, char*);
	~MeshObject();


	void release();
	void update(UINT fps);
	void draw(IDirect3DDevice9*)const;

	bool loadFromX(IDirect3DDevice9* , char*);

	void setMesh(ID3DXMesh * m) { this->mesh = m; };

	void setMaterial(D3DMATERIAL9 material, unsigned maretialNumber = 0);
	void setDiffuse(D3DCOLORVALUE diffuse, unsigned matNumber = 0);
	void setAmbient(D3DCOLORVALUE Ambient, unsigned matNumber = 0);
	void setSpecular(D3DCOLORVALUE Specular, float power, unsigned matNumber = 0);
	void setEmissive(D3DCOLORVALUE Emissive, unsigned matNumber = 0);

};


class SpriteObject: public GameObject {
private:

	ID3DXSprite		  *sprite = NULL;
	IDirect3DTexture9 *texture = NULL;

public:
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

private:


public:

	SpriteObject(IDirect3DDevice9*, char*);
	~SpriteObject();


	void release();
	void update(UINT fps);
	void draw(IDirect3DDevice9*)const;

	void onLostDevice() { sprite->OnLostDevice(); }
	void onResetDevice() { sprite->OnResetDevice(); }


};
