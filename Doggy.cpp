#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "Draw.h"
#include "GLXtras.h"
#include "IO.h"
#include "Sprite.h"
#include "Text.h"
#include "mmsystem.h"
#include "windows.h"
#include <algorithm>
#include <random>
#include <fstream>
#include <iostream>
#include <string>
//using namespace std;

// app
int windowWidth = 600, windowHeight = 700;
vec3 red(1, 0, 0), green(0, 1, 0), yellow(1, 1, 0);

// sprites
Sprite background, player, explosion, congratz, menuBG, startBt, endBt, replayBt, returnToMenuBt;
Sprite *intersected = NULL;
vector<Sprite> obstacles;

// obstacle attributes
const char *obstacleNames[] = { "car-blue.png", "car-green.png", "car-purple.png" };
const float obstacleDelays[] = { 2.0f, 3.0f, 5.3f };
const int nObstacles = sizeof(obstacleDelays) / sizeof(const char*);
float obstacleStartXs[] = { -0.45f, -0.15, 0.15f, 0.45f };

// images directories
string dir("Image/");
// string dir("C:/Users/Jules/Code/GG-Projects/#3-Car-Collision/");
string backgroundImg_path = dir+"game-bg.png";
string playerImg_path = dir+"car-yellow.png";
string explosionImg_path = dir+"game-over.png";
string menuBG_path = dir+"start-bg-w.png";
string startBt_path = dir+"button-play.png";
string endBt_path = dir+"button-exit.png";
string buttonreplay_path = dir+"button-replay1.png";
string buttonmenu_path = dir+"button-menu.png";
string congratz_path = dir + "new_record.png";
string gameRecordFileName = "game_record.txt";

// audio file path
const char* gameOverWav_path = "Audio/mixkit-truck-crash-with-explosion-1616.wav";
const char* btnClickWav_path = "Audio/mixkit-car-door-slam-1564.wav";
const char* menuBgMusic_path = "Audio/Bush - The People That We Love (Instrumental).wav";
const char* carSoundEffect_path = "Audio/Lexus Lf-a Sound.wav";

float leftBoundary = -0.5, rightBoundary = 0.5, startX = -0.45f, startY = -0.85f;

bool gameover = false;
bool gamerunning = false;
bool terminateGame = false;
bool hoveringStartBt = false;
bool hoveringEndBt = false;
bool hoveringreplayBt = false;
bool hoveringreturnToMenuBt = false;

const float baseLoopDuration = 4;
float loopDuration = 4;
time_t startTime;
float elapsedTime = 0; // in seconds
float curRecord = 0; // hold game best record

// initial obstacleStartXs
void shuffleStartXs() {
	int length = sizeof(obstacleStartXs) / sizeof(obstacleStartXs[0]);
	shuffle(obstacleStartXs, obstacleStartXs + length, default_random_engine(time(nullptr)));
}

// handle game record
void checkGameRecord() {
	// create txt file to save game record
	fstream myFile;
	myFile.open(gameRecordFileName, ios::in);
	string line;
	if (myFile.is_open()) {
		getline(myFile, line);
	}
	if (line.empty()) {
		cout << "no current game record is found" << endl;
		myFile.close();
		return;
	}
	curRecord = stof(line);
	myFile.close();
	cout << "the current game record is: " << curRecord << endl;
}

void writeGameRecord() {
	fstream myFile;
	myFile.open(gameRecordFileName, ios::out);
	if (myFile.is_open()) {
		myFile << to_string(elapsedTime);
		myFile.close();
	}
}

// sound functions
void playBtnSound() {
	PlaySoundA(NULL, NULL, 0);
	PlaySoundA(btnClickWav_path, NULL, SND_SYNC | SND_NODEFAULT | SND_NOSTOP);
}

