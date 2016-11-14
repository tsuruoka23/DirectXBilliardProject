
#include "../Header/stdafx.h"
#include "../Header/game_object.h"
#include "../Header/utility.h"

using namespace std;
UINT32 GameObject::objectId;

GameObject::GameObject() {
	id = objectId++;
	physicalFeatures = NULL;
	collider = NULL;

	velocity = { 0, 0, 0 };
	location = { 0, 0, 0 };
	extraForce = { 0, 0, 0 };
}

GameObject::~GameObject() {
	delete physicalFeatures;
	delete collider;
}



void GameObject::updatePhysics(UINT fps) {
	PhysicalFeaturs *pf = physicalFeatures;
	
	if (pf != NULL && pf->mass > 0 && !pf->isStatic) {
		// 重力、空気抵抗から物体の加速aを求め、次の速度[m/s]を決定する
		Vector3 mg = { 0, -pf->gravity * pf->mass, 0 };
		Vector3 ma = mg - (velocity * pf->linearDrag) + extraForce;
		velocity += ma / (pf->mass * fps);
		extraForce = { 0, 0, 0 };
		if (velocity.square() < CALC_ERROR) {
			velocity = { 0, 0, 0 };
		}
	}
	
	// 速度ベクトルによる移動
	setLocation(location + (velocity / fps));
}

Vector3 GameObject::getMomentum() {
	if (physicalFeatures == NULL) return{ 0, 0, 0 };
	return physicalFeatures->mass * velocity;
}

float GameObject::getKineticEnergy() {
	if (physicalFeatures == NULL) return 0;
	return physicalFeatures->mass * velocity.square() / 2;
}

void GameObject::Collision(GameObject * obj) {
	detectCollision(*this, *obj);
}

// コライダーが設定されたオブジェクト同士の衝突を検知した場合trueを返す
bool GameObject::detectCollision(GameObject & obj1, GameObject & obj2) {
	Collider *c1 = obj1.collider;
	Collider *c2 = obj2.collider;
	if (c1 == NULL || c2 == NULL || !obj1.enable || !obj2.enable) { return false; };

	if (c1->getShape() == Collider::Sphere && c2->getShape() == Collider::Sphere) {
		detectCollision_Sphere_Sphere(obj1, obj2);
	}

	else if (c1->getShape() == Collider::Box && c2->getShape() == Collider::Box) {
		detectCollision_Box_Box(obj1, obj2);
	}
	else if (c1->getShape() == Collider::Sphere && c2->getShape() == Collider::Box) {
		detectCollision_Sphere_Box(obj1, obj2);

	}
	return false;
}

