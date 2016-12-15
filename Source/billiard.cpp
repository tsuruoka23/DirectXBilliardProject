<<<<<<< HEAD
#include "../Header/stdafx.h"
#include "../Header/billiard.h"

using namespace std;

const int BilliardBuilder::BALL_MAX = 16;		// 使用するボールの数
const float BilliardBuilder::RADIUS = 0.0571f;	// 球の半径
const float BilliardBuilder::CUE_LENGTH = 1.0f;	// キューの半径


// Cue -----------------------------------------------------------------------------

void Cue::calcLocation(const MeshObject & cueBall) {
	Vector3 shift = { 0, 0, (length) / 2 + pullLevel + 0.08f };
	shift *= rotationMatrix;
	setLocation(cueBall.getLocation() - shift);
}

void Cue::pull() {
	if (pullLevel >= length / 2) {
		pullLevel = length / 2;
		return;
	}
	pullLevel += length / 64;
}

// 手球に与える力[kg*m / s ^ 2]を返す
Vector3 Cue::shot() {
	Vector3 power = { 0, 0, pullLevel};
	power *= rotationMatrix;
	pullLevel = 0;
	return power;
}



// BilliardBuilder -----------------------------------------------------------------------------

// 頂点座標からテクスチャ座標を算出する. 
// 変換式はなるべくいい感じになるように調整したが、数字が歪む
static void convert(Vector3 vertex, float R, float& u, float& v) {
	float theta = atan2(vertex.x, vertex.z);
	float ry = (vertex.y > R / 2) ?  -0.8f * vertex.y / R + 0.8f : 0.4f;
	v = 0.5f - ry * cos(theta);
	u = 0.5f + ry * sin(theta);
}

ID3DXMesh* BilliardBuilder::createBallMesh(IDirect3DDevice9 * d3dDevice, const float radius) {
	ID3DXMesh *mesh, *tmpMesh;		// 球のメッシュ

	// 球のメッシュを作成（全ての球で使いまわす）
	D3DXCreateSphere(d3dDevice, radius, 64, 32, &tmpMesh, NULL);

	// テクスチャが張れるようにFVFを変えたクローンを作成
	tmpMesh->CloneMeshFVF(tmpMesh->GetOptions(), MeshObject::FVF, d3dDevice, &mesh);

	MeshObject::MyVertex* pVer = NULL;
	mesh->LockVertexBuffer(0, (VOID**)&pVer);

	// テクスチャ座標を設定
	for (DWORD n = 0; n < mesh->GetNumVertices(); n++) {
		convert(pVer[n].position, radius, pVer[n].tu, pVer[n].tv);
		//Vector3 p = pVer[n].position;
		//printf("{%+6.2f, %+6.4f, %+6.4f} ... %+f, %+f, %d \n", p.x, p.y, p.z, pVer[n].tu, pVer[n].tv, n);
	}

	mesh->UnlockVertexBuffer();
	tmpMesh->Release();

	return mesh;
}

