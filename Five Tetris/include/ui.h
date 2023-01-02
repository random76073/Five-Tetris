#pragma once
#include <basic.h>
#include <dwrite.h>
#include <vector>

#pragma comment (lib, "dwrite.lib")
// Statements
class Surface;
class Frame;
class Component;
class Text;
class IntPanel;
class StrPanel;

// All component must inherit this.
// This contain all the basic movement.
class Component {
public:
	int x = 0, y = 0;
	int width = 0, height = 0;
	unsigned int id = 0;
	ID2D1DeviceContext* deviceContext;
	Component(ID2D1DeviceContext* deviceContext);
	virtual void render() = 0;
	void move(int x, int y);
};


// Please create a surface to contain all the component.
// This define one surface.
class Surface : public Component {
	std::vector<Component*> components;
public:
	int layer = -1;
	template <typename C>
	void operator+=(C* component) {
		components.push_back(component);
		component->id = components.size(); // Pay attention here, id == subscript + 1!
	}

	template <typename C>
	void operator-=(C* component) {
		if (component->id != 0) {
			components.erase(components.begin() + (component->id - 1));
			// move
			for (int i = component->id - 1; i < components.size(); i++) {
				components[i]->id -= 1;
			}
			// clear
			component->id = 0;
		}
	}

	void render();

};
// Please create a frame to contain all the surface.
// This can be used in IGame.
// One program must only has one frame, or the identity of each component will be wrong.
class Frame {
	std::vector<Surface&> surfaces;
	int maxLayer = -1;
	ID2D1DeviceContext* deviceContext;
public:
	Frame(ID2D1DeviceContext* deviceContext);
	void operator+=(Surface& surface);
	void operator-=(Surface& surface);
	void enable(Surface& surface);
	void disable(Surface& surface);
};
// Show text.
class Text : public Component {
protected:
	IDWriteTextFormat* font = nullptr;
	ID2D1SolidColorBrush* brush = nullptr;
public:
	Text(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color);
	Text(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush);
	Text(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush,
		const wchar_t* fontname, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL, float fontSize = 72.0f);
};
// Show fixed text.
class Label : public Text {
protected:
	const wchar_t* text;
public:
	Label(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, const wchar_t* text);
	Label(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, const wchar_t* text);
	Label(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, const wchar_t* text,
		const wchar_t* fontname, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL, float fontSize = 72.0f);
	void render();
	void operator+=(IntPanel intPanel);
};

// Show volatile integer.
class IntPanel : public Text {
	int& integer;
public:
	IntPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, int& integer);
	IntPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, int& integer);
	IntPanel(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush,
		const wchar_t* fontname, int& integer, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL, float fontSize = 72.0f);
	void render();
	void operator+=(Label label);
};
// Show volatile string.
class StrPanel : public Text {
	wchar_t* str;
public:
	StrPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, D2D1::ColorF color, wchar_t*& str);
	StrPanel(int x, int y, IDWriteTextFormat* font, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush, wchar_t*& str);
	StrPanel(int x, int y, IDWriteFactory* factory, ID2D1DeviceContext* deviceContext, ID2D1SolidColorBrush* brush,
		const wchar_t* fontname, wchar_t*& str, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL, float fontSize = 72.0f);
	void render();
};