bool GameObject::detectCollision_Sphere_Sphere(GameObject & obj1, GameObject & obj2) {
	SphereCollider *sc1 = (SphereCollider*)obj1.collider;
	SphereCollider *sc2 = (SphereCollider*)obj2.collider;
	float sqrD = (obj1.location - obj2.location).lengthSq();	// オブジェクト間の距離の2乗
	float sqrR = std::pow((sc1->radius + sc2->radius), 2);		// 2つの球体の半径の合計の2乗
	if (sqrD > sqrR) return false;

	if (sqrD <= sqrR - CALC_ERROR) {	// オブジェクト同士がめり込んでる
		Vector3 v = (obj1.velocity - obj2.velocity);
		float vv = v.square();
		if (vv > 0) {
			float pv = Vector3::dot((obj2.location - obj1.location), v);
			float t = (-pv + std::sqrt((pv * pv) - vv * (sqrD - sqrR))) / vv;
			obj1.setLocation(obj1.location - (t * obj1.velocity));
			obj2.setLocation(obj2.location - (t * obj2.velocity));

			//printf("<めり込み %d : %d>\n", obj1.getId(), obj2.getId());
			//cout << " location : " << obj1.location.toString() << ", " << obj2.location.toString() << endl;
			//printf(" d1 = %f, d' = %f, r = %f, t = %f\n", sqrD, (obj1.location - obj2.location).lengthSq(), sqrR, t);
		}
		else {
			return true;
		}
	}
	
	//printf("Collision  id = %d, %d\n", obj1.getId(), obj2.getId());
	float m1 = obj1.physicalFeatures->mass;	// obj1の質量 
	float m2 = obj2.physicalFeatures->mass;	// obj2の質量 
	float e = obj1.collider->restitution * obj2.collider->restitution;
	Vector3 v1 = obj1.velocity;	// obj1の衝突前速度
	Vector3 v2 = obj2.velocity;	// obj2の衝突前速度
	Vector3 n = Vector3::makeUnitVector(obj1.location, obj2.location);	// 衝突面の法線ベクトル
	Vector3 v12 = Vector3::dot(obj1.velocity, n) * n;		// v1のうち衝突面に垂直な方向成分
	Vector3 v21 = Vector3::dot(obj2.velocity, -n) * (-n);	// v2のうち衝突面に垂直な方向成分
	
	if (Vector3::dot(v12 - v21, n) <= 0) {
		return true;	// obj1のu方向の相対速度が負ならば2物体は離れていく
	}

	Vector3 v12next = ((v21 * (1 + e) * m2) + (v12 * (m1 - (e * m2)))) / (m1 + m2);
	Vector3 v21next = ((v12 * (1 + e) * m1) + (v21 * (m2 - (e * m1)))) / (m1 + m2);

	UINT32 fps = 60;
	Vector3 f1 = (v12next - v12) * obj1.physicalFeatures->mass * fps;
	Vector3 f2 = (v21next - v21) * obj2.physicalFeatures->mass * fps;
	Vector3 tmp1 = obj1.velocity;
	Vector3 tmp2 = obj2.velocity;
	obj1.setVelocity(obj1.velocity + v12next - v12);
	obj2.setVelocity(obj2.velocity + v21next - v21);
	//printf("<衝突 %d : %d>\n", obj1.getId(), obj2.getId());
	//cout << " u  : " << n.toString() << endl;
	//cout << " V1 : " << tmp1.toString() << " → " << obj1.velocity.toString() << "  force : " << f1.toString() << endl;
	//cout << " V2 : " << tmp2.toString() << " → " << obj2.velocity.toString() << "  force : " << f2.toString() << "\n" << endl;

	return true;
}

bool GameObject::detectCollision_Box_Box(GameObject & obj1, GameObject & obj2) {
	return false;
}

bool GameObject::detectCollision_Sphere_Box(GameObject &sphere, GameObject &box) {
	return false;
}



// ------------------------------------------------------------------------------------ MeshObject

MeshObject::MeshObject(ID3DXMesh * mesh, D3DMATERIAL9 material) {
	this->mesh = mesh;
	this->materials = NULL;
	materialNum = 0;

	setMaterial(material);
}

MeshObject::MeshObject(IDirect3DDevice9 *d3dDevice, char *fname) {
	this->mesh = NULL;
	this->materials = NULL;
	materialNum = 0;

	loadFromX(d3dDevice, fname);
}

MeshObject::~MeshObject(){
	delete[] materials;
}

void MeshObject::release() {
}

void MeshObject::update(UINT fps) {
	updateCount++;
	if (!enable)return;
	updatePhysics(fps);
}

void MeshObject::draw(IDirect3DDevice9* d3dDevice) const {
	if (!enable || !visible)return;
	D3DXMATRIX rx, ry, rz, translate;
	D3DXMatrixRotationX(&rx, rotation.x);
	D3DXMatrixRotationY(&ry, rotation.y);
	D3DXMatrixRotationZ(&rz, rotation.z);
	D3DXMatrixTranslation(&translate, location.x, location.y, location.z);
	D3DXMATRIX transform = rx * ry * rz * translate;

	d3dDevice->SetFVF(MeshObject::FVF);
	d3dDevice->SetTransform(D3DTS_WORLD, &transform);

	if (materialNum <= 0) {
		d3dDevice->SetMaterial(&DEFAULT_MATERIAL);
		mesh->DrawSubset(0);
	}
	else {
		for (DWORD i = 0; i < materialNum; i++) {
			d3dDevice->SetMaterial(&(materials[i].MatD3D));
			mesh->DrawSubset(i);
		};
	}
}