vector<MeshObject*> BilliardBuilder::createBalls(IDirect3DDevice9* d3dDevice) {

	const D3DCOLORVALUE BALL_COLORS[BALL_MAX] = {	// 球の色
		{ 0.9f, 0.9f, 0.9f },
		{ 0.9f, 0.7f, 0.0f }, { 0.0f, 0.1f, 0.9f }, { 0.9f, 0.0f, 0.0f },
		{ 0.2f, 0.0f, 0.7f }, { 0.8f, 0.4f, 0.2f },	{ 0.0f, 0.9f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.1f, 0.1f, 0.1f },	{ 0.8f, 0.8f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
	};
	const char* fnames[BALL_MAX] = {
		"Assets/Textures/Ball_00.png",
		"Assets/Textures/Ball_01.png", "Assets/Textures/Ball_02.png", "Assets/Textures/Ball_03.png",
		"Assets/Textures/Ball_04.png", "Assets/Textures/Ball_05.png", "Assets/Textures/Ball_06.png",
		"Assets/Textures/Ball_07.png", "Assets/Textures/Ball_08.png", "Assets/Textures/Ball_09.png",
		"Assets/Textures/Ball_10.png", "Assets/Textures/Ball_11.png", "Assets/Textures/Ball_12.png",
		"Assets/Textures/Ball_13.png", "Assets/Textures/Ball_14.png", "Assets/Textures/Ball_15.png"
	};

	vector<MeshObject*> ballList;
	ID3DXMesh		*mesh = createBallMesh(d3dDevice, RADIUS);
	PhysicalFeaturs *pf	  = new PhysicalFeaturs(0.1f, 0.02f, 0.0f, 9.8f, false);
	Collider		*c	  = new SphereCollider(RADIUS, 0.1f, 0.1f, 1.0f);

	// 球のメッシュを作成（全ての球で使いまわす）
	for (int i = 0; i < BALL_MAX; i++) {
		MeshObject *ball = new MeshObject(mesh);
		ball->loadTexture(d3dDevice, fnames[i]);
		ball->setDiffuse({ 1.0f, 1.0f, 1.0f, 1.0f });
		ball->setAmbient({ 0.6f, 0.6f, 0.6f, 1.0f });
		ball->setPhysicalFeatures(pf);
		ball->setCollider(c);
		ballList.push_back(ball);
	}

	return ballList;
}

void BilliardBuilder::initBallPosition(std::vector<MeshObject*> ballList, const int initNumber) {
	if (ballList.size() != BALL_MAX) {
		printf("error: ball list size is not correct %d", ballList.size());
		return;
	}

	const float margin = 0.001f;	// 球と球の隙間
	const POINT BALL_POINTS0[BALL_MAX] = {
		{ 0, -10 },
		{ 0, 0 },
		{ -1, 2 },{ 1, 2 },
		{ -2, 4 },{ 0, 4 },{ 2, 4 },
		{ -3, 6 },{ -1, 6 },{ 1, 6 },{ 3, 6 },
		{ -4, 8 },{ -2, 8 },{ 0, 8 },{ 2, 8 },{ 4, 8 }
	};
	const POINT BALL_POINTS1[BALL_MAX] = {	// 球の配置
		{ 0, -10 },
		{  0, 0 },
		{ -1, 2 },{ 1, 2 },
		{ -2, 4 },{ 0, 4 },{ 2, 4 },
		{ -1, 6 },{ 1, 6 },
		{  0, 8 },
		{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },
	};

	switch (initNumber){
	case 1:
		for (int i = 0; i < BALL_MAX; i++) {
			FLOAT x = BALL_POINTS1[i].x * (RADIUS + margin);
			FLOAT z = BALL_POINTS1[i].y * (RADIUS + margin);
			ballList[i]->setLocation(x, RADIUS, z);
			if (i > 9)ballList[i]->setEnable(false);
		}
		break;
	default:
		for (int i = 0; i < BALL_MAX; i++) {
			FLOAT x = BALL_POINTS0[i].x * (RADIUS + margin);
			FLOAT z = BALL_POINTS0[i].y * (RADIUS + margin);
			ballList[i]->setLocation(x, RADIUS, z);
		}
		break;
	}
}

std::vector<MeshObject*> BilliardBuilder::createTable(IDirect3DDevice9 * d3dDevice) {
	vector<MeshObject*> objList;
	ID3DXMesh *mesh, *mesh_lr, *mesh_bf;
	D3DMATERIAL9 material = {};
	const float WIDTH  = 1.6f;
	const float HEIGHT = 0.2f;
	const float DEPTH  = 2.9f;
	const float HOLE_SIZE = 0.28f;
	Vector3 wallLR = { HEIGHT, HEIGHT * 2, DEPTH / 2 - HOLE_SIZE, };
	Vector3 wallBF = { WIDTH - HOLE_SIZE, HEIGHT * 2, HEIGHT };

	D3DXCreateBox(d3dDevice, WIDTH, HEIGHT, DEPTH, &mesh, NULL);
	D3DXCreateBox(d3dDevice, wallLR.x, wallLR.y, wallLR.z, &mesh_lr, NULL);
	D3DXCreateBox(d3dDevice, wallBF.x, wallBF.y, wallBF.z, &mesh_bf, NULL);
	material.Diffuse = { 0.1f, 0.4f, 0.1f };
	material.Ambient = { 0.4f, 0.4f, 0.4f };

	PhysicalFeaturs *pf = new PhysicalFeaturs(400.0f, 0.0f, 0.0f, 0.0f, true);
	Collider *c_table = new BoxCollider(WIDTH, HEIGHT, DEPTH, 0.2f, 0.1f, 0.0f);
	Collider *c_lr    = new BoxCollider(wallLR.x, wallLR.y, wallLR.z, 0.2f, 0.1f, 0.4f);
	Collider *c_bf    = new BoxCollider(wallBF.x, wallBF.y, wallBF.z, 0.2f, 0.1f, 0.4f);

	objList.push_back(new MeshObject(mesh, material));		// テーブル
	objList.push_back(new MeshObject(mesh_lr, material));	// 左壁1
	objList.push_back(new MeshObject(mesh_lr, material));	// 左壁2
	objList.push_back(new MeshObject(mesh_lr, material));	// 右壁1
	objList.push_back(new MeshObject(mesh_lr, material));	// 右壁2
	objList.push_back(new MeshObject(mesh_bf, material));	// 後壁
	objList.push_back(new MeshObject(mesh_bf, material));	// 前壁
	
	objList[0]->setCollider(c_table);
	objList[0]->setLocation(0, -HEIGHT / 2, 0);

	for (int i = 0; i < 7; i++) {
		objList[i]->setPhysicalFeatures(pf);
	}
	for (int i = 1, j = 1; i < 5; i++, j *= -1) {	// 左右の壁
		objList[i]->setCollider(c_lr);
		if (i < 3) { objList[i]->setLocation(-(WIDTH + HEIGHT) / 2, 0, j * DEPTH / 4); }
		else	   { objList[i]->setLocation( (WIDTH + HEIGHT) / 2, 0, j * DEPTH / 4); }
	}
	for (int i = 5, j = 1; i < 7; i++, j *= -1) {	// 後前の壁
		objList[i]->setCollider(c_bf);
		objList[i]->setLocation(0, 0, j * (DEPTH + HEIGHT) / 2);
	}

	return objList;
}

Cue * BilliardBuilder::createCue(IDirect3DDevice9 * d3dDevice) {
	ID3DXMesh *mesh;
	D3DMATERIAL9 material = { 0 };

	D3DXCreateCylinder(d3dDevice, 0.03f, 0.01f, CUE_LENGTH, 16, 8, &mesh, NULL);
	material.Diffuse = { 0.9f, 0.9f, 0.9f };
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	Cue *cue = new Cue(mesh, material, 1.0f);
	cue->setPhysicalFeatures(new PhysicalFeaturs(3.0f));
	return cue;
}

// BilliardBuilderで生成したオブジェクトを破棄する
void BilliardBuilder::deleteObjects(std::vector<MeshObject*> balls, std::vector<MeshObject*> table) {
	balls[0]->release();
	SAFE_DELETE(balls[0]);
	for (size_t i = 1; i < balls.size(); i++) {
		balls[i]->releaseTexture();
		SAFE_DELETE(balls[i]);
	}

	PhysicalFeaturs* pf = table[0]->getPhysicalFeaturs();
	SAFE_DELETE(pf);
	
	int index[3] = { 0, 1, 5 };
	for (int i = 0; i < 3; i++) {
		table[index[i]]->deleteCollider();
		table[index[i]]->releaseMesh();
	}
}
=======
#include "../Header/stdafx.h"
#include "../Header/billiard.h"

using namespace std;

const int BilliardBuilder::BALL_MAX = 16;		// 使用するボールの数
const float BilliardBuilder::RADIUS = 0.0571f;	// 球の半径
const float BilliardBuilder::CUE_LENGTH = 1.0f;	// キューの半径


// Cue -----------------------------------------------------------------------------

void Cue::calcLocation(const MeshObject & cueBall) {
	Vector3 shift = { 0, 0, (length) / 2 + pullLevel + 0.08f };
	shift *= rotationMatrix;
	setLocation(cueBall.getLocation() - shift);
}

void Cue::pull() {
	if (pullLevel >= length / 2) {
		pullLevel = length / 2;
		return;
	}
	pullLevel += length / 64;
}

// 手球に与える力[kg*m / s ^ 2]を返す
Vector3 Cue::shot() {
	Vector3 power = { 0, 0, pullLevel};
	power *= rotationMatrix;
	pullLevel = 0;
	return power;
}



// BilliardBuilder -----------------------------------------------------------------------------

// 頂点座標からテクスチャ座標を算出する. 
// 変換式はなるべくいい感じになるように調整したが、数字が歪む
static void convert(Vector3 vertex, float R, float& u, float& v) {
	float theta = atan2(vertex.x, vertex.z);
	float ry = (vertex.y > R / 2) ?  -0.8f * vertex.y / R + 0.8f : 0.4f;
	v = 0.5f - ry * cos(theta);
	u = 0.5f + ry * sin(theta);
}

ID3DXMesh* BilliardBuilder::createBallMesh(IDirect3DDevice9 * d3dDevice, const float radius) {
	ID3DXMesh *mesh, *tmpMesh;		// 球のメッシュ

	// 球のメッシュを作成（全ての球で使いまわす）
	D3DXCreateSphere(d3dDevice, radius, 64, 32, &tmpMesh, NULL);

	// テクスチャが張れるようにFVFを変えたクローンを作成
	tmpMesh->CloneMeshFVF(tmpMesh->GetOptions(), MeshObject::FVF, d3dDevice, &mesh);

	MeshObject::MyVertex* pVer = NULL;
	mesh->LockVertexBuffer(0, (VOID**)&pVer);

	// テクスチャ座標を設定
	for (DWORD n = 0; n < mesh->GetNumVertices(); n++) {
		convert(pVer[n].position, radius, pVer[n].tu, pVer[n].tv);
		//Vector3 p = pVer[n].position;
		//printf("{%+6.2f, %+6.4f, %+6.4f} ... %+f, %+f, %d \n", p.x, p.y, p.z, pVer[n].tu, pVer[n].tv, n);
	}

	mesh->UnlockVertexBuffer();
	tmpMesh->Release();

	return mesh;
}

vector<MeshObject*> BilliardBuilder::createBalls(IDirect3DDevice9* d3dDevice) {

	const D3DCOLORVALUE BALL_COLORS[BALL_MAX] = {	// 球の色
		{ 0.9f, 0.9f, 0.9f },
		{ 0.9f, 0.7f, 0.0f }, { 0.0f, 0.1f, 0.9f }, { 0.9f, 0.0f, 0.0f },
		{ 0.2f, 0.0f, 0.7f }, { 0.8f, 0.4f, 0.2f },	{ 0.0f, 0.9f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.1f, 0.1f, 0.1f },	{ 0.8f, 0.8f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
	};
	const char* fnames[BALL_MAX] = {
		"Assets/Textures/Ball_00.png",
		"Assets/Textures/Ball_01.png", "Assets/Textures/Ball_02.png", "Assets/Textures/Ball_03.png",
		"Assets/Textures/Ball_04.png", "Assets/Textures/Ball_05.png", "Assets/Textures/Ball_06.png",
		"Assets/Textures/Ball_07.png", "Assets/Textures/Ball_08.png", "Assets/Textures/Ball_09.png",
		"Assets/Textures/Ball_10.png", "Assets/Textures/Ball_11.png", "Assets/Textures/Ball_12.png",
		"Assets/Textures/Ball_13.png", "Assets/Textures/Ball_14.png", "Assets/Textures/Ball_15.png"
	};

	vector<MeshObject*> ballList;
	ID3DXMesh		*mesh = createBallMesh(d3dDevice, RADIUS);
	PhysicalFeaturs *pf	  = new PhysicalFeaturs(0.1f, 0.02f, 0.0f, 9.8f, false);
	Collider		*c	  = new SphereCollider(RADIUS, 0.1f, 0.1f, 1.0f);

	// 球のメッシュを作成（全ての球で使いまわす）
	for (int i = 0; i < BALL_MAX; i++) {
		MeshObject *ball = new MeshObject(mesh);
		ball->loadTexture(d3dDevice, fnames[i]);
		ball->setDiffuse({ 1.0f, 1.0f, 1.0f, 1.0f });
		ball->setAmbient({ 0.6f, 0.6f, 0.6f, 1.0f });
		ball->setPhysicalFeatures(pf);
		ball->setCollider(c);
		ballList.push_back(ball);
	}

	return ballList;
}

void BilliardBuilder::initBallPosition(std::vector<MeshObject*> ballList, const int initNumber) {
	if (ballList.size() != BALL_MAX) {
		printf("error: ball list size is not correct %d", ballList.size());
		return;
	}

	const float margin = 0.001f;	// 球と球の隙間
	const POINT BALL_POINTS0[BALL_MAX] = {
		{ 0, -10 },
		{ 0, 0 },
		{ -1, 2 },{ 1, 2 },
		{ -2, 4 },{ 0, 4 },{ 2, 4 },
		{ -3, 6 },{ -1, 6 },{ 1, 6 },{ 3, 6 },
		{ -4, 8 },{ -2, 8 },{ 0, 8 },{ 2, 8 },{ 4, 8 }
	};
	const POINT BALL_POINTS1[BALL_MAX] = {	// 球の配置
		{ 0, -10 },
		{  0, 0 },
		{ -1, 2 },{ 1, 2 },
		{ -2, 4 },{ 0, 4 },{ 2, 4 },
		{ -1, 6 },{ 1, 6 },
		{  0, 8 },
		{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },{ 1, 0 },
	};

	switch (initNumber){
	case 1:
		for (int i = 0; i < BALL_MAX; i++) {
			FLOAT x = BALL_POINTS1[i].x * (RADIUS + margin);
			FLOAT z = BALL_POINTS1[i].y * (RADIUS + margin);
			ballList[i]->setLocation(x, RADIUS, z);
			if (i > 9)ballList[i]->setEnable(false);
		}
		break;
	default:
		for (int i = 0; i < BALL_MAX; i++) {
			FLOAT x = BALL_POINTS0[i].x * (RADIUS + margin);
			FLOAT z = BALL_POINTS0[i].y * (RADIUS + margin);
			ballList[i]->setLocation(x, RADIUS, z);
		}
		break;
	}
}

std::vector<MeshObject*> BilliardBuilder::createTable(IDirect3DDevice9 * d3dDevice) {
	vector<MeshObject*> objList;
	ID3DXMesh *mesh, *mesh_lr, *mesh_bf;
	D3DMATERIAL9 material = {};
	const float WIDTH  = 1.6f;
	const float HEIGHT = 0.2f;
	const float DEPTH  = 2.9f;
	const float HOLE_SIZE = 0.28f;
	Vector3 wallLR = { HEIGHT, HEIGHT * 2, DEPTH / 2 - HOLE_SIZE, };
	Vector3 wallBF = { WIDTH - HOLE_SIZE, HEIGHT * 2, HEIGHT };

	D3DXCreateBox(d3dDevice, WIDTH, HEIGHT, DEPTH, &mesh, NULL);
	D3DXCreateBox(d3dDevice, wallLR.x, wallLR.y, wallLR.z, &mesh_lr, NULL);
	D3DXCreateBox(d3dDevice, wallBF.x, wallBF.y, wallBF.z, &mesh_bf, NULL);
	material.Diffuse = { 0.1f, 0.4f, 0.1f };
	material.Ambient = { 0.4f, 0.4f, 0.4f };

	PhysicalFeaturs *pf = new PhysicalFeaturs(400.0f, 0.0f, 0.0f, 0.0f, true);
	Collider *c_table = new BoxCollider(WIDTH, HEIGHT, DEPTH, 0.2f, 0.1f, 0.0f);
	Collider *c_lr    = new BoxCollider(wallLR.x, wallLR.y, wallLR.z, 0.2f, 0.1f, 0.4f);
	Collider *c_bf    = new BoxCollider(wallBF.x, wallBF.y, wallBF.z, 0.2f, 0.1f, 0.4f);

	objList.push_back(new MeshObject(mesh, material));		// テーブル
	objList.push_back(new MeshObject(mesh_lr, material));	// 左壁1
	objList.push_back(new MeshObject(mesh_lr, material));	// 左壁2
	objList.push_back(new MeshObject(mesh_lr, material));	// 右壁1
	objList.push_back(new MeshObject(mesh_lr, material));	// 右壁2
	objList.push_back(new MeshObject(mesh_bf, material));	// 後壁
	objList.push_back(new MeshObject(mesh_bf, material));	// 前壁
	
	objList[0]->setCollider(c_table);
	objList[0]->setLocation(0, -HEIGHT / 2, 0);

	for (int i = 0; i < 7; i++) {
		objList[i]->setPhysicalFeatures(pf);
	}
	for (int i = 1, j = 1; i < 5; i++, j *= -1) {	// 左右の壁
		objList[i]->setCollider(c_lr);
		if (i < 3) { objList[i]->setLocation(-(WIDTH + HEIGHT) / 2, 0, j * DEPTH / 4); }
		else	   { objList[i]->setLocation( (WIDTH + HEIGHT) / 2, 0, j * DEPTH / 4); }
	}
	for (int i = 5, j = 1; i < 7; i++, j *= -1) {	// 後前の壁
		objList[i]->setCollider(c_bf);
		objList[i]->setLocation(0, 0, j * (DEPTH + HEIGHT) / 2);
	}

	return objList;
}

Cue * BilliardBuilder::createCue(IDirect3DDevice9 * d3dDevice) {
	ID3DXMesh *mesh;
	D3DMATERIAL9 material = { 0 };

	D3DXCreateCylinder(d3dDevice, 0.03f, 0.01f, CUE_LENGTH, 16, 8, &mesh, NULL);
	material.Diffuse = { 0.9f, 0.9f, 0.9f };
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	Cue *cue = new Cue(mesh, material, 1.0f);
	cue->setPhysicalFeatures(new PhysicalFeaturs(3.0f));
	return cue;
}

// BilliardBuilderで生成したオブジェクトを破棄する
void BilliardBuilder::deleteObjects(std::vector<MeshObject*> balls, std::vector<MeshObject*> table) {
	balls[0]->release();
	SAFE_DELETE(balls[0]);
	for (size_t i = 1; i < balls.size(); i++) {
		balls[i]->releaseTexture();
		SAFE_DELETE(balls[i]);
	}

	PhysicalFeaturs* pf = table[0]->getPhysicalFeaturs();
	SAFE_DELETE(pf);
	
	int index[3] = { 0, 1, 5 };
	for (int i = 0; i < 3; i++) {
		table[index[i]]->deleteCollider();
		table[index[i]]->releaseMesh();
	}
}
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
