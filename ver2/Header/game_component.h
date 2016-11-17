#pragma once

#include "stdafx.h"
#include "game_object.h"
#include "billiard.h"

// ゲーム内に存在するオブジェクトを管理するクラス
class GameComponent {
private :
	IDirect3DDevice9 *d3dDevice;

	D3DLIGHT9 light;
	std::vector<GameObject*> gameObjects;

	UINT updateCount;

	bool stopFrame = false;
	bool nextFrame = false;

public:
	GameComponent(IDirect3DDevice9 *device) {
		this->d3dDevice = device;
		updateCount = 0;
	}
	~GameComponent() {}

	void initGameObjects();
	void releseGameObjects();
	void update(UINT fps);
	void draw();

	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);
};