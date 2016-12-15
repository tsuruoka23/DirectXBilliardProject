#pragma once
#include "stdafx.h"
#include "game_object.h"
#include "game_screen.h"
#include "game_ui.h"
#include "billiard.h"

class GameScreen;

class GameScene {
protected:
	IDirect3DDevice9 *d3dDevice;
	GameScreen *gameScreen;
	UINT updateCount;
	int sceneId;

public:
	GameScene(IDirect3DDevice9 *device, GameScreen *screen) {
		d3dDevice = device;
		gameScreen = screen;
	};
	virtual ~GameScene() {
		printf("GameScene> destruct\n");
	};

	virtual void onCreate() = 0;	// シーン生成時の処理
	virtual void onReset() = 0;	// シーンリセット時処理
	virtual void onDestroy() = 0;	// シーン破棄時処理
	virtual void onLostDevice() = 0;
	virtual void onResetDevice() = 0;

	virtual void update(UINT fps) = 0;
	virtual void draw() = 0;
	virtual void  receiveMouseInput(UINT msg, WPARAM wp, int x, int y) = 0;

	GameScreen* getScreen() { return gameScreen; };
	int getSceneId() { return sceneId; }
};


// スタート画面. ボタン以外特にない
class StartScene : public GameScene{
public:
	static const int SCENE_ID = 0x0001;
private:
	D3DLIGHT9 light;
	MeshObject* ball;
	std::vector<GameUI*> gameUIList;

public:
	StartScene(IDirect3DDevice9 *device, GameScreen *screen) : GameScene(device, screen) {
		onCreate();
	};
	~StartScene() {
		onDestroy();
		printf("StartScene> destruct\n");
	};

	void onCreate();
	void onReset();
	void onDestroy();
	void onLostDevice();
	void onResetDevice();

	void update(UINT fps);
	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

};

// ビリヤードをプレイするシーン
class PlayScene: public GameScene {
private:
	class ResultMenu {
	private: 
		PlayScene *playScene;
		GameUI* uiList[3];	// 全てのUIを一括管理するための配列
		ButtonUI *btn_restart;
		ButtonUI *btn_end;
		TextUI *txt_msg;
		bool enable = true;
	public:
		ResultMenu(PlayScene *scene);
		~ResultMenu();
		void setEnable(bool is) { enable = is; }
		bool getEnable() { return enable; }
		void show();
		void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);
		void onLostDevice()  { for each(GameUI* ui in uiList) { ui->onLostDevice(); } };
		void onResetDevice() { for each(GameUI* ui in uiList) { ui->onResetDevice(); } };
	private:
		//void makeResultMenue();
	};
public:
	static const int SCENE_ID = 0x0002;
private:
	static const int BALL_LOWER_LIMIT_Y = -1;	// 球のy座標がこれ以下になったらポケットインしているとする
	D3DLIGHT9 light;
	ResultMenu resultMenu;

	std::vector<GameUI*> gameUIList;
	std::vector<GameObject*> gameObjects;
	std::vector<MeshObject*> balls;	// テーブルの上に存在する球
	std::vector<MeshObject*> table;	// テーブル（壁含む）
	MeshObject* cueBall;	// 手玉
	Cue* cue;				// キュー
	TextUI *txt_score;

	bool stopFrame = false;	// デバッグ用, フレーム単位で描画を確認するため
	bool nextFrame = false;
	int inPocketCount;		// ポケットに入ったボールの数
	
	enum GameState { ReadyShot, BallMoving, GameOver };
	GameState gameState;

public:
	PlayScene(IDirect3DDevice9 *device, GameScreen *screen) : GameScene(device, screen), resultMenu(this){
		onCreate();
	};
	~PlayScene() {
		onDestroy();
		printf("PlayScene> destruct\n");
	};

	void onCreate();
	void onReset();
	void onDestroy();
	void onLostDevice();
	void onResetDevice();

	void update(UINT fps);
	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

};