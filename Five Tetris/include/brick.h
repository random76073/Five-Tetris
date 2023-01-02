#pragma once
#include <d2d1.h>
#include <d2d1_1.h>
#include <wincodec.h>
#include "basic.h"
#include <vector>
#include <tuple>
#include <ctime>

const int MSH_WIDTH = 18, MSH_HEIGHT = 24;

class Style {
public:
	int mesh[5][5];
	D2D1::ColorF color;
	Style(const int mesh[5][5], D2D1::ColorF color): color(color) {
		if (mesh != nullptr) {
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					this->mesh[i][j] = mesh[i][j];
				}
			}
		}
	};
};

const int STYLE_V[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 0
};

const int STYLE_W[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 1, 0, 0,
	0, 0, 1, 1, 0,
	0, 0, 0, 0, 0
};

const int STYLE_P[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 1, 0, 0,
	0, 1, 1, 0, 0,
	0, 1, 0, 0, 0,
	0, 0, 0, 0, 0
};

const int STYLE_F[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 1, 0, 0,
	0, 0, 1, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 0, 0, 0
};

const int STYLE_Z[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 1, 0,
	0, 0, 0, 0, 0
};

const int STYLE_U[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 0, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

const int STYLE_Y[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 1, 0, 0,
	0, 1, 0, 0, 0
};

const int STYLE_I[5][5] = {
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	1, 1, 1, 1, 1,
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0
};

const int STYLE_L[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 0, 0, 0,
	0, 1, 1, 0, 0
};

const int STYLE_T[5][5] = {
	0, 0, 0, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0,
	0, 0, 0, 0, 0
};

const int STYLE_N[5][5] = {
	0, 0, 0, 0, 0,
	0, 0, 0, 1, 0,
	0, 0, 1, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 1, 0, 0
};

const int STYLE_X[5][5] = {
	0, 0, 0, 0, 0,
	0, 0, 1, 0, 0,
	0, 1, 1, 1, 0,
	0, 0, 1, 0, 0,
	0, 0, 0, 0, 0
};

enum BRICK_MOVE_DIRECTION {
	BM_DOWN,
	BM_LEFT,
	BM_RIGHT,
	BM_HARDDOWN,
	BM_UP
};

enum BRICK_TURN_DIRECTION {
	BT_LEFT,
	BT_RIGHT
};

class Thumbnail {
public:
	ID2D1Bitmap* image;
	ID2D1DeviceContext* deviceContext;
	Style style;
	float width, height;

	int x, y;

	Thumbnail(ID2D1DeviceContext*& deviceContext, Style style, IWICImagingFactory* WICFactory, float width, float height, int x, int y);
	void render(ID2D1SolidColorBrush* brush);
};
class Brick {
public:
	ID2D1Bitmap* image;
	ID2D1DeviceContext* deviceContext;
	D2D1::ColorF color;
	float width, height;
	// This x and y is the position in global mesh.
	int x, y;

	Brick(ID2D1DeviceContext *&deviceContext, D2D1::ColorF color, IWICImagingFactory *WICFactory, float width, float height, int x, int y);
	void render(ID2D1SolidColorBrush* brush, unsigned int xoffset, unsigned int yoffset);
	// Left, right and down.
	std::tuple<bool, bool, bool> isEdge(std::vector<std::vector<unsigned int>> mesh);
	// Protecting mesh from the subscript out of the range when turn or flip.
	std::tuple<bool, bool, bool, bool> isFault(std::vector<std::vector<unsigned int>> mesh);
	bool inMesh();
};

class BrickLayout;

class BrickGroup {
public:
	Style style;
	std::vector<Brick> bricks;
	ID2D1SolidColorBrush* brush;
	int x, y;
	std::vector<std::vector<unsigned int>> mesh;

	BrickGroup(Style style, ID2D1DeviceContext* deviceContext, IWICImagingFactory* WICFactory, int x, int y, float width, float height, std::vector<std::vector<unsigned int>> mesh);
	/// @returns
	// It returns whether the group should be hardened.
	bool move(BRICK_MOVE_DIRECTION direction, BrickLayout* layout);
	void turn(BRICK_TURN_DIRECTION direction, BrickLayout* layout);
	void flip(BrickLayout* layout);
	void render(unsigned int xoffset, unsigned int yoffset);
	// Left, right and down.
	std::tuple<bool, bool, bool> isEdge();
	// Flush the position of bricks in vector container.
	void flush();
};

class BrickLayout {
private:
	
public:
	// Code meaning:
	// 0: White
	// 1: Active brick
	// 2: Hardened brick
	std::vector<std::vector<unsigned int>> mesh;
	ID2D1DeviceContext* deviceContext;
	IWICImagingFactory* WICFactory;
	ID2D1SolidColorBrush* defaultBrush;
	// The container to save all the hardened bricks.
	std::vector<Brick> bricks;
	// This group is the active group now.
	BrickGroup group;
	// This group is on saving.
	Style savingGroup;
	Thumbnail savingGroupThumbnail;
	std::vector<Style> groupBuffer;
	std::vector<Thumbnail> groupBufferThumbnail;
	// You only can save once every cycle.
	bool canSave=true;
	// The width and height of any single brick.
	float width, height;
	// The offset from the left-up corner of screen.
	unsigned int x, y;

	BrickLayout(ID2D1DeviceContext*deviceContext, IWICImagingFactory* WICFactory, float width, float height, unsigned int x, unsigned int y , std::vector<std::vector<unsigned int>>mesh);
	void render();
	void harden();
	void paste();
	std::vector<unsigned int> deterLine();
	void deleteLine(unsigned int line);
	bool isFailed();
	void save();
private:
	void generateGroup();
};
