// include
#include "brick.h"
#include <windowsx.h>
#include "resource.h"
#include <wincodecsdk.h>
#include <dwrite.h>
#include <math.h>
#include <iostream>
#include <imm.h>
#include <d2d1effects.h>
#include <d2d1effectauthor.h>
#include <d2d1effecthelpers.h>
// static library
#pragma comment (lib, "windowscodecs.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "imm32.lib")
#pragma comment (lib, "dxguid.lib")
// macro (constants)
#define FREQ 1.0f / 60.0f
// factories and render target
ID2D1Factory1* Factory;
IWICImagingFactory* WICFactory;
ID2D1HwndRenderTarget* renderTarget;
IDWriteFactory* DWriteFactory;
// callback function statement
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// resource
ID2D1Bitmap* background;
IDWriteTextFormat* defaultFont;
ID2D1SolidColorBrush* defaultBrush;
// clock and frequent
float timer = 0.0f;
float lastTimer = 0.0f;
float downFreq = 1.0f;
unsigned int downFlag = 0;
const unsigned int SCR_WIDTH = 1280, SCR_HEIGHT = 720;
// movement status flag
// Down, left, right, harddown.
bool moveFlag[4] = { false, false, false, false };
bool turnFlag[2] = { false, false };
bool flip = false;
// self-defining interface
BrickLayout* layout;
SoundPlayer* player;
// display
float length;
// IMM settings
HIMC g_hIMC = nullptr;
// score record
unsigned int score = 0;
// game status flag
bool paused = false;
int combo = 0;
// main function
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	// Create window
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	RegisterClass(&wc);

	HWND window = CreateWindowEx(0, L"Sample Window Class", L"Five Tetris", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, SCR_WIDTH, SCR_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(window, nShowCmd);
	g_hIMC = ImmAssociateContext(window, nullptr);
start:
	// Initialize
	timer = 0.0f;
	downFreq = 1.0f;
	downFlag = 0;
	// movement status flag
	// Down, left, right, harddown.
	// IMM settings
	g_hIMC = nullptr;
	// score record
	score = 0;

	length = (renderTarget->GetSize().width > renderTarget->GetSize().height) ? renderTarget->GetSize().height / static_cast<float>(MSH_HEIGHT) : renderTarget->GetSize().width / static_cast<float>(MSH_WIDTH);
	std::vector<std::vector<unsigned int>>mesh;
	for (int i = 0; i < MSH_HEIGHT; i++) {
		mesh.push_back(std::vector<unsigned int>());
		for (int j = 0; j < MSH_WIDTH; j++) {
			mesh[i].push_back(0);
		}
	}

	BrickLayout tmp(renderTarget, WICFactory, length, length, 360, 0, mesh);
	layout = &tmp;

	SoundPlayer tmpplayer;
	player = &tmpplayer;
	player->start();

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {

		while (!paused && GetMessage(&msg, nullptr, 0, 0) > 0) {

			timer = clock() / static_cast<float>(CLOCKS_PER_SEC);

			if ((timer - lastTimer) / downFreq > downFlag) {
				downFlag += 1;
				moveFlag[0] = true;

			}
			for (int i = 0; i < 4; i++) {
				if (moveFlag[i] == true) {
					if (layout->group.move(static_cast<BRICK_MOVE_DIRECTION>(i), layout)) {
						layout->harden();
						std::vector<unsigned int> tmp = layout->deterLine();

						if (tmp.size() > 0) {
							combo += 1;
						}else{
							combo = 0;
						}
						score += pow(tmp.size() * combo, 2) * 1000;

						for (int i = 0; i < tmp.size(); i++) {
							layout->deleteLine(tmp[i]);
						}
					}
					layout->paste();
					moveFlag[i] = false;
				}
			}
			for (int i = 0; i < 2; i++) {
				if (turnFlag[i] == true) {
					layout->group.turn(static_cast<BRICK_TURN_DIRECTION>(i), layout);
					turnFlag[i] = false;
				}
			}
			if (flip == true) {
				layout->group.flip();
				flip = false;
			}
			// Update down frequent
			downFreq -= 0.00002f;

			// Update screen
			InvalidateRect(window, nullptr, true);

			// Deter if failed
			if (layout->isFailed()) {
				if (MessageBox(window, L"Oops, you are lose! \n Would you like to restart?", L"Five Tetris", MB_OKCANCEL)) {
					lastTimer = timer;
					goto start;
				}else{
					CloseWindow(window);
				}
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		player->pause();
		while (paused && GetMessage(&msg, nullptr, 0, 0) > 0) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		player->resume();
	}

	return 0;

}
// callback function defining
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &Factory);
		CreateRenderSource(Factory, &renderTarget, hwnd);
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&WICFactory)
		);
		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&DWriteFactory));
		DWriteFactory->CreateTextFormat(L"Terminal", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-us", &defaultFont);
		defaultFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		defaultFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		loadResourceBitmap(renderTarget, WICFactory, MAKEINTRESOURCE(IDB_PNG2), L"PNG", &background);
		renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.8f, 0.4f, 0.2f), &defaultBrush);
		return 0;
	}
	case WM_CLOSE:
		if (MessageBox(hwnd, L"Are you sure want to exit?", L"Quit?", MB_OKCANCEL) == IDOK) {
			DestroyWindow(hwnd);
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		CoUninitialize();
		player->stop();
		player->close();
		exit(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case 0x41: // Left A
			moveFlag[1] = true;
			return 0;
		case 0x44: // right D
			moveFlag[2] = true;
			return 0;
		case 0x57: // harddown W
			moveFlag[3] = true;
			return 0;
		case 0x53: // down S
			moveFlag[0] = true;
			return 0;

		case 0x48: // Turn left H
			turnFlag[0] = true;
			return 0;
		case 0x4A: // Turn right J
			turnFlag[1] = true;
			return 0;

		case 0x4B: // Flip
			flip = true;
			return 0;

			// case VK_SPACE: // Pause
				// paused = !paused;
		case 0x4C: // Save
			layout->save();
		}
	case WM_PAINT: {
		renderTarget->BeginDraw();
		renderTarget->DrawBitmap(background, D2D1::RectF(0, 0, SCR_WIDTH, SCR_HEIGHT));
		layout->render();
		char str2[10];
		sprintf_s(str2, 10, "%d", score);
		wchar_t text[10];
		mbstowcs_s(nullptr, text, 10, str2, strlen(str2));
		renderTarget->DrawText(text, (UINT32)(wcslen(text)), defaultFont, D2D1::RectF(360 + length * MSH_WIDTH, 0, 120 + 360 + length * MSH_WIDTH, 48 + 0), defaultBrush);
		renderTarget->EndDraw();
	}
	case WM_SETFOCUS:
		ImmAssociateContext(hwnd, NULL);
	case WM_KILLFOCUS:
		ImmAssociateContext(hwnd, g_hIMC);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}