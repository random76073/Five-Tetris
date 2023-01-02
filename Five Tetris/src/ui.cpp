#include <ui.h>
#include <string>
void Surface::render() {
	for (int i = 0; i < components.size(); i++) {
		components[i]->render();
	}
}
Component::Component(ID2D1DeviceContext* deviceContext) : deviceContext(deviceContext) {
	
}
void Component::move(int x, int y) {
	this->x = x;
	this->y = y;
}
IntPanel::IntPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, int& integer)
	: Text(x, y, font, deviceContext, color), integer(integer) {
}
IntPanel::IntPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, int& integer)
	: Text(x, y, font, deviceContext, brush), integer(integer) {

}
IntPanel::IntPanel(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, const wchar_t* fontname, int& integer, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize)
	: Text(x, y, factory, deviceContext, brush, fontname, fontWeight, fontStyle, fontStretch, fontSize), integer(integer){

}
void IntPanel::render() {
	char str2[10];
	sprintf_s(str2, 10, "%d", integer);

	wchar_t text[10];
	mbstowcs_s(nullptr, text, 10, str2, strlen(str2));
	deviceContext->DrawText(text, (UINT32)(wcslen(text)), font, D2D1::RectF(x, y, font->GetFontSize() * wcslen(text) + x, font->GetFontSize() + y), brush);
}

void IntPanel::operator+=(Label label) {
	char str2[10];
	sprintf_s(str2, 10, "%d", integer);

	wchar_t text[10];
	mbstowcs_s(nullptr, text, 10, str2, strlen(str2));
	label.Component::move(font->GetFontSize() * wcslen(text) + x, y);
}

StrPanel::StrPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, wchar_t*& str)
	: Text(x, y, font, deviceContext, color), str(str) {
}

StrPanel::StrPanel(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, 
	const wchar_t* fontname, wchar_t*& str, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize)
	: Text(x, y, factory, deviceContext, brush, fontname, fontWeight, fontStyle, fontStretch, fontSize), str(str) {

}

StrPanel::StrPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, wchar_t*& str)
	: Text(x, y, font, deviceContext, brush), str(str) {

}

void StrPanel::render() {
	deviceContext->DrawText(str, (UINT32)(wcslen(str)), font, D2D1::RectF(x, y, x + font->GetFontSize() * wcslen(str), font->GetFontSize() + y), brush);
}

Label::Label(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, const wchar_t* text) 
	: Text(x, y, font, deviceContext, color), text(text) {

}

Label::Label(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, const wchar_t* text) 
	: Text(x, y, font, deviceContext, brush), text(text) {

}

Label::Label(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, const wchar_t* text, 
	const wchar_t* fontname, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize) 
	: Text(x, y, factory, deviceContext, brush, fontname, fontWeight, fontStyle, fontStretch, fontSize), text(text){

}
void Label::operator+=(IntPanel intPanel){
	intPanel.Component::move(x + font->GetFontSize() * wcslen(text), y);
}
void Label::render() {
	deviceContext->DrawText(text, (UINT32)(wcslen(text)), font, D2D1::RectF(x, y, x + font->GetFontSize() * wcslen(text), font->GetFontSize() + y), brush);
}

Text::Text(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color) : Component(deviceContext), font(font) {
	deviceContext->CreateSolidColorBrush(color, &brush);
	this->x = x;
	this->y = y;
}

Text::Text(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush) : Component(deviceContext), font(font), brush(brush) {
	this->x = x;
	this->y = y;
}

Text::Text(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, 
	const wchar_t* fontname, DWRITE_FONT_WEIGHT fontWeight, DWRITE_FONT_STYLE fontStyle, DWRITE_FONT_STRETCH fontStretch, float fontSize) 
	: Component(deviceContext), brush(brush) {
	factory->CreateTextFormat(fontname, nullptr, fontWeight, fontStyle, fontStretch, fontSize, L"en-us", &font);
}

Frame::Frame(ID2D1DeviceContext* deviceContext) : deviceContext(deviceContext){
	
}

void Frame::operator+=(Surface& surface) {
	surfaces.push_back(surface);
	surface.id = surfaces.size(); // Pay attention here, id == subscript + 1 !
}
void Frame::operator-=(Surface& surface) {
	surfaces.erase(surfaces.begin() + (surface.id - 1));
	for (int i = surface.id - 1; i < surfaces.size(); i++) {
		surfaces[i].id -= 1;
	}
	surface.id = 0;
}

void Frame::enable(Surface& surface) {
	if (surface.layer == -1) {
		surfaces[surface.id].layer = ++maxLayer;
	}
}

void Frame::disable(Surface& surface) {
	if (surface.layer >= 0) {
		surface.layer = -1;
		maxLayer -= 1;
	}
}
