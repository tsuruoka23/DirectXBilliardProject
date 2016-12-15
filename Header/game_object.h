#pragma once
#include "stdafx.h"
#include "physics.h"
#include "d3dmath.h"

class GameObject {
public:

protected:
	char* tag;
	bool enable = true;
	bool visible = true;

	PhysicalFeaturs *physicalFeatures;
	Collider *collider;
	Vector3 velocity;	// 速度ベクトル[m/s]
	Vector3 location;	// 位置ベクトル[メートル]
	//Vector3 grotation;	// 回転ベクトル[度]
	Vector3 extraForce;	// オブジェクト内で解決できない外からの力.[kg*m/s^2]
	Matrix rotationMatrix;	// 回転行列
	bool alreadyCollision;	// フレーム間で既に衝突計算が行われている場合true. updatePhyisicsでfalseになる

	static UINT32 objectId;
	UINT32 id;
	UINT32 updateCount;	// updateが呼び出された回数

public: 

	GameObject();
	virtual ~GameObject();

	void setTag(char* tag) { this->tag = tag; }
	void setEnable(bool is) { enable = is; }
	void setVisible(bool is) { visible = is; }
	char* getTag() { return tag; }
	bool getEnable() { return enable; }
	bool getVisible() { return visible; }

	UINT32 getId() { return id; }
	bool getAlreadyCollision() { return alreadyCollision; }
	void resetAlreayCollision() { alreadyCollision = false; }

	virtual void release() = 0;
	virtual void update(UINT fps) = 0;
	virtual void draw(IDirect3DDevice9*) const = 0;

	void setVelocity(FLOAT vx, FLOAT vy, FLOAT vz) { setVelocity(Vector3{ vx, vy, vz }); };
	void setVelocity(Vector3 v) { velocity = v; };
	void setLocation(FLOAT x, FLOAT y, FLOAT z) { setLocation(Vector3{ x, y, z }); };
	void setLocation(Vector3 l) { location = l; };
	void setRotation(FLOAT x, FLOAT y, FLOAT z) { setRotation(Vector3{ x, y, z }); };
	void setRotation(Vector3 r) { setRotationMatrix(Matrix::makeRotationMatrixXYZ(r)); };
	void setRotationMatrix(Matrix m) { rotationMatrix = m; };
	Vector3 getVelocity() const { return velocity; }
	Vector3 getLocation() const { return location; }
	Matrix getRocationMatrix() const { return rotationMatrix; }
	void resetPhysics();	// 速度、回転、外力を0にする
 
	void setPhysicalFeatures(float mass = 0, float linDrag = 0, float angDrag = 0, float gravity = 0, bool isStatic = false) {
		setPhysicalFeatures(new PhysicalFeaturs(mass, linDrag, angDrag, gravity, isStatic));
	};
	void setBoxCollider(float width, float height, float depth, float sFriction = 0, float dFriction = 0, float bounciness = 0) {
		setCollider(new BoxCollider(width, height, depth, sFriction, dFriction, bounciness));
	}
	void setSphereCollider(float radius, float sFriction = 0, float dFriction = 0, float bounciness = 0) {
		setCollider(new SphereCollider(radius, sFriction, dFriction, bounciness));
	}
	
	void setPhysicalFeatures(PhysicalFeaturs *pf) { physicalFeatures = pf; }
	void setCollider(Collider *c) { collider = c; }
	PhysicalFeaturs* getPhysicalFeaturs() { return physicalFeatures; }
	Collider* getCollider() { return collider; }
	void deletePhysicalFeatures() { SAFE_DELETE(physicalFeatures); }
	void deleteCollider() { SAFE_DELETE(collider); }

	void addForce(FLOAT x, FLOAT y, FLOAT z) { addForce(Vector3{ x, y, z }); }
	void addForce(Vector3 f) { if(physicalFeatures != NULL && !physicalFeatures->isStatic)extraForce += f; }
	Vector3 getMomentum();
	float getKineticEnergy();

	void Collision(GameObject *obj);

	static bool detectCollision(GameObject &obj1, GameObject &obj2);

protected:
	void updatePhysics(UINT fps);

	static bool detectCollision_Sphere_Sphere(GameObject &obj1, GameObject &obj2);
	static bool detectCollision_Box_Box(GameObject &obj1, GameObject &obj2);
	static bool detectCollision_Sphere_Box(GameObject &sphere, GameObject &box);
	static void calcVelocity(GameObject &obj1, GameObject &obj2, const Vector3 &n12);
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
	IDirect3DTexture9 *texture;
	DWORD materialNum;
	const D3DMATERIAL9 DEFAULT_MATERIAL = {};

public:

	struct  MyVertex {
		Vector3 position;
		Vector3 n;
		float tu, tv;
	};
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

public:

	MeshObject(ID3DXMesh * mesh, D3DMATERIAL9 material = {0});
	MeshObject(IDirect3DDevice9*, char*);
	~MeshObject();

	void release();	// 全てのオブジェクトを解放するのでmeshなど他で使いまわしているときは注意
	void update(UINT fps);
	void draw(IDirect3DDevice9*)const;

	void releaseMesh();
	void releaseTexture();

	bool loadFromX(IDirect3DDevice9* , char*);
	void loadTexture(IDirect3DDevice9 *d3dDevice, const char * fname);
	void setMesh(ID3DXMesh * m) { this->mesh = m; };
	//ID3DXMesh* getMesh() const { return this->mesh; }

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
