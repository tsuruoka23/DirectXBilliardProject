#pragma once
#include "stdafx.h"
#include "game_object.h"


class Ball: public MeshObject {

	Ball(ID3DXMesh * mesh, D3DMATERIAL9 material = {});
	Ball(IDirect3DDevice9*, char*);
};

class Table: public MeshObject {

};

// ビリヤードの初期化や終了判定、ゲーム進行を管理する
class Billiardfacilitator {
public:
	static const int BALL_NUM = 16;

	

	Billiardfacilitator() {};
	~Billiardfacilitator() {};

	static std::vector<GameObject*> createBalls(IDirect3DDevice9* d3dDevice);
	static std::vector<GameObject*> createTable(IDirect3DDevice9* d3dDevice);
	
};