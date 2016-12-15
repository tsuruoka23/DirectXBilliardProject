<<<<<<< HEAD

#include "../Header/stdafx.h"
#include "../Header/game_screen.h"
#include "../Header/utility.h"
#include "../Header/game_ui.h"

using namespace std;


bool GameScreen::initDXGraphics(HWND &hWnd) {

	D3DDISPLAYMODE dmode;	// 動作環境のディスプレイモード
	direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	// 構造体D3DPRESENT_PARAMETERSの値を設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = dmode.Format;
	d3dpp.BackBufferCount = 1;					// ダブル、トリプルバッファとは？
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// ダブルバッファリング時のスワップ動作を指定
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

	// fovy:視野角, aspect:アスペクト比, zn:最も近い面のZ座標, zf:最も遠い面のZ座標
	D3DXMatrixPerspectiveFovLH(&d3dm, (FLOAT)D3DXToRadian(45.0), aspect, 1, 1000);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dm);

	// pEye:カメラの位置, pAt:カメラが注目する焦点, pUp:ワールド座標形のカメラの上部
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
	
	d3dDevice->LightEnable(0, TRUE);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// カリングモード: 背面の処理を排除する
	d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff808080);		// アンビエントライト

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



// フレームを更新した後 draw()を呼び出す
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
	d3dDevice->BeginScene();	// バッファへの描画開始

	activeScene->draw();
	
	d3dDevice->EndScene();		// バッファへの描画終了 
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

// カメラの位置を変える.  cameraNo = 1:真上, 2:斜め横, 3:横, その他:デフォルト,
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
	nonActiveScene = activeScene;	// 一時保持しておいて後で(update())解放する
	
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

	D3DDISPLAYMODE dmode;	// 動作環境のディスプレイモード
	direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	// 構造体D3DPRESENT_PARAMETERSの値を設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = dmode.Format;
	d3dpp.BackBufferCount = 1;					// ダブル、トリプルバッファとは？
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// ダブルバッファリング時のスワップ動作を指定
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

	// fovy:視野角, aspect:アスペクト比, zn:最も近い面のZ座標, zf:最も遠い面のZ座標
	D3DXMatrixPerspectiveFovLH(&d3dm, (FLOAT)D3DXToRadian(45.0), aspect, 1, 1000);
	d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dm);

	// pEye:カメラの位置, pAt:カメラが注目する焦点, pUp:ワールド座標形のカメラの上部
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	d3dDevice->SetTransform(D3DTS_VIEW, &d3dm);
	
	d3dDevice->LightEnable(0, TRUE);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// カリングモード: 背面の処理を排除する
	d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	d3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff808080);		// アンビエントライト

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



// フレームを更新した後 draw()を呼び出す
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
	d3dDevice->BeginScene();	// バッファへの描画開始

	activeScene->draw();
	
	d3dDevice->EndScene();		// バッファへの描画終了 
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

// カメラの位置を変える.  cameraNo = 1:真上, 2:斜め横, 3:横, その他:デフォルト,
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
	nonActiveScene = activeScene;	// 一時保持しておいて後で(update())解放する
	
	switch (sceneId) {
	case StartScene::SCENE_ID: activeScene = new StartScene(d3dDevice, this); return;
	case PlayScene::SCENE_ID:  activeScene = new PlayScene(d3dDevice, this); return;
	default:
		printf("ERROR:  scene id is not correct: %d\n", sceneId);
	}
	
}

>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
