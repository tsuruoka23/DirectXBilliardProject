<<<<<<< HEAD

#include "../Header/stdafx.h"
#include "../Header/game_screen.h"
#include "../Header/utility.h"
#include "../Header/game_ui.h"

using namespace std;


bool GameScreen::initDXGraphics(HWND &hWnd) {

	D3DDISPLAYMODE dmode;	// ������̃f�B�X�v���C���[�h
	direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	// �\����D3DPRESENT_PARAMETERS�̒l��ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = dmode.Format;
	d3dpp.BackBufferCount = 1;					// �_�u���A�g���v���o�b�t�@�Ƃ́H
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// �_�u���o�b�t�@�����O���̃X���b�v������w��
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice);

	changeCameraPoint(0);
	activeScene = new StartScene(d3dDevice, this);

	successedInit = true;
	fps = dmode.RefreshRate;

	printf("dmode w, h, fRate, Format = %d, %d, %d, %d\n", dmode.Width, dmode.Height, dmode.RefreshRate, dmode.Format);
	printf("adapter count = %d\n", direct3D->GetAdapterCount());
	
	return true;
}

void GameScreen::initRender(int width, int height) {

	D3DXMATRIX d3dm;
	FLOAT aspect = (FLOAT)width / (FLOAT)height;

	// fovy:����p, aspect:�A�X�y�N�g��, zn:�ł��߂��ʂ�Z���W, zf:�ł������ʂ�Z���W
	D3DXMatrixPerspectiveFovLH(&d3dm, (FLOAT)D3DXToRadian(45.0), aspect, 1, 1000);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dm);

	// pEye:�J�����̈ʒu, pAt:�J���������ڂ���œ_, pUp:���[���h���W�`�̃J�����̏㕔
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
	
	d3dDevice->LightEnable(0, TRUE);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// �J�����O���[�h: �w�ʂ̏�����r������
	d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff808080);		// �A���r�G���g���C�g

}

void GameScreen::changeWindowSize(int width, int height) {
	if (!d3dDevice) return;

	activeScene->onLostDevice();

	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dDevice->Reset(&d3dpp);
	printf("changeWindowSize: w,h = %4d, %4d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
	initRender(width, height);

	activeScene->onLostDevice();
}



// �t���[�����X�V������ draw()���Ăяo��
void GameScreen::update() {
	updateCount++;
	if (activeScene == NULL) {
		MessageBox(NULL, "game scene is NULL", "ERROR", MB_OK);
	}
	if (nonActiveScene != NULL) {
		SAFE_DELETE(nonActiveScene);
	}
	activeScene->update(fps);
	draw();
}


void GameScreen::draw() {

	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x33, 0x33, 0x33), 1.0f, 0);
	d3dDevice->BeginScene();	// �o�b�t�@�ւ̕`��J�n

	activeScene->draw();
	
	d3dDevice->EndScene();		// �o�b�t�@�ւ̕`��I�� 
	d3dDevice->Present(NULL, NULL, NULL, NULL);
	//ValidateRect(hWnd, NULL);
}


void GameScreen::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		mouse_ldown = true;
		mouse_from.x = x;
		mouse_from.y = y;
		break;
	case WM_LBUTTONUP:
		mouse_ldown = false;
		break;
	case WM_RBUTTONDOWN: break;
	case WM_RBUTTONUP: break;
	case WM_MOUSEMOVE:
		if (mouse_ldown) {
			moveCamera(wp, x, y);
		}
		break;
	case WM_KEYUP:
		if (wp == '8') { switchScene(StartScene::SCENE_ID); }
		else if (wp == '9') { switchScene(PlayScene::SCENE_ID); }
		else if (wp >= '0' && wp <= '7') changeCameraPoint(wp - '0');
		break;
	}

	activeScene->receiveMouseInput(msg, wp, x, y);
}

