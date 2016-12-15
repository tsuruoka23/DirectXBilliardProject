<<<<<<< HEAD
#pragma once
#include "stdafx.h"
#include "game_object.h"
#include "billiard.h"
#include "game_scene.h"
#include "game_ui.h"

class GameScene;

// �Q�[����ʂ̍X�V��`����s���N���X
// �f�o�C�X���Ǘ�����
class GameScreen {

private:
	IDirect3D9		 *direct3D;
	IDirect3DDevice9 *d3dDevice;
	D3DPRESENT_PARAMETERS d3dpp; // �f�o�C�X�̏�Ԃ������\����
	D3DXVECTOR3 camera;
	
	GameScene *activeScene;		// ���ݕ\�����̃V�[��
	GameScene *nonActiveScene;	// �j�����Ă悢�V�[��
	
	UINT32 updateCount;	// update���Ăяo���ꂽ��
	UINT32 fps;
	bool successedInit;

	POINT mouse_from;			// �J���������̑�����s�����߂̕ϐ�
	bool  mouse_ldown = false;	// �}�E�X��L�{�^����������Ă��邩�ǂ���

public:

	GameScreen() {
		activeScene = NULL;
		nonActiveScene = NULL;
		successedInit = false;
	};

	~GameScreen() {
		SAFE_RELEASE(direct3D);
		SAFE_RELEASE(d3dDevice);
	}

	UINT32 getFps() { return fps; }

	bool initDXGraphics(HWND &hWnd);
	void initRender(int width, int height);
	void changeWindowSize(int width, int height);
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

	void moveCamera(int wp, int mouse_x, int mouse_y);
	void changeCameraPoint(const int cameraNo);
	void switchScene(const int sceneId);
	void update();

private:
	void draw();
=======
#pragma once
#include "stdafx.h"
#include "game_object.h"
#include "billiard.h"
#include "game_scene.h"
#include "game_ui.h"

class GameScene;

// �Q�[����ʂ̍X�V��`����s���N���X
// �f�o�C�X���Ǘ�����
class GameScreen {

private:
	IDirect3D9		 *direct3D;
	IDirect3DDevice9 *d3dDevice;
	D3DPRESENT_PARAMETERS d3dpp; // �f�o�C�X�̏�Ԃ������\����
	D3DXVECTOR3 camera;
	
	GameScene *activeScene;		// ���ݕ\�����̃V�[��
	GameScene *nonActiveScene;	// �j�����Ă悢�V�[��
	
	UINT32 updateCount;	// update���Ăяo���ꂽ��
	UINT32 fps;
	bool successedInit;

	POINT mouse_from;			// �J���������̑�����s�����߂̕ϐ�
	bool  mouse_ldown = false;	// �}�E�X��L�{�^����������Ă��邩�ǂ���

public:

	GameScreen() {
		activeScene = NULL;
		nonActiveScene = NULL;
		successedInit = false;
	};

	~GameScreen() {
		SAFE_RELEASE(direct3D);
		SAFE_RELEASE(d3dDevice);
	}

	UINT32 getFps() { return fps; }

	bool initDXGraphics(HWND &hWnd);
	void initRender(int width, int height);
	void changeWindowSize(int width, int height);
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

	void moveCamera(int wp, int mouse_x, int mouse_y);
	void changeCameraPoint(const int cameraNo);
	void switchScene(const int sceneId);
	void update();

private:
	void draw();
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
};