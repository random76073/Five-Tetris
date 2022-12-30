#include <IGame.h>

void IGame::init() {
	Game2D::init();
	loadResourceBitmap(renderTarget, WICFactory, MAKEINTRESOURCE(IDB_PNG2), L"PNG", &background);
}
void IGame::start() {
	Game2D::start();
	// Initialize
	downFreq = 1.0f;
	downFlag = 0;
	// movement status flag
	// Down, left, right, harddown.
	// score record
	score = 0;

	float length = (renderTarget->GetSize().width > renderTarget->GetSize().height) ? renderTarget->GetSize().height / static_cast<float>(MSH_HEIGHT) : renderTarget->GetSize().width / static_cast<float>(MSH_WIDTH);
	std::vector<std::vector<unsigned int>>mesh;
	for (int i = 0; i < MSH_HEIGHT; i++) {
		mesh.push_back(std::vector<unsigned int>());
		for (int j = 0; j < MSH_WIDTH; j++) {
			mesh[i].push_back(0);
		}
	}

	layout = new BrickLayout(renderTarget, WICFactory, length, length, 360, 0, mesh);

	player = new SoundPlayer();
	player->start();
}
void IGame::destroy() {
	player->stop();
	player->close();
	delete layout;
	delete player;
	Game2D::destroy();
}
void IGame::render() {
	renderTarget->BeginDraw();
	renderTarget->DrawBitmap(background, D2D1::RectF(0, 0, SCR_WIDTH, SCR_HEIGHT));
	layout->render();
	char str2[10];
	sprintf_s(str2, 10, "%d", score);
	wchar_t text[10];
	mbstowcs_s(nullptr, text, 10, str2, strlen(str2));
	float length = (renderTarget->GetSize().width > renderTarget->GetSize().height) ? renderTarget->GetSize().height / static_cast<float>(MSH_HEIGHT) : renderTarget->GetSize().width / static_cast<float>(MSH_WIDTH);
	renderTarget->DrawText(text, (UINT32)(wcslen(text)), defaultFont, D2D1::RectF(360 + length * MSH_WIDTH, 0, 120 + 360 + length * MSH_WIDTH, 48 + 0), defaultBrush);
	renderTarget->EndDraw();
}
void IGame::processInput(WPARAM wParam) {
	switch (wParam) {
	case 0x41: // Left A
		layout->group.move(BM_LEFT, layout);
		return;
	case 0x44: // right D
		layout->group.move(BM_RIGHT, layout);
		return;
	case 0x57: // harddown W
		if (layout->group.move(BM_HARDDOWN, layout)) {
			layout->harden();
			layout->paste();
		}
		return;
	case 0x53: // down S
		if (layout->group.move(BM_DOWN, layout)) {
			layout->harden();
			layout->paste();
		}
		return;

	case 0x48: // Turn left H
		layout->group.turn(BT_LEFT, layout);
		return;
	case 0x4A: // Turn right J
		layout->group.turn(BT_RIGHT, layout);
		return;

	case 0x4B: // Flip
		layout->group.flip(layout);
		return;

		// case VK_SPACE: // Pause Space
			// paused = !paused;
	case 0x4C: // Save
		layout->save();
	}
}
void IGame::update(float fps) {
	switch (status)
	{
	case Game2D::GS_GAME: {
		// Down
		if (clock.getTime() / downFreq > downFlag) {
			downFlag += 1;
			if (layout->group.move(BM_DOWN, layout)) {
				layout->harden();
				layout->paste();
			}

		}

		// Update down frequent
		if (downFreq > 0.2f) {
			downFreq -= 0.00002f;
		}

		// deletermiline
		std::vector<unsigned int> tmp = layout->deterLine();
		score += pow(tmp.size() + combo, 2) * 1000;
		if (tmp.size() > 0) {
			combo += 1;
		}
		else {
			combo = 0;
		}
		for (int i = 0; i < tmp.size(); i++) {
			layout->deleteLine(tmp[i]);
		}

		break;
	}
	case Game2D::GS_PAUSE:
		break;
	case Game2D::GS_WIN:
		break;
	case Game2D::GS_LOSE:
		break;
	case Game2D::GS_MENU:
		break;
	default:
		break;
	}
		
	Game2D::update(fps);
}
void IGame::deterStatus() {
	// Deter if failed
	if (layout->isFailed()) {
		changeStatus(GS_LOSE);
		if (MessageBox(window, L"Oops, you are lose! \n Would you like to restart?", L"Five Tetris", MB_OKCANCEL)) {
			changeStatus(GS_GAME);
			start();
		}
		else {
			CloseWindow(window);
		}
	}
}
