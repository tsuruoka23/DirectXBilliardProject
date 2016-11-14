#pragma once
#include "stdafx.h"
#include "game_object.h"
#include "billiard.h"
#include "game_component.h"

#define PI 3.14


// ゲーム画面の更新や描画を行うクラス
// デバイスを管理する
class GameScreen {

private:
	IDirect3D9		 *direct3D;
	IDirect3DDevice9 *d3dDevice;
	D3DPRESENT_PARAMETERS d3dpp; // デバイスの状態を示す構造体

	D3DXVECTOR3 camera;
	GameComponent *gameComponent;
	
	UINT32 updateCount;	// updateが呼び出された回数
	UINT32 fps;
	bool successedInit;

	POINT mouse_from;			// カメラ向きの操作を行うための変数
	bool  mouse_ldown = false;	// マウスのLボタンが押されているかどうか

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