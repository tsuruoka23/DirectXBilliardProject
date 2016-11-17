
#include "../Header/stdafx.h"
#include "../Header/game_component.h"
#include "../Header/utility.h"



void GameComponent::initGameObjects() {
	gameObjects = Billiardfacilitator::createBalls(d3dDevice);
	std::vector<GameObject*> table = Billiardfacilitator::createTable(d3dDevice);
	gameObjects.insert(gameObjects.end(), table.begin(), table.end());

	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Direction = D3DXVECTOR3(-1, -20, 0);
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 0.8f , 0.8f, 0.8f };
	light.Ambient = { 0.6f , 0.6f, 0.6f };
	light.Range = 1000;

}

void GameComponent::releseGameObjects() {
	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->release();
		//delete gameObjects[i];
	}
	gameObjects.clear();
}

void GameComponent::update(UINT fps) {

	updateCount++;
	//if (updateCount % 6 != 0)return;
	if (stopFrame && !nextFrame) return;
	nextFrame = false;

	light.Direction.x = 20 * sin(D3DXToRadian(updateCount));
	light.Direction.z = 20 * cos(D3DXToRadian(updateCount));
	d3dDevice->SetLight(0, &light);


	float energy = 0;
	Vector3 moment = { 0, 0, 0 };

	for (size_t i = 0; i < gameObjects.size(); i++) {
		//if (i < 15)gameObjects[i]->enable = false;
		gameObjects[i]->update(fps);
		if (gameObjects[i]->enable) {
			energy += gameObjects[i]->getKineticEnergy();
			moment += gameObjects[i]->getMomentum();
		}
	}

	for (size_t i = 0; i < gameObjects.size(); i++) {
		for (size_t j = i + 1; j < gameObjects.size(); j++) {
			GameObject::detectCollision(*gameObjects[i], *gameObjects[j]);
		}
	}

	if (updateCount % 20 == 0) {
		//printf("kE = %5.2f, moment = %s\n", energy, moment.toString().c_str());
	}
}

void GameComponent::draw() {
	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->draw(d3dDevice);
	}
}

void GameComponent::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	switch (msg)
	{
	case WM_KEYUP: 
		if (gameObjects.size() > 0) {
			float force = 1 * 60 * 0.3;
			if (wp == 'W')gameObjects[0]->addForce({ 0, 0, force });
			else if (wp == 'S')gameObjects[0]->addForce({ 0, 0, -force });
			else if (wp == 'A')gameObjects[0]->addForce({ -force,0, 0 });
			else if (wp == 'D')gameObjects[0]->addForce({ force,0, 0 });
		}
		if (wp == 'P') stopFrame = (stopFrame) ? false : true;
		else if (wp == 'L') nextFrame = true;
		else if (wp == 'I') {
			stopFrame = true;
			releseGameObjects();
			initGameObjects();
			stopFrame = false;
		}
		break;
	}
}
