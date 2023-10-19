#pragma once
#include "Menu.h"
using namespace std;

class Game {
private:
	void drawGameStage();
public:
	Game();
	~Game();
	bool isPlay;
	void draw();
	void startGame();
	void play();
	Menu menu;
};