<<<<<<< HEAD
#include "../Header/stdafx.h"
#include "../Header/game_scene.h"

using namespace std;


// PlayScene::ResultMenue ----------------------------------------------------------------------

PlayScene::ResultMenu::ResultMenu(PlayScene *scene) : playScene(scene) {

	class ClickListener_Start: public GameUI::IClickListener {
	public:
		ClickListener_Start(void *obj) : IClickListener(obj) {}
		void click(GameUI* ui) {
			GameScene* scene = (GameScene*)callingObject;
			switch (ui->getId()){
			case 0x0100: scene->onReset(); break;
			case 0x0200: scene->getScreen()->switchScene(StartScene::SCENE_ID); break;
			default:
				break;
			}
			scene->onReset();
		}
	};

	btn_restart = new ButtonUI(scene->d3dDevice);
	btn_restart->setClickListener(new ClickListener_Start(scene));
	btn_restart->loadTexture("Assets/UI/btn_restart.png", { 0, 40, 512, 216 }, { 0, 296, 512, 472 });
	btn_restart->setDrawingRect( 50, 150, 562, 326);
	btn_restart->setId(0x0100);
	
	btn_end = new ButtonUI(scene->d3dDevice);
	btn_end->setClickListener(new ClickListener_Start(scene));
	btn_end->loadTexture("Assets/UI/btn_end.png", { 0, 40, 512, 216 }, { 0, 296, 512, 472 });
	btn_end->setDrawingRect(50, 350, 562, 526);
	btn_end->setId(0x0200);

	txt_msg = new TextUI(scene->d3dDevice);
	txt_msg->init(60, false, "GAME OVER", { 50, 10, 1000, 200 });


	uiList[0] = btn_restart;
	uiList[1] = btn_end;
	uiList[2] = txt_msg;
}

PlayScene::ResultMenu::~ResultMenu() {
	for each(GameUI* ui in uiList) { SAFE_DELETE(ui); }
}

void PlayScene::ResultMenu::show() {
	if (!enable)return;
	for each(GameUI* ui in uiList) { ui->draw(); }
}

void PlayScene::ResultMenu::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	if (!enable)return;
	btn_restart->receiveMouseInput(msg, wp, x, y);
	btn_end->receiveMouseInput(msg, wp, x, y);
}


// PlayScene -----------------------------------------------------------------------------------

void PlayScene::onCreate() {
	balls = BilliardBuilder::createBalls(d3dDevice);
	table = BilliardBuilder::createTable(d3dDevice);
	cue = BilliardBuilder::createCue(d3dDevice);
	cueBall = balls[0];

	gameObjects.insert(gameObjects.end(), balls.begin(), balls.end());
	gameObjects.insert(gameObjects.end(), table.begin(), table.end());
	gameObjects.push_back(cue);

	txt_score = new TextUI(d3dDevice);
	txt_score->init(30, false, "", { 0, 100, 500, 200 });

	onReset();
}

void PlayScene::onReset() {
	stopFrame = true;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Direction = D3DXVECTOR3(-1, -20, 0);
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 0.8f , 0.8f, 0.8f };
	light.Ambient = { 0.4f , 0.4f, 0.4f };
	light.Range = 1000;

	for each(GameObject *obj in gameObjects) {
		obj->resetPhysics();
		obj->setEnable(true);
	}
	BilliardBuilder::initBallPosition(balls, 1);
	resultMenu.setEnable(false); 
	inPocketCount = 0;
	gameState = ReadyShot;
	gameScreen->changeCameraPoint(0);
	stopFrame = false;
}

void PlayScene::onDestroy() {
	BilliardBuilder::deleteObjects(balls, table);
	cue->release();
	SAFE_DELETE(cue);
	SAFE_DELETE(txt_score);
}

void PlayScene::onLostDevice() {
	resultMenu.onLostDevice();
	txt_score->onLostDevice();
}

void PlayScene::onResetDevice() {
	resultMenu.onResetDevice();
	txt_score->onResetDevice();
}

void PlayScene::update(UINT fps) {
	updateCount++;
	if (stopFrame && !nextFrame) return;
	nextFrame = false;

	bool isBallRemain = false;	// 的球以外で球がテーブルに残っているか
	float energy = 0;
	Vector3 moment = { 0, 0, 0 };

	d3dDevice->SetLight(0, &light);

	for (int i = balls.size() - 1; i >= 0; i--) {
		MeshObject* ball = balls[i];
		if (ball->getEnable()) {
			if (ball->getLocation().y < BALL_LOWER_LIMIT_Y) {
				ball->setEnable(false);
				if (i == 0) {
					printf("的球が落ちました！\n");
					gameState = GameOver;
				}
				else {
					printf("%d番の球が落ちました！計:%d\n", i, ++inPocketCount);
				}
			}
			else if(i != 0){
				isBallRemain = true;
			}
		}
	}
	if (!isBallRemain && gameState != GameOver) {
		printf("全ての球を落としました！\n");
		gameState = GameOver;
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->update(fps);
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (!gameObjects[i]->getEnable())continue;
		for (size_t j = i + 1; j < gameObjects.size(); j++) {
			if (!gameObjects[j]->getEnable())continue;
			GameObject::detectCollision(*gameObjects[i], *gameObjects[j]);
		}
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->getEnable()) {
			energy += gameObjects[i]->getKineticEnergy();
			moment += gameObjects[i]->getMomentum();
		}
	}
	
	switch (gameState){
	case PlayScene::ReadyShot:
		cue->setEnable(true);
		cue->calcLocation(*cueBall);
		if (energy > 0) gameState = BallMoving;
		break;
	case PlayScene::BallMoving:
		cue->setEnable(false);
		if (energy == 0) gameState = ReadyShot;
		break;
	case PlayScene::GameOver:
		resultMenu.setEnable(true);
		break;
	}

	if (updateCount % 60 == 1) {
		//printf("kE = %6.3f, moment = %s\n", energy, moment.toString().c_str());
	}
}