void GameScreen::moveCamera(int wp, int mouse_x, int mouse_y) {
	D3DXMATRIX m;
	D3DXMATRIX	d3dm;

	if (abs(mouse_from.x - mouse_x) < abs(mouse_from.y - mouse_y)) {
		if ((wp & MK_SHIFT) || (wp & MK_CONTROL)) {
			camera *= (mouse_from.y - mouse_y > 0) ? 1.05f : 0.95f;
		}
		else {
			camera.y += (mouse_from.y - mouse_y > 0) ? 0.1f : -0.1f;
		}
	}
	else if(abs(mouse_from.x - mouse_x) > 1){
		D3DXMatrixRotationY(&d3dm, D3DXToRadian((mouse_from.x - mouse_x > 0) ? 3 : -3));
		camera = multiple(camera, d3dm);
	}

	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3( 0, 0, 0 ), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);

	mouse_from.x = mouse_x;
	mouse_from.y = mouse_y;
}

// �J�����̈ʒu��ς���.  cameraNo = 1:�^��, 2:�΂߉�, 3:��, ���̑�:�f�t�H���g,
void GameScreen::changeCameraPoint(const int cameraNo) {
	switch (cameraNo)
	{
	case 1: camera = D3DXVECTOR3(0, 4, -1); break;
	case 2: camera = D3DXVECTOR3(2, 3, -2); break;
	case 3: camera = D3DXVECTOR3(3, 1, 0); break;
	default: camera = D3DXVECTOR3(0, 3, -3); break;
	}

	D3DXMATRIX d3dm;
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
}

void GameScreen::switchScene(const int sceneId) {
	if (nonActiveScene != NULL) {
		printf("ERROR: nonActiveScene has not deleted.\n");
		return;
	}
	nonActiveScene = activeScene;	// �ꎞ�ێ����Ă����Č��(update())�������
	
	switch (sceneId) {
	case StartScene::SCENE_ID: activeScene = new StartScene(d3dDevice, this); return;
	case PlayScene::SCENE_ID:  activeScene = new PlayScene(d3dDevice, this); return;
	default:
		printf("ERROR:  scene id is not correct: %d\n", sceneId);
	}
	
}

=======

#include "../Header/stdafx.h"
#include "../Header/game_screen.h"
#include "../Header/utility.h"
#include "../Header/game_ui.h"

using namespace std;


bool GameScreen::initDXGraphics(HWND &hWnd) {

	D3DDISPLAYMODE dmode;	// ������̃f�B�X�v���C���[�h
	direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	// �\����D3DPRESENT_PARAMETERS�̒l��ݒ�
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = dmode.Format;
	d3dpp.BackBufferCount = 1;					// �_�u���A�g���v���o�b�t�@�Ƃ́H
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// �_�u���o�b�t�@�����O���̃X���b�v������w��
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	direct3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice);

	changeCameraPoint(0);
	activeScene = new StartScene(d3dDevice, this);

	successedInit = true;
	fps = dmode.RefreshRate;

	printf("dmode w, h, fRate, Format = %d, %d, %d, %d\n", dmode.Width, dmode.Height, dmode.RefreshRate, dmode.Format);
	printf("adapter count = %d\n", direct3D->GetAdapterCount());
	
	return true;
}

void GameScreen::initRender(int width, int height) {

	D3DXMATRIX d3dm;
	FLOAT aspect = (FLOAT)width / (FLOAT)height;

	// fovy:����p, aspect:�A�X�y�N�g��, zn:�ł��߂��ʂ�Z���W, zf:�ł������ʂ�Z���W
	D3DXMatrixPerspectiveFovLH(&d3dm, (FLOAT)D3DXToRadian(45.0), aspect, 1, 1000);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dm);

	// pEye:�J�����̈ʒu, pAt:�J���������ڂ���œ_, pUp:���[���h���W�`�̃J�����̏㕔
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
	
	d3dDevice->LightEnable(0, TRUE);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// �J�����O���[�h: �w�ʂ̏�����r������
	d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff808080);		// �A���r�G���g���C�g

}

