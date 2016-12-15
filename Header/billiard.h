<<<<<<< HEAD
#pragma once
#include "stdafx.h"
#include "game_object.h"


class Ball: public MeshObject {

	Ball(ID3DXMesh * mesh, D3DMATERIAL9 material = {});
	Ball(IDirect3DDevice9*, char*);
};


class Table: public MeshObject {

};


// キュー（手球を弾く棒）
class Cue : public MeshObject{
private:
	float length;		// 棒の長さ
	float pullLevel;	// キューをどれだけ引いているか
public:
	Cue(ID3DXMesh * mesh, D3DMATERIAL9 material, float length) : MeshObject(mesh, material), length(length) {};
	~Cue(){};

	// 手球の位置とキューの回転からキューの適切な座標を求める
	void calcLocation(const MeshObject& cueBall);
	void pull();
	Vector3 shot();
};


// ビリヤードの初期化や終了判定、ゲーム進行を管理する
class BilliardBuilder {
public:
	static const int BALL_MAX;		// 使う球の最大数
	static const float RADIUS;		// 球の半径
	static const float CUE_LENGTH;	// キューの長さ

	BilliardBuilder() {};
	~BilliardBuilder() {};

	static Cue* createCue(IDirect3DDevice9* d3dDevice);
	static ID3DXMesh* createBallMesh(IDirect3DDevice9* d3dDevice, const float radius);
	static std::vector<MeshObject*> createBalls(IDirect3DDevice9* d3dDevice);
	static std::vector<MeshObject*> createTable(IDirect3DDevice9* d3dDevice);
	static void initBallPosition(std::vector<MeshObject*> balls, const int initNumber = 0);

	static void deleteObjects(std::vector<MeshObject*> balls, std::vector<MeshObject*> table);
=======
#pragma once
#include "stdafx.h"
#include "game_object.h"


class Ball: public MeshObject {

	Ball(ID3DXMesh * mesh, D3DMATERIAL9 material = {});
	Ball(IDirect3DDevice9*, char*);
};


class Table: public MeshObject {

};


// キュー（手球を弾く棒）
class Cue : public MeshObject{
private:
	float length;		// 棒の長さ
	float pullLevel;	// キューをどれだけ引いているか
public:
	Cue(ID3DXMesh * mesh, D3DMATERIAL9 material, float length) : MeshObject(mesh, material), length(length) {};
	~Cue(){};

	// 手球の位置とキューの回転からキューの適切な座標を求める
	void calcLocation(const MeshObject& cueBall);
	void pull();
	Vector3 shot();
};


// ビリヤードの初期化や終了判定、ゲーム進行を管理する
class BilliardBuilder {
public:
	static const int BALL_MAX;		// 使う球の最大数
	static const float RADIUS;		// 球の半径
	static const float CUE_LENGTH;	// キューの長さ

	BilliardBuilder() {};
	~BilliardBuilder() {};

	static Cue* createCue(IDirect3DDevice9* d3dDevice);
	static ID3DXMesh* createBallMesh(IDirect3DDevice9* d3dDevice, const float radius);
	static std::vector<MeshObject*> createBalls(IDirect3DDevice9* d3dDevice);
	static std::vector<MeshObject*> createTable(IDirect3DDevice9* d3dDevice);
	static void initBallPosition(std::vector<MeshObject*> balls, const int initNumber = 0);

	static void deleteObjects(std::vector<MeshObject*> balls, std::vector<MeshObject*> table);
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
};