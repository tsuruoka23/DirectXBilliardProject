<<<<<<< HEAD
#pragma once
#include "stdafx.h"
#include "d3dmath.h"

class GameUI {
public:
	class IClickListener {
	protected:
		void* callingObject;
	public:
		IClickListener(void *obj) : callingObject(obj) {}
		virtual ~IClickListener() {}
		virtual void click(GameUI* gameUI) = 0;
	};

private:
	static UINT32 ID;
protected:
	IDirect3DDevice9 *d3dDevice;
	ID3DXSprite *sprite = NULL;
	RECT drawingRect;		// 描画位置＆範囲
	
	IClickListener *clickListener;
	UINT32 id;
	bool isPressed = false;
	bool enable = true;

public:	
	GameUI(IDirect3DDevice9* d3dDevice) {
		this->d3dDevice = d3dDevice;
		D3DXCreateSprite(d3dDevice, &sprite);
		drawingRect = { 0, 0, 0, 0 };
		id = ID++;
	};
	virtual ~GameUI() {
		SAFE_RELEASE(sprite);
		SAFE_DELETE(clickListener);
	};

	void setId(const UINT32 id) { this->id = id; }
	void setEnable(const bool is) { enable = is; }
	UINT32 getId()const { return id; }
	bool getEnable() const { return enable; }

	virtual void draw() = 0;
	virtual void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

	virtual void onLostDevice()  const { sprite->OnLostDevice(); }
	virtual void onResetDevice() const { sprite->OnResetDevice(); }

	void setDrawingPoint(const LONG x, const LONG y, const LONG z = 0) { setDrawingRect({ x, y, x + drawingRect.right, y + drawingRect.bottom}); }
	void setDrawingPoint(const Vector3 &p) { setDrawingPoint((LONG)p.x, (LONG)p.y, (LONG)p.z); }
	void setDrawingRect(const LONG left, const LONG top, const LONG right, const LONG bottom) { drawingRect = { left, top, right, bottom }; }
	void setDrawingRect(const RECT &rect) { drawingRect = rect; }
	void setClickListener(IClickListener *listener) { clickListener = listener; }
	

protected:
};


class TextUI: public GameUI {
protected:
	ID3DXFont *font;
	LPCSTR text;

public:
	TextUI(IDirect3DDevice9* device) : GameUI(device) {
		font = NULL;
		text = "";
	};
	~TextUI() { 
		SAFE_RELEASE(font); 
	};

	void init(const int fontSize, const bool italic = false, char* text = "", RECT drawRect = { 0, 0, 0, 0 });
	void setFont(const int fontSize, const bool italic = false);
	void setText(LPCSTR text) { this->text = text; };

	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {};
	void onLostDevice()  const { font->OnLostDevice(); }
	void onResetDevice() const { font->OnResetDevice(); }

protected:
};

class ButtonUI: public GameUI {
protected:
	IDirect3DTexture9 *texture = NULL;	// 上半分がデフォルト、下半分が押されたときの画像を格納したテクスチャ
	RECT drawingRect_default;
	RECT drawingRect_pressed;
	D3DSURFACE_DESC surface;

public:
	ButtonUI(IDirect3DDevice9* device) : GameUI(device) {}
	~ButtonUI() {
		SAFE_RELEASE(texture);
	}

	void loadTexture(const char* fname, const RECT &defaultRect, const RECT &pressedRect);
	//void loadTexture(const char* fname1, const char* fname2);
	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);
=======
#pragma once
#include "stdafx.h"
#include "d3dmath.h"

class GameUI {
public:
	class IClickListener {
	protected:
		void* callingObject;
	public:
		IClickListener(void *obj) : callingObject(obj) {}
		virtual ~IClickListener() {}
		virtual void click(GameUI* gameUI) = 0;
	};

private:
	static UINT32 ID;
protected:
	IDirect3DDevice9 *d3dDevice;
	ID3DXSprite *sprite = NULL;
	RECT drawingRect;		// 描画位置＆範囲
	
	IClickListener *clickListener;
	UINT32 id;
	bool isPressed = false;
	bool enable = true;

public:	
	GameUI(IDirect3DDevice9* d3dDevice) {
		this->d3dDevice = d3dDevice;
		D3DXCreateSprite(d3dDevice, &sprite);
		drawingRect = { 0, 0, 0, 0 };
		id = ID++;
	};
	virtual ~GameUI() {
		SAFE_RELEASE(sprite);
		SAFE_DELETE(clickListener);
	};

	void setId(const UINT32 id) { this->id = id; }
	void setEnable(const bool is) { enable = is; }
	UINT32 getId()const { return id; }
	bool getEnable() const { return enable; }

	virtual void draw() = 0;
	virtual void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);

	virtual void onLostDevice()  const { sprite->OnLostDevice(); }
	virtual void onResetDevice() const { sprite->OnResetDevice(); }

	void setDrawingPoint(const LONG x, const LONG y, const LONG z = 0) { setDrawingRect({ x, y, x + drawingRect.right, y + drawingRect.bottom}); }
	void setDrawingPoint(const Vector3 &p) { setDrawingPoint((LONG)p.x, (LONG)p.y, (LONG)p.z); }
	void setDrawingRect(const LONG left, const LONG top, const LONG right, const LONG bottom) { drawingRect = { left, top, right, bottom }; }
	void setDrawingRect(const RECT &rect) { drawingRect = rect; }
	void setClickListener(IClickListener *listener) { clickListener = listener; }
	

protected:
};


class TextUI: public GameUI {
protected:
	ID3DXFont *font;
	LPCSTR text;

public:
	TextUI(IDirect3DDevice9* device) : GameUI(device) {
		font = NULL;
		text = "";
	};
	~TextUI() { 
		SAFE_RELEASE(font); 
	};

	void init(const int fontSize, const bool italic = false, char* text = "", RECT drawRect = { 0, 0, 0, 0 });
	void setFont(const int fontSize, const bool italic = false);
	void setText(LPCSTR text) { this->text = text; };

	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y) {};
	void onLostDevice()  const { font->OnLostDevice(); }
	void onResetDevice() const { font->OnResetDevice(); }

protected:
};

class ButtonUI: public GameUI {
protected:
	IDirect3DTexture9 *texture = NULL;	// 上半分がデフォルト、下半分が押されたときの画像を格納したテクスチャ
	RECT drawingRect_default;
	RECT drawingRect_pressed;
	D3DSURFACE_DESC surface;

public:
	ButtonUI(IDirect3DDevice9* device) : GameUI(device) {}
	~ButtonUI() {
		SAFE_RELEASE(texture);
	}

	void loadTexture(const char* fname, const RECT &defaultRect, const RECT &pressedRect);
	//void loadTexture(const char* fname1, const char* fname2);
	void draw();
	void receiveMouseInput(UINT msg, WPARAM wp, int x, int y);
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
};