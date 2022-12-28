#include "basic.h"
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
HRESULT CreateRenderSource(ID2D1Factory1* factory, ID2D1HwndRenderTarget** renderTarget, HWND hwnd) {
	HRESULT hr = S_OK;
	if (*renderTarget == nullptr) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hwnd, size),
            renderTarget);
	}
    return hr;
}
template<class Interface>
void SAFE_RELEASE(Interface** ppInterfaceToRelease){
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}
void loadResourceBitmap(
    ID2D1RenderTarget* pRenderTarget,
    IWICImagingFactory* pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    ID2D1Bitmap** ppBitmap
)
{
    IWICBitmapDecoder* pDecoder = NULL;
    IWICBitmapFrameDecode* pSource = NULL;
    IWICStream* pStream = NULL;
    IWICFormatConverter* pConverter = NULL;
    IWICBitmapScaler* pScaler = NULL;

    HRSRC imageResHandle = NULL;
    HGLOBAL imageResDataHandle = NULL;
    void* pImageFile = NULL;
    DWORD imageFileSize = 0;

    // Locate the resource.
    imageResHandle = FindResource(HINST_THISCOMPONENT, resourceName, resourceType);
    // Load the resource.
    imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
    pImageFile = LockResource(imageResDataHandle);
    imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
    pIWICFactory->CreateStream(&pStream);
    pStream->InitializeFromMemory(
        reinterpret_cast<BYTE*>(pImageFile),
        imageFileSize
    );

    pIWICFactory->CreateDecoderFromStream(
        pStream,
        NULL,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
    );

    pDecoder->GetFrame(0, &pSource);

    pIWICFactory->CreateFormatConverter(&pConverter);
    pConverter->Initialize(
        pSource,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeMedianCut
    );

    pRenderTarget->CreateBitmapFromWicBitmap(
        pConverter,
        NULL,
        ppBitmap
    );
    SAFE_RELEASE(&pDecoder);
    SAFE_RELEASE(&pSource);
    SAFE_RELEASE(&pStream);
    SAFE_RELEASE(&pConverter);
    SAFE_RELEASE(&pScaler);

}
SoundPlayer::SoundPlayer() {
    MCIERROR err = mciSendString(L"open Tetris.mp3 alias song", NULL, 0, NULL);
    mciSendString(L"setaudio song volume to 20", NULL, 0, NULL);
}
void SoundPlayer::start() {
    mciSendString(L"play song", NULL, 0, NULL);
}
void SoundPlayer::pause() {
    mciSendString(L"pause song", NULL, 0, NULL);
}
void SoundPlayer::resume() {
    mciSendString(L"resume song", NULL, 0, NULL);
}
void SoundPlayer::stop() {
    mciSendString(L"stop song", NULL, 0, NULL);
}
void SoundPlayer::close() {
    mciSendString(L"close song", NULL, 0, NULL);
}


