#include "basic.h"
HRESULT CreateRenderSource(ID2D1Factory1* factory, ID2D1HwndRenderTarget* renderTarget, HWND hwnd) {
	HRESULT hr = S_OK;
	if (renderTarget == nullptr) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            &renderTarget);
	}
    return hr;
}