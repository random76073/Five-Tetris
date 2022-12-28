#include "brick.h"
#include "resource.h"
#include <random>

Style VSTYLE(STYLE_V, D2D1::ColorF(0.8f, 0.2f, 0.2f, 0.5f));
Style USTYLE(STYLE_U, D2D1::ColorF(0.2f, 0.8f, 0.4f, 0.5f));
Style WSTYLE(STYLE_W, D2D1::ColorF(0.4f, 0.5f, 0.1f, 0.5f));
Style ZSTYLE(STYLE_Z, D2D1::ColorF(0.1f, 0.7f, 0.1f, 0.5f));
Style XSTYLE(STYLE_X, D2D1::ColorF(0.7f, 0.4f, 0.4f, 0.5f));
Style YSTYLE(STYLE_Y, D2D1::ColorF(0.1f, 0.2f, 0.7f, 0.5f));
Style FSTYLE(STYLE_F, D2D1::ColorF(0.1f, 0.5f, 0.7f, 0.5f));
Style LSTYLE(STYLE_L, D2D1::ColorF(0.5f, 0.2f, 0.6f, 0.5f));
Style PSTYLE(STYLE_P, D2D1::ColorF(0.2f, 0.6f, 0.5f, 0.5f));
Style NSTYLE(STYLE_N, D2D1::ColorF(0.6f, 0.9f, 0.1f, 0.5f));
Style ISTYLE(STYLE_I, D2D1::ColorF(0.2f, 0.6f, 0.9f, 0.5f));
Style TSTYLE(STYLE_T, D2D1::ColorF(0.7f, 0.6f, 0.3f, 0.5f));
Style STYLELIST[12] = {
	VSTYLE, USTYLE, WSTYLE, ZSTYLE, XSTYLE, YSTYLE, FSTYLE, LSTYLE, PSTYLE, NSTYLE, ISTYLE, TSTYLE
};

// Brick
Brick::Brick(ID2D1HwndRenderTarget *&renderTarget, D2D1::ColorF color, IWICImagingFactory *WICFactory, float width, float height, int x, int y)
	:renderTarget(renderTarget), color(color), width(width), height(height), x(x), y(y){
	loadResourceBitmap(renderTarget, WICFactory, MAKEINTRESOURCE(IDB_PNG1), L"PNG", &image);

}
void Brick::render(ID2D1SolidColorBrush* brush, unsigned int xoffset, unsigned int yoffset) {
	renderTarget->DrawBitmap(image, D2D1::RectF(x * width + xoffset, y * height + yoffset, (x + 1) * width + xoffset, (y + 1) * height + yoffset));
	renderTarget->FillRectangle(D2D1::RectF(x * width + xoffset, y * height + yoffset, (x + 1) * width + xoffset, (y + 1) * height + yoffset), brush);
}
std::tuple<bool, bool, bool> Brick::isEdge(std::vector<std::vector<unsigned int>> mesh) {
	bool left, right, down;
	// not on edge
	if (x != 0) {
		// in mesh
		if (this->inMesh()) {
			// isn't on side by bricks
			if (mesh[y][x - 1] != 2) {
				left = false;
			}else{
				left = true;
			}
		}else{
			left = false;
		}
	}else{
		left = true;
	}

	// not on edge
	if (x != MSH_WIDTH - 1) {
		// in mesh
		if (this->inMesh()) {
			// isn't on side by bricks
			if (mesh[y][x + 1] != 2) {
				right = false;
			}else{
				right = true;
			}
		}else{
			right = false;
		}
	}else{
		right = true;
	}

	// not on edge
	if (y != MSH_HEIGHT - 1) {
		// in mesh
		if (this->inMesh()) {
			// isn't on side by bricks
			if (mesh[y + 1][x] != 2) {
				down = false;
			}else{
				down = true;
			}
		}else{
			down = false;
		}
	}else{
		down = true;
	}
	
	return std::make_tuple(left, right, down);
}

