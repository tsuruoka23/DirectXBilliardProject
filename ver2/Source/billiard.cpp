#include "../Header/stdafx.h"
#include "../Header/billiard.h"

using namespace std;

struct BallColor {
	float r, g, b;
};



vector<GameObject*> Billiardfacilitator::createBalls(IDirect3DDevice9* d3dDevice) {
	vector<GameObject*> ballList;

	ID3DXMesh *mesh;		// 球のメッシュ
	D3DMATERIAL9 material = {};
	float radius = 0.0571f;	// 球の半径
	float margin = 0.001f;	// 初期配置における球と球の隙間
	//radius = 0.05f;

	const D3DCOLORVALUE BALL_COLORS[BALL_NUM] = {	// 球の色
		{ 0.9f, 0.9f, 0.9f },
		{ 0.9f, 0.7f, 0.0f }, { 0.0f, 0.1f, 0.9f }, { 0.9f, 0.0f, 0.0f },
		{ 0.2f, 0.0f, 0.7f }, { 0.8f, 0.4f, 0.2f },	{ 0.0f, 0.9f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.1f, 0.1f, 0.1f },	{ 0.8f, 0.8f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
	};

	const POINT BALL_POINTS[BALL_NUM] = {	// 球の配置
		{0, -10}, 
		{0, 0}, 
		{ -1, 1 },{  1, 1 },
		{ -2, 2 },{  0, 2 },{ 2, 2 },
		{ -3, 3 },{ -1, 3 },{ 1, 3 },{ 3, 3 },
		{ -4, 4 },{ -2, 4 },{ 0, 4 },{ 2, 4 },{ 4, 4 } 
	};

	// 球のメッシュを作成（全ての球で使いまわす）
	D3DXCreateSphere(d3dDevice, radius, 32, 16, &mesh, NULL);
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	for (int i = 0; i < BALL_NUM; i++) {
		MeshObject *ball = new MeshObject(mesh, material);
		PhysicalFeaturs *pf = new PhysicalFeaturs(0.1f, 0.0f, 0.0f, 0.0f, false);
		Collider *c = new SphereCollider(radius, 0.1f, 0.1f, 1.0f);
		FLOAT x = BALL_POINTS[i].x * (radius + margin);
		FLOAT z = BALL_POINTS[i].y * (radius + margin) * (FLOAT)sqrt(3) * 1.1;

		ball->setDiffuse(BALL_COLORS[i]);
		ball->setPhysicalFeatures(pf);
		ball->setCollider(c);
		ball->setLocation(x, 0.12f, z);

		ballList.push_back(ball);
	}

	return ballList;
}

// とりあえずやっつけで
std::vector<GameObject*> Billiardfacilitator::createTable(IDirect3DDevice9 * d3dDevice) {

	vector<GameObject*> objList;
	ID3DXMesh *mesh, *mesh_side1, *mesh_side2;
	D3DMATERIAL9 material = {};
	const float WIDTH  = 1.60f;
	const float HEIGHT = 0.10f;
	const float DEPTH  = 2.90f;

	D3DXCreateBox(d3dDevice, WIDTH, HEIGHT, DEPTH, &mesh, NULL);
	D3DXCreateBox(d3dDevice, 0.1f, HEIGHT * 3, DEPTH, &mesh_side1, NULL);
	D3DXCreateBox(d3dDevice, WIDTH, HEIGHT * 3, 0.1f, &mesh_side2, NULL);
	material.Diffuse = { 0.2f, 0.7f, 0.2f };
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	MeshObject *table = new MeshObject(mesh, material);
	MeshObject *side_left = new MeshObject(mesh_side1, material);
	MeshObject *side_right = new MeshObject(mesh_side1, material);
	MeshObject *side_top = new MeshObject(mesh_side2, material);
	MeshObject *side_bottom = new MeshObject(mesh_side2, material);
	PhysicalFeaturs *pf = new PhysicalFeaturs(400.0f, 0.0f, 0.0f, 0.0f, true);
	Collider *c = new BoxCollider(WIDTH, HEIGHT, DEPTH, 0.2f, 0.1f, 0.6f);
	Collider *c_side1 = new BoxCollider(0.1f, HEIGHT * 3, DEPTH * 1, 0.2f, 0.1f, 0.7f);
	Collider *c_side2 = new BoxCollider(WIDTH, HEIGHT * 3, 0.1f, 0.2f, 0.1f, 0.7f);

	table->setPhysicalFeatures(pf);
	table->setCollider(c);
	table->setLocation(0, 0.0f, 0);
	side_left->setPhysicalFeatures(pf);
	side_left->setCollider(c_side1);
	side_left->setLocation(-WIDTH / 2, HEIGHT, 0);
	side_right->setPhysicalFeatures(pf);
	side_right->setCollider(c_side1);
	side_right->setLocation(WIDTH / 2, HEIGHT, 0);
	side_top->setPhysicalFeatures(pf);
	side_top->setCollider(c_side2);
	side_top->setLocation(0, HEIGHT, DEPTH / 2);
	side_bottom->setPhysicalFeatures(pf);
	side_bottom->setCollider(c_side2);
	side_bottom->setLocation(0, HEIGHT, -DEPTH / 2);
	//table->setRotation(0, (float)PI / 2, 0);
	objList.push_back(table);
	objList.push_back(side_left);
	objList.push_back(side_right);
	objList.push_back(side_top);
	objList.push_back(side_bottom);

	return objList;
}
