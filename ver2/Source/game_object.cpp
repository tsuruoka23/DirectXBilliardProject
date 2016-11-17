
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
		extraForce += mg - (velocity * pf->linearDrag);
		velocity += extraForce / (pf->mass * fps);
		extraForce = { 0, 0, 0 };
		if (velocity.square() < CALC_ERROR) {
			//velocity = { 0, 0, 0 };
		}
	}
	
	// 速度ベクトルによる移動
	setLocation(location + (velocity / fps));
	alreadyCollision = false;
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
	bool detected = false;
	if (c1 == NULL || c2 == NULL || !obj1.enable || !obj2.enable ||
		obj1.alreadyCollision || obj2.alreadyCollision ||
		(obj1.physicalFeatures->isStatic && obj2.physicalFeatures->isStatic)) { return false; };

	if (c1->getShape() == Collider::Sphere && c2->getShape() == Collider::Sphere) {
		detected = detectCollision_Sphere_Sphere(obj1, obj2);
	}

	else if (c1->getShape() == Collider::Box && c2->getShape() == Collider::Box) {
		detected = detectCollision_Box_Box(obj1, obj2);
	}
	else if (c1->getShape() == Collider::Sphere && c2->getShape() == Collider::Box) {
		detected = detectCollision_Sphere_Box(obj1, obj2);
	}
	if (!obj1.physicalFeatures->isStatic)obj1.alreadyCollision = detected;
	if (!obj2.physicalFeatures->isStatic)obj2.alreadyCollision = detected;

	return detected;
}

bool GameObject::detectCollision_Sphere_Sphere(GameObject & obj1, GameObject & obj2) {
	SphereCollider *sc1 = (SphereCollider*)obj1.collider;
	SphereCollider *sc2 = (SphereCollider*)obj2.collider;
	float sqrD = (obj1.location - obj2.location).lengthSq();	// オブジェクト間の距離の2乗
	float sqrR = std::pow((sc1->radius + sc2->radius), 2);		// 2つの球体の半径の合計の2乗
	
	if (sqrD > sqrR) { return false; }
	if (sqrD < sqrR) {	// オブジェクト同士がめり込んでる
		Vector3 v = (obj1.velocity - obj2.velocity);
		if (v.square() != 0) {
			float pv = Vector3::dot((obj2.location - obj1.location), v);
			float t = (-pv + std::sqrt((pv * pv) - v.square() * (sqrD - sqrR))) / v.square();
			obj1.setLocation(obj1.location - (t * obj1.velocity));	// t秒前に衝突
			obj2.setLocation(obj2.location - (t * obj2.velocity));
			printf("<めり込み %d : %d>\n", obj1.getId(), obj2.getId());
			cout << "  location : " << obj1.location.toString() << ", " << obj2.location.toString() << endl;
			printf("  d1 = %f, d' = %f, r = %f, t = %f\n", sqrD, (obj1.location - obj2.location).lengthSq(), sqrR, t);
			if (t > 0.1) printf("t!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1\n\n");
		}
		else {
			return true;
		}
		
	}

	calcVelocity(obj1, obj2, (obj2.location - obj1.location).getNorm() );
	return true;
}

bool GameObject::detectCollision_Box_Box(GameObject & obj1, GameObject & obj2) {
	return false;
}

