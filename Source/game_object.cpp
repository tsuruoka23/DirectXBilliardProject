
#include "../Header/stdafx.h"
#include "../Header/game_object.h"

using namespace std;
UINT32 GameObject::objectId;

GameObject::GameObject() {
	id = objectId++;
	physicalFeatures = NULL;
	collider = NULL;

	velocity = { 0, 0, 0 };
	location = { 0, 0, 0 };
	extraForce = { 0, 0, 0 };
	D3DXMatrixIdentity(&rotationMatrix);
}

GameObject::~GameObject() {}

void GameObject::updatePhysics(UINT fps) {
	PhysicalFeaturs *pf = physicalFeatures;

	//cout << "update> l = " << location.toString() << ", v = " << velocity.toString() << " : " << id << endl;
	if (pf != NULL && pf->mass > 0 && !pf->isStatic) {
		// オブジェクトにかかる外力と現在の速度から次フレームの速度[m/s]を決定する
		velocity += extraForce / (pf->mass * fps);

		if (velocity.square() < CALC_ERROR * 128) {
			velocity = { 0, 0, 0 };
		}
		else if(collider != NULL && collider->getShape() == Collider::Sphere){	// 球を適当に回転させる
			Vector3 axis = { velocity.z, 0, -velocity.x };	// 進行方向（y方向は無視）に垂直なベクトル
			if (axis.lengthSq() > CALC_ERROR) {
				Matrix r;
				SphereCollider* sc = (SphereCollider*)collider;
				float w = velocity.length() / (sc->radius * fps);
				D3DXMatrixRotationAxis(&r, &axis, w);
				rotationMatrix *= r;
			}
		}
		Vector3 mg = { 0, -pf->gravity * pf->mass, 0 };
		extraForce = mg -(velocity.getNorm() * pf->linearDrag);	// 常にかかる力（重力と疑似空間摩擦）
	}
	else {
		extraForce = { 0, 0, 0 };
	}

	// 速度ベクトルによる移動
	setLocation(location + (velocity / (float)fps));
	alreadyCollision = false;
	//cout << "update> l = " << location.toString() << ", v = " << velocity.toString() << " : " << id << endl;
}

// 速度、回転、外力を0にする
void GameObject::resetPhysics() {
	velocity = {0, 0, 0};
	extraForce = {0, 0, 0};
	D3DXMatrixIdentity(&rotationMatrix);
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
		//obj1.alreadyCollision || obj2.alreadyCollision ||
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
	
	/* めり込み処理。挙動がおかしくなるため現在コメントアウト中
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
	*/

	calcVelocity(obj1, obj2, (obj2.location - obj1.location).getNorm() );
	return true;
}

bool GameObject::detectCollision_Box_Box(GameObject & obj1, GameObject & obj2) {
	return false;
}


bool GameObject::detectCollision_Sphere_Box(GameObject &sphere, GameObject &box) {
	SphereCollider *sc = (SphereCollider*)sphere.collider;
	BoxCollider *bc = (BoxCollider*)box.collider;
	Vector3 distance3(0, 0, 0);		// xyz方向のBoxからの距離
	Vector3 direct3[3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };	// 各面の法線ベクトル（今回はBoxの回転無視）
	float len3[3] = { bc->width / 2, bc->height / 2, bc->depth / 2};

	// とりあえず回転無視でSphereからBoxへのxyz距離計算
	for (int i = 0; i < 3; i++) {
		float bsi = Vector3::dot(sphere.location - box.location, direct3[i]);	// Box→Sphereベクトルの方向iへの射影
		float di = std::abs(bsi) - len3[i];		// i方向におけるBoxからの距離
		
		if (bsi < 0) { direct3[i] = -direct3[i]; }
		if (di > 0) { distance3[i] = di; }
	}

	float distance = distance3.length();
	float max = distance3.getMax();
	Vector3 n = (max == distance3.x) ? direct3[0] : (max == distance3.y) ? direct3[1] : direct3[2];

	// オブジェクト同士が接しているとき、抗力を与える
	if (distance <= sc->radius + CALC_ERROR) {
		float f1 = Vector3::dot(box.extraForce, n);
		float f2 = Vector3::dot(sphere.extraForce, -n);
		//cout << "抗力: " << f1 << " , " << f2 << " f = " << sphere.extraForce.toString() << endl;
		if (f1 > 0) {
			Vector3 fn1 = f1 * n;
			sphere.addForce(fn1);
			box.addForce(-fn1);
		}
		if (f2 > 0) {
			Vector3 fn2 = f2 * (-n);
			sphere.addForce(-fn2);
			box.addForce(fn2);
		}
	}
	else if (distance > sc->radius + CALC_ERROR) return false;

	// オブジェクト同士がめり込んでいるとき、接触した瞬間まで戻す
	if (distance < sc->radius - CALC_ERROR) {
		Vector3 vn1 = box.velocity;
		Vector3 vn2 = sphere.velocity;
		Vector3 vn12 = vn1 - vn2;
		//cout << "1 - v21 = " << vn21.toString() << endl;
		//cout << "めり込み - location = " << sphere.getLocation().toString() << endl;
		if (Vector3::dot(vn12, n) > 0) {
			float t = (sc->radius - distance) / vn12.length();
			box.setLocation(box.location - (t * box.velocity));	
			sphere.setLocation(sphere.location - (t * sphere.velocity));
		}
		//cout << "めり込み - location = " << sphere.getLocation().toString() << endl;
	}
	
	calcVelocity(box, sphere, n);

	return false;
}

