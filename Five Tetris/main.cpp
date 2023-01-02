// include
#include "IGame.h"
#include "brick.h"

IGame game;
// callback function statement
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// main function
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	game.Game2D::createWindow(hInstance, hPrevInstance, lpCmdLine, nShowCmd, WindowProc);
	game.init();
	game.start();
	while (game.getMessage()) {	
		game.update(60);
		game.deterStatus();
	}
	return 0;
}
// callback function defining
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		
		return 0;
	}
	case WM_CLOSE:
		game.Game2D::close();
		return 0;
	case WM_DESTROY:
		game.destroy();
		return 0;
	case WM_KEYDOWN:
		game.processInput(wParam);
	case WM_PAINT: {
		game.render();
	}
	case WM_SETFOCUS:
		game.Game2D::setFocus();
	case WM_KILLFOCUS:
		game.Game2D::killFocus();
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}