void playCarSoundEffect() {
	PlaySoundA(carSoundEffect_path, NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
}

void playMenuBgMusic() {
	PlaySoundA(menuBgMusic_path, NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);
	//PlaySound(menuBgMusic_path, NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP);
}

// StartGame function serves as transition to start a game session
void StartGame() {
	shuffleStartXs();
	for (int i = 0; i < nObstacles; i++)
		obstacles[i].SetPosition(vec2(obstacleStartXs[i], 1.5f));
	player.SetPosition(vec2(startX, startY));
	elapsedTime = 0;
	startTime = clock();
	loopDuration = baseLoopDuration;
	gameover = false;
	intersected = NULL;
	// extract game record
	checkGameRecord();
	// play bg music and car sound effect
	playCarSoundEffect();
}

// Mouse Handlers
void MouseButton(float x, float y, bool left, bool down) { 
	if (left && down) {
		if (startBt.Hit(x, y)) {
			playBtnSound();
			gamerunning = true;
			StartGame();
		}
		if (endBt.Hit(x, y)) {
			playBtnSound();
			terminateGame = true;
		}
		if (replayBt.Hit(x, y)) {
			playBtnSound();
			StartGame();
		}	
		if (returnToMenuBt.Hit(x, y)) {
			playBtnSound();
			playMenuBgMusic();
			gamerunning = false;
		}
	}
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	hoveringStartBt = startBt.Hit(x, y);
	hoveringEndBt = endBt.Hit(x, y);
	hoveringreplayBt = replayBt.Hit(x, y);
	hoveringreturnToMenuBt = returnToMenuBt.Hit(x, y);
}

// scrolling function - handle forward motion in game play
void Scroll() {
	// increase speed with time, until loopDuration is smaller than 1(150s)
	if (loopDuration > 1)
		loopDuration = baseLoopDuration - elapsedTime * 0.02;

	// scroll background
	float u = 0, v = elapsedTime/loopDuration;
	background.uvTransform = Translate(u, v, 0);

	// move active obstacle(s) with background
	for (int i = 0; i < nObstacles; i++)
		if (elapsedTime > obstacleDelays[i]) {
			float obstacleScrollTime = elapsedTime - obstacleDelays[i];
			float vObstacle = fmod(obstacleScrollTime/loopDuration, 1.f);
			if (1 - 2 * vObstacle <= -0.99) {
				float last = obstacleStartXs[3];
				obstacleStartXs[3] = obstacleStartXs[i];
				obstacleStartXs[i] = last;
				obstacles[i].SetPosition(vec2(obstacleStartXs[i], 1.5f));
			}
			else
				obstacles[i].SetPosition(vec2(obstacleStartXs[i], 1 - 2 * vObstacle));
		}
}

// handle game elapased time, scrolling, and player keyboard inputs
void Animate() {
	if (!gameover) {
		elapsedTime = (float)(clock() - startTime)/CLOCKS_PER_SEC;
		Scroll();
		vec2 p = player.GetPosition();
		if (KeyDown(GLFW_KEY_LEFT) && p.x > leftBoundary)
			player.SetPosition(vec2(p.x - .007f, p.y));
		if (KeyDown(GLFW_KEY_RIGHT) && p.x < rightBoundary)
			player.SetPosition(vec2(p.x + .007f, p.y));
	}
}

// display & render main menu
void DisplayMenu() {
	menuBG.Display();
	startBt.Display();
	endBt.Display();
	if (hoveringStartBt)
		startBt.Outline(yellow, 2);
	if (hoveringEndBt)
		endBt.Outline(yellow, 2);
}

// display & render game play
void DisplayGame() {
	background.Display();
	player.Display();

	// display time and current game record
	Text(10, windowHeight-40, green, 13, "%s", "Score:");
	Text(90, windowHeight-40, green, 13, "%3.1f", elapsedTime);
	Text(windowWidth-120, windowHeight-40, green, 13, "%s", "Best:");
	Text(windowWidth-60, windowHeight-40, green, 13, "%3.1f", curRecord);

	// display obstacles
	for (Sprite &s : obstacles)
		s.Display();
	if (!gameover)
		// test for intersection
		for (int i = 0; i < nObstacles && !intersected; i++)
			if (elapsedTime > obstacleDelays[i] && obstacles[i].Intersect(player))
				intersected = &obstacles[i];
		
	if (gameover || intersected) {
		// game over
		if (!gameover) {
			// compare the current time to game record
			if (elapsedTime > curRecord) {
				writeGameRecord();
			}
			PlaySoundA(NULL, NULL, 0);
			PlaySoundA(gameOverWav_path, NULL, SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
		}
		gameover = true;
		// display red outline of obstacle sprite object if collision occurs
		intersected -> Outline(red, 2);
		explosion.Display();
		replayBt.Display();
		returnToMenuBt.Display();
		// display congratulation sprite
		if (elapsedTime > curRecord) {
			congratz.Display();
		}
		// display player socre
		Text(windowWidth/2-80, windowHeight/2, red, 16, "%s", "Your score");
		Text(windowWidth/2-30, windowHeight/2-25, red, 16, "%3.1f", elapsedTime);
		if (hoveringreplayBt) 
			replayBt.Outline(yellow, 2);
		if (hoveringreturnToMenuBt)
			returnToMenuBt.Outline(yellow, 2);
	}
	glFlush();
}

// main display & render function
void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (!gamerunning)
		DisplayMenu();
	else
		DisplayGame();
}

// initialize menu resource
void InitializeMenuSprites() {
	menuBG.Initialize(menuBG_path);
	startBt.Initialize(startBt_path);
	endBt.Initialize(endBt_path);
	startBt.SetPosition(vec2(-0.5f, 0.0f));
	endBt.SetPosition(vec2(0.5f, 0.0f));
	startBt.SetScale(vec2(0.31f, 0.1f));
	endBt.SetScale(vec2(0.31f, 0.1f));
}

// initialize obstacle(s) sprites
void InitializeObstacle(Sprite &s, const char *name, float startX) {
	s.Initialize(dir+string(name));
	s.SetScale(.1f);
}

// initialize game play sprites
void InitializeGameSprites() {
	background.Initialize(backgroundImg_path);
	player.Initialize(playerImg_path);
	player.SetScale(.1f);
	player.SetPosition(vec2(startX, startY));
	obstacles.resize(nObstacles);
	for (int i = 0; i < nObstacles; i++)
		InitializeObstacle(obstacles[i], obstacleNames[i], obstacleStartXs[i]);
	explosion.Initialize(explosionImg_path);
	explosion.SetPosition(vec2(.0f, .45f));
	explosion.SetScale(.5f);
	congratz.Initialize(congratz_path);
	congratz.SetPosition(vec2(.0f, -.8f));
	congratz.SetScale(vec2(0.88f, 0.2f));
	replayBt.Initialize(buttonreplay_path);
	replayBt.SetPosition(vec2(.0f, -0.2f));
	replayBt.SetScale(vec2(0.32f, 0.08f));
	returnToMenuBt.Initialize(buttonmenu_path);
	returnToMenuBt.SetPosition(vec2(.0f, -0.4f));
	returnToMenuBt.SetScale(vec2(0.32f, 0.08f));
}

// Application
void Resize(int width, int height) { glViewport(0, 0, width, height); }

int main(int ac, char** av) {
	GLFWwindow *w = InitGLFW(450, 200, windowWidth, windowHeight, "Doggy");
	glfwSetWindowSizeLimits(w, 450, 200, windowWidth, windowHeight);

	// read background, foreground, and mat textures
	InitializeMenuSprites();
	InitializeGameSprites();

	// callbacks
	RegisterResize(Resize);
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);

	// start playing menu BGM
	playMenuBgMusic();

	// main/master event loop
	while (!glfwWindowShouldClose(w) && !terminateGame) {
		Animate();
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	glfwDestroyWindow(w);
	glfwTerminate();
}