// n12 = 衝突面の法線ベクトル
void GameObject::calcVelocity(GameObject & obj1, GameObject & obj2, const Vector3 &n12) {

	float e = obj1.collider->bounciness * obj2.collider->bounciness;	// 反発度. とりあえず掛け算
	float m1 = obj1.physicalFeatures->mass;						// obj1の質量 
	float m2 = obj2.physicalFeatures->mass;						// obj2の質量 
	Vector3 vn12 = Vector3::dot(obj1.velocity, n12) * n12;		// v1のうち衝突面に垂直な方向成分
	Vector3 vn21 = Vector3::dot(obj2.velocity, n12) * n12;		// v2のうち衝突面に垂直な方向成分

	if (Vector3::dot(vn12 - vn21, n12) <= 0) {
		return;	// obj1のn方向の相対速度が負ならば2物体は離れていく
	}

	if (obj1.physicalFeatures->isStatic) {
		obj2.setVelocity(obj2.velocity - (1 + e) * vn21);
	}
	else if (obj2.physicalFeatures->isStatic) {
		obj1.setVelocity(obj1.velocity - (1 + e) * vn12);
	}
	else {
		obj1.setVelocity(obj1.velocity + ((vn21 - vn12) * (1 + e) * m2 / (m1 + m2)));
		obj2.setVelocity(obj2.velocity + ((vn12 - vn21) * (1 + e) * m1 / (m1 + m2)));
	}
}




// ------------------------------------------------------------------------------------ MeshObject

MeshObject::MeshObject(ID3DXMesh * mesh, D3DMATERIAL9 material) {
	this->mesh = mesh;
	this->materials = NULL;
	this->texture = NULL;
	this->materialNum = 0;
	setMaterial(material);
}

MeshObject::MeshObject(IDirect3DDevice9 *d3dDevice, char *fname) {
	this->mesh = NULL;
	this->materials = NULL;
	this->texture = NULL;
	this->materialNum = 0;

	loadFromX(d3dDevice, fname);
}

MeshObject::~MeshObject(){
	delete[] materials;
}

// 全てのオブジェクトを解放するのでmeshなど他で使いまわしているときは注意
void MeshObject::release() {
	SAFE_RELEASE(mesh);
	SAFE_RELEASE(texture);
	SAFE_DELETE(physicalFeatures);
	SAFE_DELETE(collider);
}

void MeshObject::update(UINT fps) {
	updateCount++;
	if (!enable)return;
	updatePhysics(fps);
}

void MeshObject::draw(IDirect3DDevice9* d3dDevice) const {
	if (!enable || !visible)return;
	D3DXMATRIX transform, translate, r;
	D3DXMatrixTranslation(&translate, location.x, location.y, location.z);
	transform = rotationMatrix * translate;

	d3dDevice->SetFVF(MeshObject::FVF);
	d3dDevice->SetTransform(D3DTS_WORLD, &transform);

	if (materialNum <= 0) {	// マテリアルがセットされていないとき
		d3dDevice->SetMaterial(&DEFAULT_MATERIAL);
		mesh->DrawSubset(0);
	}
	else {	// マテリアル、テクスチャが複数ある場合にも対応
		for (DWORD i = 0; i < materialNum; i++) {
			d3dDevice->SetMaterial(&(materials[i].MatD3D));
			d3dDevice->SetTexture(0, texture);
			mesh->DrawSubset(0);
		};
	}
}

void MeshObject::releaseMesh() {
	SAFE_RELEASE(mesh);
}

void MeshObject::releaseTexture() {
	SAFE_RELEASE(texture);
}

bool MeshObject::loadFromX(IDirect3DDevice9 *d3dDevice, char *fname){
	ID3DXBuffer *matBuffer;
	if (FAILED(D3DXLoadMeshFromX(_T(fname), D3DXMESH_MANAGED, d3dDevice, NULL, &matBuffer, NULL, &materialNum, &mesh))) {
		MessageBox(NULL, _T(fname), "Xファイル読み込みエラー", MB_OK);
		return false;
	}
	else {  printf("num materia = %2d\n", materialNum); }

	if (materialNum <= 0) {
		MessageBox(NULL, "marialNum <= 0", "ERROR", MB_OK);
		return false;
	}
	printf("FVF = %04x,  %04x\n", mesh->GetFVF(), MeshObject::FVF);

	cout << fname << ": " << materialNum << endl;
	materials = new D3DXMATERIAL[materialNum];
	for (DWORD i = 0; i < materialNum; i++) {
		materials[i] = ((D3DXMATERIAL*)matBuffer->GetBufferPointer())[i];
	}
	return true;
}

void MeshObject::loadTexture(IDirect3DDevice9 *d3dDevice, const char * fname) {
	if (FAILED(D3DXCreateTextureFromFile(d3dDevice, fname, &texture))) {
		MessageBox(NULL, fname, "Texture Load Error", MB_OK);
	}
	//texture->GetLevelDesc(0, &surface);
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





SpriteObject::SpriteObject(IDirect3DDevice9 *d3dDevice, char *fname) {
	D3DXCreateSprite(d3dDevice, &sprite);
	if (FAILED(D3DXCreateTextureFromFile(d3dDevice, fname, &texture))) {
		MessageBox(NULL, fname, _T("Texture Load Error"), MB_OK);
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