std::tuple<bool, bool, bool, bool> Brick::isFault(std::vector<std::vector<unsigned int>> mesh){
	bool left, right, down, center=false;
	left = (x < 0) ? true : false;
	right = (x > MSH_WIDTH - 1) ? true : false;
	down = (y > MSH_HEIGHT - 1) ? true : false;
	if (!(left || right)) {
		center = (mesh[y][x] == 2) ? true : false;
	}
	return std::make_tuple(left, right, down, center);
}
bool Brick::inMesh() {
	return (0 <= x && x < MSH_WIDTH && y < MSH_HEIGHT) ? true : false;
}

// Brick Group
BrickGroup::BrickGroup(Style style, ID2D1HwndRenderTarget* renderTarget, IWICImagingFactory* WICFactory, int x, int y, float width, float height, std::vector<std::vector<unsigned int>> mesh)
	: x(x), y(y), style(style), mesh(mesh){
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (style.mesh[i][j] == 1) {
				bricks.push_back(Brick(renderTarget, style.color, WICFactory, width, height, x + j, y + i));
			}
		}
	}
	renderTarget->CreateSolidColorBrush(style.color, &brush);
	this->flush();

}
std::tuple<bool, bool, bool> BrickGroup::isEdge() {
	bool left, right, down;
	left = right = down = false;
	std::tuple<bool, bool, bool> tmp;
	for (int i = 0; i < 5; i++) {
		tmp = bricks[i].isEdge(mesh);
		left = (std::get<0>(tmp)) ? true : left;
		right = (std::get<1>(tmp)) ? true : right;
		down = (std::get<2>(tmp)) ? true : down;
	}
	return std::make_tuple(left, right, down);
}
bool BrickGroup::move(BRICK_MOVE_DIRECTION direction, BrickLayout* layout) {
	if (std::get<2>(this->isEdge()) && direction == BM_DOWN) {
		return true;
	}
	else if ((std::get<0>(this->isEdge()) && direction == BM_LEFT) ||
		(std::get<1>(this->isEdge()) && direction == BM_RIGHT)) {
		return false;
	}
	switch (direction){
		case BM_DOWN:
			y += 1;
			this->flush();
			break;
		case BM_LEFT:
			x -= 1;
			this->flush();
			break;
		case BM_RIGHT:
			x += 1;
			this->flush();
			break;
		case BM_HARDDOWN:
			while (!(std::get<2>(this->isEdge()))) {
				y += 1;
				this->flush();
				layout->paste();
			}
			return true;
	}
	return false;
}
void BrickGroup::turn(BRICK_TURN_DIRECTION direction, BrickLayout* layout) {
	// save
	unsigned int lastMesh[5][5];
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			lastMesh[i][j] = style.mesh[i][j];
		}
	}
	// turn
	unsigned int newMesh[5][5] = { 0 };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (style.mesh[j][i] == 1) {
				if (direction == BT_LEFT) {
					newMesh[4 - i][j] = 1;
				}else{
					newMesh[i][4 - j] = 1;
				}
			}
		}
	}
	// set
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			style.mesh[i][j] = newMesh[i][j];
		}
	}
	// flush
	this->flush();
	// verify
	for (int i = 0; i < 5; i++) {
		// overlapping and down
		if (std::get<3>(bricks[i].isFault(mesh)) || std::get<2>(bricks[i].isFault(mesh))) {
			// discard
			goto discard;
		}else{
			// left
			while (std::get<0>(bricks[i].isFault(mesh))) {
				int x = bricks[i].x;
				this->move(BM_RIGHT, layout);
				if (x == bricks[i].x) {
					// discard
					goto discard;
				}
			}
			// right
			while (std::get<1>(bricks[i].isFault(mesh))) {
				int x = bricks[i].x;
				this->move(BM_LEFT, layout);
				if (x == bricks[i].x) {
					// discard
					goto discard;
				}
			}
		}
	}
	layout->paste();
	return;
