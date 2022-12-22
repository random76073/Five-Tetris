#include "basic.h"
#pragma comment(lib, "d2d1")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
ID2D1Factory1* factory = nullptr;
ID2D1HwndRenderTarget* renderTarget = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);


    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Five Tetris",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
        case WM_CREATE:
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
            CreateRenderSource(factory, renderTarget, hwnd);
		case WM_DESTROY:
            CoUninitialize();
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
            ID2D1SolidColorBrush* brush;
            renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0), &brush);

            renderTarget->BeginDraw();
            renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
            renderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(), 400.0f, 400.0f), brush);
            renderTarget->EndDraw();

			EndPaint(hwnd, &ps);
			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}