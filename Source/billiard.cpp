#include "../Header/stdafx.h"
#include "../Header/billiard.h"

using namespace std;

struct BallColor {
	float r, g, b;
};



vector<GameObject*> Billiardfacilitator::createBalls(IDirect3DDevice9* d3dDevice) {
	vector<GameObject*> ballList;

	ID3DXMesh *mesh;		// ���̃��b�V��
	D3DMATERIAL9 material = {};
	float radius = 0.0571f;	// ���̔��a
	float margin = 0.001f;	// �����z�u�ɂ����鋅�Ƌ��̌���
	//radius = 0.05f;

	const D3DCOLORVALUE BALL_COLORS[BALL_NUM] = {	// ���̐F
		{ 0.9f, 0.9f, 0.9f },
		{ 0.9f, 0.7f, 0.0f }, { 0.0f, 0.1f, 0.9f }, { 0.9f, 0.0f, 0.0f },
		{ 0.2f, 0.0f, 0.7f }, { 0.8f, 0.4f, 0.2f },	{ 0.0f, 0.9f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.1f, 0.1f, 0.1f },	{ 0.8f, 0.8f, 0.0f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
		{ 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f }, { 0.3f, 0.4f, 0.4f },
	};

	const POINT BALL_POINTS[BALL_NUM] = {	// ���̔z�u
		{0, -10}, 
		{0, 0}, 
		{ -1, 1 },{  1, 1 },
		{ -2, 2 },{  0, 2 },{ 2, 2 },
		{ -3, 3 },{ -1, 3 },{ 1, 3 },{ 3, 3 },
		{ -4, 4 },{ -2, 4 },{ 0, 4 },{ 2, 4 },{ 4, 4 } 
	};

	// ���̃��b�V�����쐬�i�S�Ă̋��Ŏg���܂킷�j
	D3DXCreateSphere(d3dDevice, radius, 32, 16, &mesh, NULL);
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	for (int i = 0; i < BALL_NUM; i++) {
		MeshObject *ball = new MeshObject(mesh, material);
		PhysicalFeaturs *pf = new PhysicalFeaturs(0.1f, 0.01f, 0.0f, 0.0f, false);
		Collider *c = new SphereCollider(radius, 0.1f, 0.1f, 1.0f);
		FLOAT x = BALL_POINTS[i].x * (radius + margin);
		FLOAT z = BALL_POINTS[i].y * (radius + margin) * (FLOAT)sqrt(3) * 1.1;

		ball->setDiffuse(BALL_COLORS[i]);
		ball->setPhysicalFeatures(pf);
		ball->setCollider(c);
		ball->setLocation(x, 0.5f, z);

		ballList.push_back(ball);
	}

	return ballList;
}

GameObject * Billiardfacilitator::createTable(IDirect3DDevice9 * d3dDevice) {

	ID3DXMesh *mesh;
	D3DMATERIAL9 material = {};
	const float WIDTH  = 2.90f;
	const float HEIGHT = 0.10f;
	const float DEPTH  = 1.60f;

	D3DXCreateBox(d3dDevice, WIDTH, HEIGHT, DEPTH, &mesh, NULL);
	material.Diffuse = { 0.2f, 0.7f, 0.2f };
	material.Ambient = { 0.3f, 0.3f, 0.3f };

	MeshObject *table = new MeshObject(mesh, material);
	PhysicalFeaturs *pf = new PhysicalFeaturs(400.0f, 0.0f, 0.0f, 0.0f, false);
	Collider *c = new BoxCollider(WIDTH, HEIGHT, DEPTH, 0.1f, 0.1f, 0.1f);

	table->setPhysicalFeatures(pf);
	table->setCollider(c);
	table->setLocation(0, 0.0f, 0);
	table->setRotation(0, (float)PI / 2, 0);

	return table;
}