void PlayScene::draw() {
	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->draw(d3dDevice);
	}
	txt_score->draw();
	resultMenu.show();
}

void PlayScene::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("PlayScene> msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	resultMenu.receiveMouseInput(msg, wp, x, y);

	switch (msg){
	case WM_KEYDOWN:
		// 方向キーでキューの向きを変更. スペースでキューを引く
		if (gameState == ReadyShot) {
			if (wp == VK_SPACE) { cue->pull(); }
			
			Vector3 r = {0, 0 , 0};
			if (wp == VK_LEFT)  { r = { 0,  (float)PI / 72, 0 }; }
			if (wp == VK_RIGHT) { r = { 0, -(float)PI / 72, 0 }; }
			if (wp == VK_UP || wp == VK_DOWN) {
				cue->setRotationMatrix(Matrix::IdentityMatrix());
				if (wp == VK_UP)   { r = { 0,  (float)PI, 0 }; };
			}
			Matrix m = cue->getRocationMatrix() * Matrix::makeRotationMatrixXYZ(r);
			cue->setRotationMatrix(m);
		}

		break;
	case WM_KEYUP:
		if (wp == VK_SPACE && gameState == ReadyShot) {
			cueBall->addForce(cue->shot() * (float)gameScreen->getFps());
		}
		// デバッグ用コマンド
		if (wp == 'M') { resultMenu.setEnable(!resultMenu.getEnable()); }
		if (wp == 'P') { stopFrame = (stopFrame) ? false : true; }
		if (wp == 'L') { nextFrame = true; }
		if (wp == 'I') { onReset(); }
		break;
	}
}
=======
#include "../Header/stdafx.h"
#include "../Header/game_scene.h"

using namespace std;


// PlayScene::ResultMenue ----------------------------------------------------------------------

PlayScene::ResultMenu::ResultMenu(PlayScene *scene) : playScene(scene) {

	class ClickListener_Start: public GameUI::IClickListener {
	public:
		ClickListener_Start(void *obj) : IClickListener(obj) {}
		void click(GameUI* ui) {
			GameScene* scene = (GameScene*)callingObject;
			switch (ui->getId()){
			case 0x0100: scene->onReset(); break;
			case 0x0200: scene->getScreen()->switchScene(StartScene::SCENE_ID); break;
			default:
				break;
			}
			scene->onReset();
		}
	};

	btn_restart = new ButtonUI(scene->d3dDevice);
	btn_restart->setClickListener(new ClickListener_Start(scene));
	btn_restart->loadTexture("Assets/UI/btn_restart.png", { 0, 40, 512, 216 }, { 0, 296, 512, 472 });
	btn_restart->setDrawingRect( 50, 150, 562, 326);
	btn_restart->setId(0x0100);
	
	btn_end = new ButtonUI(scene->d3dDevice);
	btn_end->setClickListener(new ClickListener_Start(scene));
	btn_end->loadTexture("Assets/UI/btn_end.png", { 0, 40, 512, 216 }, { 0, 296, 512, 472 });
	btn_end->setDrawingRect(50, 350, 562, 526);
	btn_end->setId(0x0200);

	txt_msg = new TextUI(scene->d3dDevice);
	txt_msg->init(60, false, "GAME OVER", { 50, 10, 1000, 200 });


	uiList[0] = btn_restart;
	uiList[1] = btn_end;
	uiList[2] = txt_msg;
}

PlayScene::ResultMenu::~ResultMenu() {
	for each(GameUI* ui in uiList) { SAFE_DELETE(ui); }
}

void PlayScene::ResultMenu::show() {
	if (!enable)return;
	for each(GameUI* ui in uiList) { ui->draw(); }
}

void PlayScene::ResultMenu::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	if (!enable)return;
	btn_restart->receiveMouseInput(msg, wp, x, y);
	btn_end->receiveMouseInput(msg, wp, x, y);
}


// PlayScene -----------------------------------------------------------------------------------

