#include "../Header/game.h"

using namespace std;

void showMatrix(D3DXMATRIX & m) {
	printf("matrix\n"
		"%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n%5.3f, %5.3f, %5.3f, %5.3f\n",
		m._11, m._12, m._13, m._14, m._21, m._22, m._23, m._24, m._31, m._32, m._33, m._34, m._41, m._42, m._43, m._44);
	//cout << "matrix = " << m << endl;
};



bool initDXGraphics(){


	// おっぱいテクスチャ読み込み
	char* file = "Resource\\meg.jpg";
	D3DXCreateSprite(pD3Device, &g_pSprite); 
	if (FAILED(D3DXCreateTextureFromFile(pD3Device, file, &g_pTexture)) ){
		MessageBox(NULL, file, "Texture Load Error", MB_OK);
	}

	// メッシュ読み込み
	mesh_Cube = new MeshObject();
	mesh_Tpot = new MeshObject();

	if (!mesh_Cube->loadFromX(pD3Device, "Resource\\Cube2.x")) return false;
	
	ID3DXMesh *mesh;
	if (FAILED(D3DXCreateTeapot(pD3Device, &mesh, NULL))) {
		MessageBox(NULL, "ティーポット読み込みエラー", "ERROR", MB_OK); return false;
	}
	mesh_Tpot->setMesh(mesh);

	
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Direction = D3DXVECTOR3(-1, -20, 0);
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = { 1.0f , 1.0f, 1.0f };
	light.Ambient = { 0.6f , 0.6f, 0.6f };
	light.Range = 1000;

	successLord = true;
	return true;
	//std::printf("dmode w, h, fRate, Format = %d, %d, %d, %d\n", dmode.Width, dmode.Height, dmode.RefreshRate, dmode.Format);
	//std::printf("adapter count = %d\n", pDirect3D->GetAdapterCount());
}


// ppD3Device->Reset(&d3dpp)について学ぶ必要がある
void GameController::changeWindowSize(int width, int height){
	if (!pD3Device) return;
	if (g_pSprite) g_pSprite->OnLostDevice();
	
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	pD3Device->Reset(&d3dpp);
	printf(" w,h = %4d, %4d\n", d3dpp.BackBufferWidth, d3dpp.BackBufferHeight);
	SetRender(width, height);

	if (g_pSprite) g_pSprite->OnResetDevice();
}

void GameController::SetRender(int width, int height) {
	D3DXMATRIX d3dm;

	// pEye:カメラの位置, pAt:カメラが注目する焦点, pUp:ワールド座標形のカメラの上部
	camera = D3DXVECTOR3(0, 0, 50);
	D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	pD3Device->SetTransform(D3DTS_VIEW, &d3dm);

	// fovy:視野角, aspect:アスペクト比, zn:最も近い面のZ座標, zf:最も遠い面のZ座標
	FLOAT aspect = (FLOAT)width / (FLOAT)height;
	D3DXMatrixPerspectiveFovLH(&d3dm, (FLOAT)D3DXToRadian(45.0), aspect, 1, 1000);
	pD3Device->SetTransform(D3DTS_PROJECTION, &d3dm);

	pD3Device->LightEnable(0, true);
	pD3Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// カリングモード: 背面の処理を排除する
	pD3Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	pD3Device->SetRenderState(D3DRS_AMBIENT, 0x00FF8080);   // アンビエントライト
}

// フレームを更新した後 draw()を呼び出す
void GameController::update() {
	updateCount ++;

	/*
	D3DXMATRIX      matWorld, matWork;
	D3DXMatrixRotationZ(&matWorld, PI * updateCount / 120);
	D3DXMatrixTranslation(&matWork, 400.0f, 300.0f, 0.0f);
	matWorld = matWorld * matWork;
	g_pSprite->SetTransform(&matWorld);
	*/

	D3DXMATRIX Offset;
	D3DXMatrixScaling(&Offset, 5, 5, 5);
	mesh_Tpot->setDiffuse(D3DCOLORVALUE{ 0.0f, 0.9f, 0.9f });
	mesh_Tpot->setAmbient(D3DCOLORVALUE{ 0.3f, 0.3f, 0.3f });
	mesh_Tpot->setTrasrateMatrix(Offset);

	light.Direction.x = 20 * sin(D3DXToRadian(updateCount));
	light.Direction.z = 20 * cos(D3DXToRadian(updateCount));
	pD3Device->SetLight(0, &light);
	draw();
}


void GameController::draw() {
	static WORD index[] = { 0, 1, 2, 3, 2, 1, 4, 1, 2};

	pD3Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0x33, 0x33, 0x33), 1.0, 0);

	// --------------- バッファへの描画開始 -------------------- //
	pD3Device->BeginScene();

	///*
	//pD3Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	//pD3Device->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 5, 3, index, D3DFMT_INDEX16, pt1, sizeof(*pt1));

	//pD3Device->SetTexture(0, g_pTexture);
	//pD3Device->SetMaterial(&mate);
	//pD3Device->SetFVF(MeshObject::FVF);
	//pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pt2, sizeof(*pt2));

	/*
	// おっぱいスプラトの描画 
	g_pSprite->Begin(0); 
	g_pSprite->Draw(g_pTexture, NULL, NULL, NULL, 0xffffffff); 
	g_pSprite->End();
	*/

	mesh_Cube->draw(pD3Device);
	mesh_Tpot->draw(pD3Device);


	pD3Device->EndScene();	
	// --------------- バッファへの描画終了 -------------------- //

	pD3Device->Present(NULL, NULL, NULL, NULL);
	//ValidateRect(hWnd, NULL);
}


void GameController::receiveMouseInput(MouseInput input, int x, int y) {
	//cout << "mouse input: " << input << endl;
	D3DXMATRIX m;
	D3DXVECTOR3 d3dv;

	switch (input)
	{
	case GameController::LDown:
		blDown = true;
		from.x = x;
		from.y = y;
		break;
	case GameController::LUp:
		blDown = false;
		D3DXMatrixTranslation(&m, 2, 5, 0);
		break;
	case GameController::RDown:
		break;
	case GameController::RUp:
		break;
	case GameController::Move:

		if (!blDown) return;
		if (abs(from.x - x) < abs(from.y - y)) {
			D3DXMatrixRotationX(&d3dm, D3DXToRadian(from.y - y > 0 ? 5 : -5));
		}
		else{
			D3DXMatrixRotationY(&d3dm, D3DXToRadian(from.x - x > 0 ? 5 : -5));
		}

		d3dv = camera;
		camera.x = d3dv.x * d3dm.m[0][0] + d3dv.y * d3dm.m[1][0] + d3dv.z * d3dm.m[2][0];
		camera.y = d3dv.x * d3dm.m[0][1] + d3dv.y * d3dm.m[1][1] + d3dv.z * d3dm.m[2][1];
		camera.z = d3dv.x * d3dm.m[0][2] + d3dv.y * d3dm.m[1][2] + d3dv.z * d3dm.m[2][2];
		//camera.y += from.y - y;

		D3DXMatrixLookAtLH(&d3dm, &camera, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
		pD3Device->SetTransform(D3DTS_VIEW, &d3dm);

		from.x = x;
		from.y = y;
		break;
	}
}
