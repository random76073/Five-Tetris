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
	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&WICFactory)
	);
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&DWriteFactory));
	DWriteFactory->CreateTextFormat(L"Terminal", nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"en-us", &defaultFont);
	
	// create device context
	IDXGIFactory2* dxgiFactory;
	IDXGIDevice1* dxgiDevice;

	D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
	};
	D3D11_CREATE_DEVICE_FLAG flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flag = static_cast<D3D11_CREATE_DEVICE_FLAG>(D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT);
#endif
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device3d, &featureLevel, &deviceContext3d);
	device3d->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	Factory->CreateDevice(dxgiDevice, &device);
	device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext);
	
	IDXGISurface* dxgiBackBuffer;
	IDXGIAdapter* dxgiAdapter;
	deviceContext->SetTarget(nullptr);
	deviceContext3d->Flush();
	RECT rect; GetClientRect(window, &rect);
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = rect.right - rect.left;
	swapChainDesc.Height = rect.bottom - rect.top;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

	dxgiDevice->GetAdapter(&dxgiAdapter);
	dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

	dxgiFactory->CreateSwapChainForHwnd(device3d, window, &swapChainDesc, nullptr, nullptr, &swapChain);
	dxgiDevice->SetMaximumFrameLatency(1);
	
	swapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
		96.0f,
		96.0f);
	deviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, bitmapProperties, &targetBitmap);
	deviceContext->SetTarget(targetBitmap);
	
	defaultFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	defaultFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	deviceContext->CreateSolidColorBrush(D2D1::ColorF(0.8f, 0.4f, 0.2f), &defaultBrush);

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

