#pragma once
#include <basic.h>
#include <windowsx.h>
#include <wincodecsdk.h>
#include <dwrite.h>
#include <imm.h>
#include <d3d11_1.h>
#include <d2d1effects.h>
#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>
#include <ui.h>
// static library
#pragma comment (lib, "windowscodecs.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "imm32.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "d3d11.lib")

class Clock {
	float timer = 0.0f, lastTime = 0.0f, lastTimer = 0.0f;
public:
	// initialize and reset.
	void init();
	void tick(float fps);
	/// @returns
	// return the time from last initialize.
	float getTime();
};

// Please implement it with class IGame if you want to use it.
// There are some method must to be implemented
// 1. render
// 2. processInput
// 3. deterStatus
// And there are also some method can be cover.
// 1. init
// 2. destroy
// 3. start
// 4. update
class Game2D {
protected:
	// factories and render target
	ID2D1Factory1* Factory;
	IWICImagingFactory* WICFactory;
	ID2D1DeviceContext* deviceContext;
	IDWriteFactory* DWriteFactory;
	ID3D11Device* device3d;
	ID2D1Device* device;
	ID3D11DeviceContext* deviceContext3d;
	D3D_FEATURE_LEVEL featureLevel;
	ID2D1Bitmap1* targetBitmap;
	// swap chain
	IDXGISwapChain1* swapChain;
	DXGI_PRESENT_PARAMETERS presentParam;
	// default paintings
	IDWriteTextFormat* defaultFont;
	ID2D1SolidColorBrush* defaultBrush;
	ID2D1Effect* defaultEffect;
	// window
	HWND window;
	const unsigned int SCR_WIDTH = 1440, SCR_HEIGHT = 810;
	MSG msg;
	// clock and frequent
	Clock clock;
	// IMM settings
	HIMC g_hIMC = nullptr;
	// game status
	enum gameStatus {
		GS_GAME,
		GS_PAUSE,
		GS_WIN,
		GS_LOSE,
		GS_MENU
	};
	gameStatus status;
public:
	// create window
	void createWindow(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd, WNDPROC WindowProc);
	// "WM_CREATE"
	virtual void init();
	// "WM_CLOSE"
	void close();
	// "WM_DESTROY"
	virtual void destroy();
	// "WM_KEYDOWN"
	virtual void processInput(WPARAM wParam) = 0;
	// "WM_PAINT"
	virtual void render() = 0;
	// "WM_SETFOCUS"
	void setFocus();
	// "WM_KILLFOCUS"
	void killFocus();
	virtual void deterStatus() = 0;
	bool getMessage();
	// Last of render cycle
	// Process some auto events
	// remember use it in your "update" in the last.
	virtual void update(float fps);
	// initialize some variables.
	// Use it before the render cycle start.
	virtual void start();
	
	void changeStatus(gameStatus status);

};