void PlayScene::onCreate() {
	balls = BilliardBuilder::createBalls(d3dDevice);
	table = BilliardBuilder::createTable(d3dDevice);
	cue = BilliardBuilder::createCue(d3dDevice);
	cueBall = balls[0];

	gameObjects.insert(gameObjects.end(), balls.begin(), balls.end());
	gameObjects.insert(gameObjects.end(), table.begin(), table.end());
	gameObjects.push_back(cue);

	txt_score = new TextUI(d3dDevice);
	txt_score->init(30, false, "", { 0, 100, 500, 200 });

	onReset();
}

void PlayScene::onReset() {
	stopFrame = true;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Direction = D3DXVECTOR3(-1, -20, 0);
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 0.8f , 0.8f, 0.8f };
	light.Ambient = { 0.4f , 0.4f, 0.4f };
	light.Range = 1000;

	for each(GameObject *obj in gameObjects) {
		obj->resetPhysics();
		obj->setEnable(true);
	}
	BilliardBuilder::initBallPosition(balls, 1);
	resultMenu.setEnable(false); 
	inPocketCount = 0;
	gameState = ReadyShot;
	gameScreen->changeCameraPoint(0);
	stopFrame = false;
}

void PlayScene::onDestroy() {
	BilliardBuilder::deleteObjects(balls, table);
	cue->release();
	SAFE_DELETE(cue);
	SAFE_DELETE(txt_score);
}

void PlayScene::onLostDevice() {
	resultMenu.onLostDevice();
	txt_score->onLostDevice();
}

void PlayScene::onResetDevice() {
	resultMenu.onResetDevice();
	txt_score->onResetDevice();
}

void PlayScene::update(UINT fps) {
	updateCount++;
	if (stopFrame && !nextFrame) return;
	nextFrame = false;

	bool isBallRemain = false;	// 的球以外で球がテーブルに残っているか
	float energy = 0;
	Vector3 moment = { 0, 0, 0 };

	d3dDevice->SetLight(0, &light);

	for (int i = balls.size() - 1; i >= 0; i--) {
		MeshObject* ball = balls[i];
		if (ball->getEnable()) {
			if (ball->getLocation().y < BALL_LOWER_LIMIT_Y) {
				ball->setEnable(false);
				if (i == 0) {
					printf("的球が落ちました！\n");
					gameState = GameOver;
				}
				else {
					printf("%d番の球が落ちました！計:%d\n", i, ++inPocketCount);
				}
			}
			else if(i != 0){
				isBallRemain = true;
			}
		}
	}
	if (!isBallRemain && gameState != GameOver) {
		printf("全ての球を落としました！\n");
		gameState = GameOver;
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->update(fps);
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (!gameObjects[i]->getEnable())continue;
		for (size_t j = i + 1; j < gameObjects.size(); j++) {
			if (!gameObjects[j]->getEnable())continue;
			GameObject::detectCollision(*gameObjects[i], *gameObjects[j]);
		}
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->getEnable()) {
			energy += gameObjects[i]->getKineticEnergy();
			moment += gameObjects[i]->getMomentum();
		}
	}
	
	switch (gameState){
	case PlayScene::ReadyShot:
		cue->setEnable(true);
		cue->calcLocation(*cueBall);
		if (energy > 0) gameState = BallMoving;
		break;
	case PlayScene::BallMoving:
		cue->setEnable(false);
		if (energy == 0) gameState = ReadyShot;
		break;
	case PlayScene::GameOver:
		resultMenu.setEnable(true);
		break;
	}

	if (updateCount % 60 == 1) {
		//printf("kE = %6.3f, moment = %s\n", energy, moment.toString().c_str());
	}
}

void PlayScene::draw() {
	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->draw(d3dDevice);
	}
	txt_score->draw();
	resultMenu.show();
}

void PlayScene::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("PlayScene> msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	resultMenu.receiveMouseInput(msg, wp, x, y);

	switch (msg){
	case WM_KEYDOWN:
		// 方向キーでキューの向きを変更. スペースでキューを引く
		if (gameState == ReadyShot) {
			if (wp == VK_SPACE) { cue->pull(); }
			
			Vector3 r = {0, 0 , 0};
			if (wp == VK_LEFT)  { r = { 0,  (float)PI / 72, 0 }; }
			if (wp == VK_RIGHT) { r = { 0, -(float)PI / 72, 0 }; }
			if (wp == VK_UP || wp == VK_DOWN) {
				cue->setRotationMatrix(Matrix::IdentityMatrix());
				if (wp == VK_UP)   { r = { 0,  (float)PI, 0 }; };
			}
			Matrix m = cue->getRocationMatrix() * Matrix::makeRotationMatrixXYZ(r);
			cue->setRotationMatrix(m);
		}

		break;
	case WM_KEYUP:
		if (wp == VK_SPACE && gameState == ReadyShot) {
			cueBall->addForce(cue->shot() * (float)gameScreen->getFps());
		}
		// デバッグ用コマンド
		if (wp == 'M') { resultMenu.setEnable(!resultMenu.getEnable()); }
		if (wp == 'P') { stopFrame = (stopFrame) ? false : true; }
		if (wp == 'L') { nextFrame = true; }
		if (wp == 'I') { onReset(); }
		break;
	}
}
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
