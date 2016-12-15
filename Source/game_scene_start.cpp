#include "../Header/stdafx.h"
#include "../Header/game_scene.h"



void StartScene::onCreate() {

	ball = new MeshObject(BilliardBuilder::createBallMesh(d3dDevice, 0.5f));
	ball->loadTexture(d3dDevice, "Assets/Textures/Ball_01.png");

	ball->setAmbient({ 0.4f, 0.4f, 0.4f, 1.0f });
	ball->setDiffuse({ 0.9f, 0.9f, 0.9f, 1.0f });
	//ball->setPhysicalFeatures(new PhysicalFeaturs(0.1f, 0.1f, 0.0f, 0.0f, false));
	//ball->setCollider(new SphereCollider(0.3f, 0.1f, 0.1f, 1.0f));
	onReset();

	class ClickListener_Start: public GameUI::IClickListener {
	public:
		ClickListener_Start(void *obj) : IClickListener(obj) {}
		void click(GameUI* ui) {
			GameScene* scene = (GameScene*)callingObject;
			scene->getScreen()->switchScene(PlayScene::SCENE_ID);
		}
	};

	ButtonUI *btn_start = new ButtonUI(d3dDevice);
	btn_start->setClickListener(new ClickListener_Start(this));
	btn_start->loadTexture("Assets/UI/btn_start.png", { 0, 40, 512, 216 }, { 0, 296, 512, 472 });
	btn_start->setDrawingRect(50, 50, 562, 226);
	gameUIList.push_back(btn_start);
}

void StartScene::onReset() {
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Direction = D3DXVECTOR3(-1, -20, 0);
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 0.8f , 0.8f, 0.8f };
	light.Ambient = { 0.4f , 0.4f, 0.4f };
	light.Range = 1000;
}

void StartScene::onDestroy() {
	SAFE_DELETE(ball);
	for each(GameUI *ui in gameUIList) { SAFE_DELETE(ui); }
}

void StartScene::onLostDevice() {
	for each(GameUI *ui in gameUIList) { ui->onLostDevice(); }
}

void StartScene::onResetDevice() {
	for each(GameUI *ui in gameUIList) { ui->onResetDevice(); }
}

void StartScene::update(UINT fps) {
	updateCount++;

	light.Direction.x = 20 * sin(D3DXToRadian(updateCount));
	light.Direction.z = 20 * cos(D3DXToRadian(updateCount));
	d3dDevice->SetLight(0, &light);

	ball->update(fps);
}

void StartScene::draw() {
	ball->draw(d3dDevice);

	for each(GameUI *ui in gameUIList) {
		ui->draw();
	}
}

void StartScene::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	for each(GameUI *ui in gameUIList) {
		ui->receiveMouseInput(msg, wp, x, y);
	}
}