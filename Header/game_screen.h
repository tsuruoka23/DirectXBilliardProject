#pragma once
#include "stdafx.h"
#include "game_object.h"
#include "billiard.h"
#include "game_component.h"

#define PI 3.14


// �Q�[����ʂ̍X�V��`����s���N���X
// �f�o�C�X���Ǘ�����
class GameScreen {

private:
	IDirect3D9		 *direct3D;
	IDirect3DDevice9 *d3dDevice;
	D3DPRESENT_PARAMETERS d3dpp; // �f�o�C�X�̏�Ԃ������\����

	D3DXVECTOR3 camera;
	GameComponent *gameComponent;
	
	UINT32 updateCount;	// update���Ăяo���ꂽ��
	UINT32 fps;
	bool successedInit;

	POINT mouse_from;			// �J���������̑�����s�����߂̕ϐ�
	bool  mouse_ldown = false;	// �}�E�X��L�{�^����������Ă��邩�ǂ���

public:

	GameScreen() {
		successedInit = false;
	};

	~GameScreen() {
		release();
	}

	bool initDXGraphics(HWND &hWnd);
	void release();
	void changeWindowSize(int width, int height);
	void setRender(int width, int height);
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);
	void changeCameraPoint(int wp, int mouse_x, int mouse_y);

	void update();
private:
	void draw();
};