#include <Game.h>
#include <ctime>

void Clock::tick(float fps) {
	lastTime = timer;
	do {
		timer = clock() / static_cast<float>(CLOCKS_PER_SEC) - lastTimer;
	} while (timer - lastTime < 1 / fps);
}
void Clock::init() {
	lastTimer = timer;
	timer = 0;
}

float Clock::getTime() {
	return timer;
}

void Game2D::createWindow(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd, WNDPROC WindowProc) {
	// Create window
	const wchar_t CLASS_NAME[] = L"Sample Window Class";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	window = CreateWindowEx(0, L"Sample Window Class", L"Five Tetris", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, SCR_WIDTH, SCR_HEIGHT, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(window, nShowCmd);
	g_hIMC = ImmAssociateContext(window, nullptr);
}

void Game2D::init() {
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &Factory);
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	CreateRenderSource(Factory, &renderTarget, window);
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
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.8f, 0.4f, 0.2f), &defaultBrush);
}

void Game2D::close() {
	if (MessageBox(window, L"Are you sure want to exit?", L"Quit?", MB_OKCANCEL) == IDOK) {
		DestroyWindow(window);
	}
}

void Game2D::destroy() {
	PostQuitMessage(0);
	CoUninitialize();
}

void Game2D::changeStatus(gameStatus status) {
	this->status = status;
}

void Game2D::setFocus() {
	ImmAssociateContext(window, nullptr);
}

void Game2D::killFocus() {
	ImmAssociateContext(window, g_hIMC);
}
bool Game2D::getMessage() {
	return GetMessage(&msg, nullptr, 0, 0) > 0;
}
void Game2D::update(float fps) {
	clock.tick(fps);
	InvalidateRect(window, 0, true);
	TranslateMessage(&msg);
	DispatchMessage(&msg);

}
void Game2D::start() {
	clock.init();
	// IMM settings
	g_hIMC = nullptr;
}