bool MeshObject::loadFromX(IDirect3DDevice9 *d3dDevice, char *fname){

	ID3DXBuffer *matBuffer;
	if (FAILED(D3DXLoadMeshFromX(_T(fname), D3DXMESH_MANAGED, d3dDevice, NULL, &matBuffer, NULL, &materialNum, &mesh))) {
		MessageBox(NULL, _T(fname), "Xファイル読み込みエラー", MB_OK);
		return false;
	}
	else { 
		printf("num materia = %2d\n", materialNum); 
	}

	if (materialNum <= 0) {
		MessageBox(NULL, "marialNum <= 0", "ERROR", MB_OK);
		return false;
	}

	materials = new D3DXMATERIAL[materialNum];
	for (DWORD i = 0; i < materialNum; i++) {
		materials[i] = ((D3DXMATERIAL*)matBuffer->GetBufferPointer())[i];
	}

	return true;
}

void MeshObject::setMaterial(D3DMATERIAL9 material, unsigned matNumber){
	
	if (materials == NULL && matNumber == 0) {
		materialNum = 1;
		materials = new D3DXMATERIAL[1];
	}
	else if (materialNum <= matNumber) {
		return;
	}

	materials[matNumber].MatD3D = material;
}

void MeshObject::setDiffuse(D3DCOLORVALUE diffuse, unsigned matNumber) {
	if (materials == NULL) setMaterial(DEFAULT_MATERIAL);
	if (materialNum > matNumber) {
		materials[matNumber].MatD3D.Diffuse = diffuse;
	}
}

void MeshObject::setAmbient(D3DCOLORVALUE ambient, unsigned matNumber) {
	if (materials == NULL) setMaterial(DEFAULT_MATERIAL);
	if (materialNum > matNumber) {
		materials[matNumber].MatD3D.Ambient = ambient;
	}
}

void MeshObject::setSpecular(D3DCOLORVALUE specular, float power, unsigned matNumber) {
	if (materials == NULL) setMaterial(DEFAULT_MATERIAL);
	if (materialNum > matNumber) {
		materials[matNumber].MatD3D.Specular = specular;
		materials[matNumber].MatD3D.Power = power;
	}
}

void MeshObject::setEmissive(D3DCOLORVALUE emissive, unsigned matNumber) {
	if (materials == NULL) setMaterial(DEFAULT_MATERIAL);
	if (materialNum > matNumber) {
		materials[matNumber].MatD3D.Emissive = emissive;
	}
}







SpriteObject::SpriteObject(IDirect3DDevice9 *d3dD3vice, char *fname) {
	D3DXCreateSprite(d3dD3vice, &sprite);
	if (FAILED(D3DXCreateTextureFromFile(d3dD3vice, fname, &texture))) {
		MessageBox(NULL, fname, "Texture Load Error", MB_OK);
	}
}

SpriteObject::~SpriteObject() {
}

void SpriteObject::release() {
}

void SpriteObject::update(UINT fps) {
	updateCount++;
}

void SpriteObject::draw(IDirect3DDevice9 *) const {

	if (sprite == NULL || texture == NULL) return;
	
	D3DXMATRIX matWorld;
	D3DXMATRIX transformMatrix;
	D3DXMatrixRotationZ(&matWorld, (FLOAT)(PI * updateCount) / 120);
	D3DXMatrixTranslation(&transformMatrix, 400.0f, 200.0f, 0.0f);
	transformMatrix = matWorld * transformMatrix;
	sprite->SetTransform(&transformMatrix);
	
	sprite->Begin(0);
	sprite->Draw(texture, NULL, NULL, NULL, 0xffffffff);
	sprite->End();
}