// width: x方向, height: y方向, depth: z方向. location: 中心座標, rotation: 回転量
Vector3* toVertexListFromBox(float width, float height, float depth, const Vector3& location, const Vector3& rotation) {
	Vector3 vertexs[8];

	vertexs[0] = location + Vector3( width / 2,  height / 2,  depth / 2);
	vertexs[1] = location + Vector3( width / 2,  height / 2, -depth / 2);
	vertexs[2] = location + Vector3( width / 2, -height / 2,  depth / 2);
	vertexs[3] = location + Vector3( width / 2, -height / 2, -depth / 2);
	vertexs[4] = location + Vector3(-width / 2,  height / 2,  depth / 2);
	vertexs[5] = location + Vector3(-width / 2,  height / 2, -depth / 2);
	vertexs[6] = location + Vector3(-width / 2, -height / 2,  depth / 2);
	vertexs[7] = location + Vector3(-width / 2, -height / 2, -depth / 2);

	if (rotation.square() > 0) {
		D3DXMATRIX rx, ry, rz, rotationMatrix;
		D3DXMatrixRotationX(&rx, rotation.x);
		D3DXMatrixRotationY(&ry, rotation.y);
		D3DXMatrixRotationZ(&rz, rotation.z);
		rotationMatrix = rx * ry * rz;
		for (int i = 0; i < 8; i++) {
			vertexs[i] *= rotationMatrix;
			//cout << "vertex[" << i << "] = " << vertexs[i].toString() << endl;
		}
	}
	return vertexs;
}

bool GameObject::detectCollision_Sphere_Box(GameObject &sphere, GameObject &box) {
	SphereCollider *sc = (SphereCollider*)sphere.collider;
	BoxCollider *bc = (BoxCollider*)box.collider;
	Vector3 distance3(0, 0, 0);   // 最終的に長さを求めるベクトル
	Vector3 direct3[3] = { Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1) };
	float len3[3] = { bc->width / 2, bc->height / 2, bc->depth / 2};

	// とりあえず回転無視で距離計算
	for (int i = 0; i < 3; i++) {
		float s = std::abs(Vector3::dot(sphere.location - box.location, direct3[i])) / len3[i];
		if (s > 1) { 
			float d = (s - 1) * len3[i];
			if (d > sc->radius) return false;
			distance3 += d * direct3[i];
		}
	}

	float distance = distance3.length();
	if (distance > sc->radius) return false;

	float max = distance3.getMax();
	Vector3 n = (max == distance3.x) ? direct3[0] : (max == distance3.y) ? direct3[1] : direct3[2];
	cout << "<衝突> 球-BOX: n = " << n.toString() << ", d3 = " << distance3.toString() << endl;
	printf("max = %f, d = %f, r = %f\n", max, distance, sc->radius);
	
	
	if (distance < sc->radius) {	// オブジェクト同士がめり込んでる
		Vector3 vn1 = box.velocity;
		Vector3 vn2 = sphere.velocity;
		Vector3 vn12 = vn1 - vn2;
		if (Vector3::dot(vn12, n) > 0) {
			float t = (sc->radius - distance) / vn12.length();
			box.setLocation(box.location - (t * box.velocity));	
			sphere.setLocation(sphere.location - (t * sphere.velocity));

		}
	}
	
	calcVelocity(box, sphere, n);

	return false;
}

// n12 = 衝突面の法線ベクトル
void GameObject::calcVelocity(GameObject & obj1, GameObject & obj2, const Vector3 &n12) {


	float e = obj1.collider->restitution * obj2.collider->restitution;	// 反発度. とりあえず掛け算
	float m1 = obj1.physicalFeatures->mass;						// obj1の質量 
	float m2 = obj2.physicalFeatures->mass;						// obj2の質量 
	Vector3 v12 = Vector3::dot(obj1.velocity, n12) * n12;		// v1のうち衝突面に垂直な方向成分
	Vector3 v21 = Vector3::dot(obj2.velocity, n12) * n12;		// v2のうち衝突面に垂直な方向成分

	if (Vector3::dot(v12 - v21, n12) <= 0) {
		return;	// obj1のn方向の相対速度が負ならば2物体は離れていく
	}

	if (obj1.physicalFeatures->isStatic) {
		obj2.setVelocity( (-e) * obj2.velocity);
	}
	else if (obj2.physicalFeatures->isStatic) {
		obj1.setVelocity( (-e) * obj1.velocity);
	}
	else {
		obj1.setVelocity(obj1.velocity + ((v21 - v12) * (1 + e) * m2 / (m1 + m2)));
		obj2.setVelocity(obj2.velocity + ((v12 - v21) * (1 + e) * m1 / (m1 + m2)));
	}

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

