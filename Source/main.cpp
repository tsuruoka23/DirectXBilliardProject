<<<<<<< HEAD
#include "../Header/stdafx.h"
#include "../Header/utility.h"
#include "../Header/game_screen.h"

#define TITLE _T("DirectX")


ConsoleViewer cv;
GameScreen gameScreen;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	cv.log("<WinMain begin>");

	WNDCLASS wc;
	wc.style		 = CS_CLASSDC;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TITLE;

	if (!RegisterClass(&wc)) return FALSE;

	cv.log("<ウィンドウ生成>");
	HWND hWnd = CreateWindow(
		TITLE, TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	
		600, 0, 1600, 900,					// デフォルト = CW_USERDEFAULT
		(HWND)NULL, (HMENU)NULL, hInstance, (LPSTR)NULL);

	cv.log("<メッセージループ開始>");
	MSG msg = {};
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			gameScreen.update();
		}
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_CREATE:
		cv.log("<WM_CREATE>");
		gameScreen.initDXGraphics(hWnd);
		return 0;
	case WM_DESTROY: cv.log("<WM_DESTROY>"); PostQuitMessage(0); break;
	case WM_CLOSE:   cv.log("<WM_CLOSE>");   PostQuitMessage(0); break;	
	case WM_SIZE:
		if (wp == SIZE_RESTORED || wp == SIZE_MAXIMIZED) {
			gameScreen.changeWindowSize(LOWORD(lp), HIWORD(lp));
		}
		return 0;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE: 
	case WM_KEYDOWN:
	case WM_KEYUP:
		gameScreen.receiveMouseInput(msg, wp, LOWORD(lp), HIWORD(lp)); 
		return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

=======
#include "../Header/stdafx.h"
#include "../Header/utility.h"
#include "../Header/game_screen.h"

#define TITLE _T("DirectX")


ConsoleViewer cv;
GameScreen gameScreen;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	cv.log("<WinMain begin>");

	WNDCLASS wc;
	wc.style		 = CS_CLASSDC;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = TITLE;

	if (!RegisterClass(&wc)) return FALSE;

	cv.log("<ウィンドウ生成>");
	HWND hWnd = CreateWindow(
		TITLE, TITLE,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	
		600, 0, 1600, 900,					// デフォルト = CW_USERDEFAULT
		(HWND)NULL, (HMENU)NULL, hInstance, (LPSTR)NULL);

	cv.log("<メッセージループ開始>");
	MSG msg = {};
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) { break; }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			gameScreen.update();
		}
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_CREATE:
		cv.log("<WM_CREATE>");
		gameScreen.initDXGraphics(hWnd);
		return 0;
	case WM_DESTROY: cv.log("<WM_DESTROY>"); PostQuitMessage(0); break;
	case WM_CLOSE:   cv.log("<WM_CLOSE>");   PostQuitMessage(0); break;	
	case WM_SIZE:
		if (wp == SIZE_RESTORED || wp == SIZE_MAXIMIZED) {
			gameScreen.changeWindowSize(LOWORD(lp), HIWORD(lp));
		}
		return 0;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN: 
	case WM_LBUTTONUP: 
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE: 
	case WM_KEYDOWN:
	case WM_KEYUP:
		gameScreen.receiveMouseInput(msg, wp, LOWORD(lp), HIWORD(lp)); 
		return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