void GameScreen::changeWindowSize(int width, int height) {
	if (!d3dDevice) return;

	activeScene->onLostDevice();

	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dDevice->Reset(&d3dpp);
	printf("changeWindowSize: w,h = %4d, %4d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
	initRender(width, height);

	activeScene->onLostDevice();
}



// �t���[�����X�V������ draw()���Ăяo��
void GameScreen::update() {
	updateCount++;
	if (activeScene == NULL) {
		MessageBox(NULL, "game scene is NULL", "ERROR", MB_OK);
	}
	if (nonActiveScene != NULL) {
		SAFE_DELETE(nonActiveScene);
	}
	activeScene->update(fps);
	draw();
}


void GameScreen::draw() {

	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x33, 0x33, 0x33), 1.0f, 0);
	d3dDevice->BeginScene();	// �o�b�t�@�ւ̕`��J�n

	activeScene->draw();
	
	d3dDevice->EndScene();		// �o�b�t�@�ւ̕`��I�� 
	d3dDevice->Present(NULL, NULL, NULL, NULL);
	//ValidateRect(hWnd, NULL);
}


void GameScreen::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		mouse_ldown = true;
		mouse_from.x = x;
		mouse_from.y = y;
		break;
	case WM_LBUTTONUP:
		mouse_ldown = false;
		break;
	case WM_RBUTTONDOWN: break;
	case WM_RBUTTONUP: break;
	case WM_MOUSEMOVE:
		if (mouse_ldown) {
			moveCamera(wp, x, y);
		}
		break;
	case WM_KEYUP:
		if (wp == '8') { switchScene(StartScene::SCENE_ID); }
		else if (wp == '9') { switchScene(PlayScene::SCENE_ID); }
		else if (wp >= '0' && wp <= '7') changeCameraPoint(wp - '0');
		break;
	}

	activeScene->receiveMouseInput(msg, wp, x, y);
}

void GameScreen::moveCamera(int wp, int mouse_x, int mouse_y) {
	D3DXMATRIX m;
	D3DXMATRIX	d3dm;

	if (abs(mouse_from.x - mouse_x) < abs(mouse_from.y - mouse_y)) {
		if ((wp & MK_SHIFT) || (wp & MK_CONTROL)) {
			camera *= (mouse_from.y - mouse_y > 0) ? 1.05f : 0.95f;
		}
		else {
			camera.y += (mouse_from.y - mouse_y > 0) ? 0.1f : -0.1f;
		}
	}
	else if(abs(mouse_from.x - mouse_x) > 1){
		D3DXMatrixRotationY(&d3dm, D3DXToRadian((mouse_from.x - mouse_x > 0) ? 3 : -3));
		camera = multiple(camera, d3dm);
	}

	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3( 0, 0, 0 ), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);

	mouse_from.x = mouse_x;
	mouse_from.y = mouse_y;
}

// �J�����̈ʒu��ς���.  cameraNo = 1:�^��, 2:�΂߉�, 3:��, ���̑�:�f�t�H���g,
void GameScreen::changeCameraPoint(const int cameraNo) {
	switch (cameraNo)
	{
	case 1: camera = D3DXVECTOR3(0, 4, -1); break;
	case 2: camera = D3DXVECTOR3(2, 3, -2); break;
	case 3: camera = D3DXVECTOR3(3, 1, 0); break;
	default: camera = D3DXVECTOR3(0, 3, -3); break;
	}

	D3DXMATRIX d3dm;
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
}

void GameScreen::switchScene(const int sceneId) {
	if (nonActiveScene != NULL) {
		printf("ERROR: nonActiveScene has not deleted.\n");
		return;
	}
	nonActiveScene = activeScene;	// �ꎞ�ێ����Ă����Č��(update())�������
	
	switch (sceneId) {
	case StartScene::SCENE_ID: activeScene = new StartScene(d3dDevice, this); return;
	case PlayScene::SCENE_ID:  activeScene = new PlayScene(d3dDevice, this); return;
	default:
		printf("ERROR:  scene id is not correct: %d\n", sceneId);
	}
	
}

>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
