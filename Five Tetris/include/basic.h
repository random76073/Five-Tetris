#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#include <d2d1.h>
#include <d2d1_1.h>

HRESULT CreateRenderSource(ID2D1Factory1* factory, ID2D1HwndRenderTarget* renderTarget, HWND hwnd);
template <class T> void release(T** ppT);