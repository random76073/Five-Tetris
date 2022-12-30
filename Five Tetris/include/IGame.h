#pragma once
#include "Game.h"
#include "brick.h"
#include "resource.h"

class IGame : public Game2D {
	// resource
	ID2D1Bitmap* background;
	// clock and frequent
	float downFreq = 1.0f;
	unsigned int downFlag = 0;
	// self-defining interface
	BrickLayout* layout;
	SoundPlayer* player;
	// score record
	unsigned int score = 0;
	int combo = 0;
public:
	void init();
	void destroy();
	void render();
	void processInput(WPARAM wParam);
	void start();
	void update(float fps);
	void deterStatus();
};

