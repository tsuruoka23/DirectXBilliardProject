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


// �L���[�i�苅��e���_�j
class Cue : public MeshObject{
private:
	float length;		// �_�̒���
	float pullLevel;	// �L���[���ǂꂾ�������Ă��邩
public:
	Cue(ID3DXMesh * mesh, D3DMATERIAL9 material, float length) : MeshObject(mesh, material), length(length) {};
	~Cue(){};

	// �苅�̈ʒu�ƃL���[�̉�]����L���[�̓K�؂ȍ��W�����߂�
	void calcLocation(const MeshObject& cueBall);
	void pull();
	Vector3 shot();
};


// �r�����[�h�̏�������I������A�Q�[���i�s���Ǘ�����
class BilliardBuilder {
public:
	static const int BALL_MAX;		// �g�����̍ő吔
	static const float RADIUS;		// ���̔��a
	static const float CUE_LENGTH;	// �L���[�̒���

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


// �L���[�i�苅��e���_�j
class Cue : public MeshObject{
private:
	float length;		// �_�̒���
	float pullLevel;	// �L���[���ǂꂾ�������Ă��邩
public:
	Cue(ID3DXMesh * mesh, D3DMATERIAL9 material, float length) : MeshObject(mesh, material), length(length) {};
	~Cue(){};

	// �苅�̈ʒu�ƃL���[�̉�]����L���[�̓K�؂ȍ��W�����߂�
	void calcLocation(const MeshObject& cueBall);
	void pull();
	Vector3 shot();
};


// �r�����[�h�̏�������I������A�Q�[���i�s���Ǘ�����
class BilliardBuilder {
public:
	static const int BALL_MAX;		// �g�����̍ő吔
	static const float RADIUS;		// ���̔��a
	static const float CUE_LENGTH;	// �L���[�̒���

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