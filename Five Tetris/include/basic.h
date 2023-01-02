#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#include <d2d1.h>
#include <d2d1_1.h>
#include <mmsystem.h>

template<typename Interface>
inline void SAFE_RELEASE(Interface** ppInterfaceToRelease);

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#if !defined(PLAYER_NOTIF_POS_COUNT)
#   define PLAYER_NOTIF_POS_COUNT   20
#endif /* PLAYER_NOTIF_POS_COUNT */

void loadResourceBitmap(
    ID2D1DeviceContext *deviceContext,
    IWICImagingFactory * pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    ID2D1Bitmap **ppBitmap
);
class SoundPlayer {
public:
    SoundPlayer();
    void start();
    void pause();
    void resume();
    void stop();
    void close();
};