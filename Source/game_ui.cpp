#include "../Header/stdafx.h"
#include "..\Header\game_ui.h"


// GameUI -------------------------------------------------------------------------------------------

UINT32 GameUI::ID = 0;

void GameUI::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("GameUI> msg = %d, wp = %d, (x, y) = (%d, %d)  press = %d\n", msg, wp, x, y, isPressed);
	if (!enable) return;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
		RECT r = drawingRect;
		isPressed = (x > r.left && y > r.top && x < r.right && y < r.bottom);	// 描画範囲内ならture
		break;
	}
	case WM_LBUTTONUP:
		if (isPressed && clickListener != NULL) {
			clickListener->click(this);
		}
		isPressed = false;
		break;
	}
}



// TextUI -------------------------------------------------------------------------------------------

void TextUI::init(const int fontSize, const bool italic, char * text, RECT drawRect) {
	setFont(fontSize,  false);
	setText(text);
	setDrawingRect(drawRect);
}

void TextUI::setFont(const int fontSize, const bool italic) {
	UINT w = fontSize;
	UINT h = fontSize * 2;
	D3DXCreateFont( d3dDevice, h, w, 0, 0, italic, 0, 0, 0, 0, NULL, &font);
}


void TextUI::draw() {
	if (sprite == NULL || !enable) return;
	font->DrawText(NULL, text, -1, &drawingRect, NULL, 0xFFAA5522);
}



// ButtonUI ------------------------------------------------------------------------------------------ 

// デフォルトおよび押されたときの画像を格納したテクスチャファイルを指定する. それぞれの描画範囲はRECTで指定する.
void ButtonUI::loadTexture(const char * fname, const RECT &defaultRect, const RECT &pressedRect) {
	if (FAILED(D3DXCreateTextureFromFile(d3dDevice, fname, &texture))) {
		MessageBox(NULL, fname, "Texture Load Error", MB_OK);
	}
	texture->GetLevelDesc(0, &surface);
	drawingRect_default = defaultRect;
	drawingRect_pressed = pressedRect;
	drawingRect = { 0, 0, defaultRect.right - defaultRect.left, defaultRect.bottom - defaultRect.top };
	//printf("texture> w = %d, h = %d\n", surface.Width, surface.Height);
}

void ButtonUI::draw() {
	if (!enable) return;

	RECT srcRect = (isPressed) ? drawingRect_pressed : drawingRect_default;
	D3DXMATRIX scale, traslation;

	float sx = (float)(drawingRect.right - drawingRect.left) / (srcRect.right - srcRect.left);
	float sy = (float)(drawingRect.bottom - drawingRect.top) / (srcRect.bottom - srcRect.top);
	D3DXMatrixScaling(&scale, sx, sy, 0);
	D3DXMatrixTranslation(&traslation, (float)drawingRect.left, (float)drawingRect.top, 0);

	sprite->SetTransform(& (scale * traslation));
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(texture, &srcRect, NULL, NULL, 0xffffffff);
	sprite->End();
}

void ButtonUI::receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {
	//printf("Button> msg = %d, wp = %d, (x, y) = (%d, %d)\n", msg, wp, x, y);
	if (!enable) return;
	GameUI::receiveMouseInput(msg, wp, x, y);

	switch (msg)
	{
	case WM_LBUTTONDOWN: break;
	case WM_LBUTTONUP:   break;
	case WM_RBUTTONDOWN: break;
	case WM_RBUTTONUP: break;
	case WM_MOUSEMOVE: break;
	case WM_KEYUP: break;
	}
	
}