discard:
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			style.mesh[i][j] = lastMesh[i][j];
		}
	}
	this->flush();
	layout->paste();
	return;

}
void BrickGroup::flip() {
	int t = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			t = style.mesh[i][j];
			style.mesh[i][j] = style.mesh[i][4 - j];
			style.mesh[i][4 - j] = t;
		}
	}
	this->flush();
}
void BrickGroup::flush() {
	int count = 0;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++){
			if (style.mesh[i][j] == 1) {
				bricks[count].x = x + j;
				bricks[count].y = y + i;
				count += 1;
			}
		}
	}
}
void BrickGroup::render(unsigned int xoffset, unsigned int yoffset) {
	for (int i = 0; i < 5; i++) {
		if (bricks[i].y > -1) {
			bricks[i].render(brush, xoffset, yoffset);
		}
	}
}

// Brick Layout
BrickLayout::BrickLayout(ID2D1HwndRenderTarget* renderTarget, IWICImagingFactory* WICFactory, float width, float height, unsigned int x, unsigned int y, std::vector<std::vector<unsigned int>>mesh)
	: renderTarget(renderTarget), WICFactory(WICFactory), group(USTYLE, renderTarget, WICFactory, 3, -1, width, height, mesh), width(width), height(height), x(x), y(y), mesh(mesh), 
	savingGroup(nullptr, 0), savingGroupThumbnail(renderTarget, Style(nullptr, D2D1::ColorF(0.1f, 0.1f, 0.8f, 0.1f)), WICFactory, width / 2, height / 2, x - width * 5 - 10, y) {
	renderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f), &defaultBrush);
	for (int i = 0; i < 5; i++) {
		Style tmp = STYLELIST[(rand() + i) % 12];
		groupBuffer.push_back(tmp);
		groupBufferThumbnail.push_back(Thumbnail(renderTarget, tmp, WICFactory, width / 2, height / 2, x + MSH_WIDTH * width, y + i * height / 2 * 5 + 10));
	}
}
void BrickLayout::harden() {
	for (int i = 0; i < MSH_HEIGHT; i++) {
		for (int j = 0; j < MSH_WIDTH; j++) {
			if (mesh[i][j] == 1) {
				mesh[i][j] = 2;
			}
		}
	}
	generateGroup();
	canSave = true;
}
void BrickLayout::generateGroup() {
	bricks.insert(bricks.end(), group.bricks.begin(), group.bricks.end());
	srand(time(0));
	groupBuffer.erase(groupBuffer.begin());
	groupBufferThumbnail.erase(groupBufferThumbnail.begin());
	for (int i = 0; i < 4; i++) {
		groupBufferThumbnail[i].y -= 60;
	}
	Style tmp = STYLELIST[rand() % 12];
	groupBuffer.push_back(tmp);
	groupBufferThumbnail.push_back(Thumbnail(renderTarget, tmp, WICFactory, width / 2, height / 2, x + MSH_WIDTH * width, y + 5 * height / 2 * 5 + 10));
	group = BrickGroup(groupBuffer[0], renderTarget, WICFactory, 3, -1, width, height, mesh);
}
void BrickLayout::render() {
	// Background
	defaultBrush->SetColor(D2D1::ColorF(0.4f, 0.4f, 0.4f));
	renderTarget->FillRectangle(D2D1::RectF(x, y, MSH_WIDTH * width + x, MSH_HEIGHT * height + y), defaultBrush);
	// Mesh
	defaultBrush->SetColor(D2D1::ColorF(0, 0, 0));
	for (int i = 0; i < MSH_HEIGHT; i++) {
		renderTarget->DrawLine(D2D1::Point2F(x, y + i * height), D2D1::Point2F(x + MSH_WIDTH * width, y + i * height), defaultBrush);
	}
	for (int i = 0; i < MSH_WIDTH; i++) {
		renderTarget->DrawLine(D2D1::Point2F(x + i * width, y), D2D1::Point2F(x + i * width, y + MSH_HEIGHT * height), defaultBrush);
	}
	// Bricks
	group.render(x, y);
	for (int i = 0; i < bricks.size(); i++) {
		defaultBrush->SetColor(bricks[i].color);
		bricks[i].render(defaultBrush, x, y);
	}
	// Save group
	defaultBrush->SetColor(savingGroupThumbnail.style.color);
	savingGroupThumbnail.render(defaultBrush);
	// Group buffer
	for (int i = 1; i < 5; i++) {
		defaultBrush->SetColor(groupBufferThumbnail[i].style.color);
		groupBufferThumbnail[i].render(defaultBrush);
	}
}
void BrickLayout::paste() {
	// clear
	for (int i = 0; i < MSH_HEIGHT; i++) {
		for (int j = 0; j < MSH_WIDTH; j++) {
			mesh[i][j] = (mesh[i][j] == 1) ? 0 : mesh[i][j];
		}
	}
	// refactor
	for (int i = 0; i < 5; i++) {
		mesh[group.bricks[i].y][group.bricks[i].x] = 1;
	}
	// update
	group.mesh = mesh;
}
std::vector<unsigned int> BrickLayout::deterLine(){
	std::vector<unsigned int> result;
	bool complete = true;
	for (int i = 0; i < MSH_HEIGHT; i++) {
		complete = true;
		for (int j = 0; j < MSH_WIDTH; j++) {
			if (!(mesh[i][j] == 2)) {
				complete = false;
				break;
			}
		}
		if (complete) {
			result.push_back(i);
		}
	}
	return result;

}
void BrickLayout::deleteLine(unsigned int line) {
	// delete line in mesh
	for (int i = 0; i < MSH_WIDTH; i++) {
		mesh[line][i] = 0;
	}
	// delete line in bricks
	for (int i = 0; i < bricks.size();) {
		if (bricks[i].y == line) {
			bricks.erase(bricks.begin() + i);
		}else{
			++i;
		}
	}
	// down all the bricks in mesh
	for (int i = line - 1; i > 0; i--) {
		for (int j = 0; j < MSH_WIDTH; j++) {
			if (mesh[i][j] == 2) {
				mesh[i][j] = 0;
				mesh[i + 1][j] = 2;
			}
		}
	}
	// down all the bricks in bricks
	for (int i = 0; i < bricks.size(); i++) {
		if (bricks[i].y < static_cast<int>(line)) {
			bricks[i].y += 1;
		}
	}

	paste();

}
bool BrickLayout::isFailed() {
	for (int i = 0; i < MSH_WIDTH; i++) {
		if (mesh[0][i] == 2) {
			return true;
		}
	}
	return false;
}
void BrickLayout::save() {
	if (canSave) {
		canSave = false;
		Style tmp = savingGroup;
		savingGroup = group.style;
		savingGroupThumbnail = Thumbnail(renderTarget, savingGroup, WICFactory, width / 2, height / 2, x - width * 5 - 10, y);
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (tmp.mesh[i][j] == 1) {
					goto notZero;
				}
			}
		}
		srand(time(0));
		group = BrickGroup(groupBuffer[0], renderTarget, WICFactory, 3, -1, width, height, mesh);

		return;
	notZero:
		group = BrickGroup(tmp, renderTarget, WICFactory, 3, -1, width, height, mesh);
	}
}

Thumbnail::Thumbnail(ID2D1HwndRenderTarget*& renderTarget, Style style, IWICImagingFactory* WICFactory, float width, float height, int x, int y) 
			: style(style), renderTarget(renderTarget), width(width), height(height), x(x), y(y) {
	loadResourceBitmap(renderTarget, WICFactory, MAKEINTRESOURCE(IDB_PNG1), L"PNG", &image);
}

void Thumbnail::render(ID2D1SolidColorBrush* brush) {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			if (style.mesh[i][j] == 1) {
				renderTarget->DrawBitmap(image, D2D1::RectF(x + j * width, y + i * height, x + (j + 1) * width, y + (i + 1) * height), 0.5f);
				renderTarget->FillRectangle(D2D1::RectF(x + j * width, y + i * height, x + (j + 1) * width, y + (i + 1) * height), brush);
			}
		}
	}
}
