// Sprite.cpp - 2D display and manipulate texture-mapped quad

#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "Draw.h"
#include "GLXtras.h"
#include "IO.h"
#include "Sprite.h"

Sprite background, player, obstacle, explosion, menuBG, startBt, endBt, replayBt, returnToMenuBt;

string backgroundImg_path = "Image/game-bg.png";
string playerImg_path = "Image/car-yellow.png";
string obstacleImg_path = "Image/car-purple.png";
string explosionImg_path = "Image/game-over.png";
string menuBG_path = "Image/start-bg-w.png";
string startBt_path = "Image/button-play.png";
string endBt_path = "Image/button-exit.png";
string buttonreplay_path = "Image/button-replay1.png";
string buttonmenu_path = "Image/button-menu.png";

constexpr float windowWidth = 600;
constexpr float windowHeight = 700;
constexpr float leftBoundary = -0.5;
constexpr float rightBoundary = 0.5;
constexpr float startX = -0.45;
constexpr float startY = -0.85;

bool isLeftKeyPressed = false;
bool isRightKeyPressed = false;
bool gameover = false;
bool gamerunning = false;
bool terminateGame = false;
bool hoveringStartBt = false;
bool hoveringEndBt = false;
bool hoveringreplayBt = false;
bool hoveringreturnToMenuBt = false;

float loopDuration = 2;
time_t startTime;
float elapsedTime = 0;
float vScrollMod = 0;
float obstacleDelay = 2; // wait till display obstacle (in secs.)

void Scroll() {
	// calculate u and v
	float v = elapsedTime / loopDuration, u = 0;
	vScrollMod = fmod(v, 1);
	// transform u and v
	background.uvTransform = Translate(u, v, 0);
	// scroll obstacle with background
	if (elapsedTime > obstacleDelay) {
		float obstacleTime = elapsedTime - obstacleDelay;
		obstacle.SetPosition(vec2(startX, 1 - vScrollMod * 2));
	}
}

void StartGame() {
	obstacle.SetPosition(vec2(startX, 1.0f));
	player.SetPosition(vec2(startX, startY));
	elapsedTime = 0;
	startTime = clock();
	gameover = false;
}

void MouseButton(float x, float y, bool left, bool down) { 
	if (left && down) {
		if (startBt.Hit(x, y)) {
			gamerunning = true;
			StartGame();
		}
		if (endBt.Hit(x, y)) {
			terminateGame = true;
		}
		if (replayBt.Hit(x, y)) {
			StartGame();
		}
		if (returnToMenuBt.Hit(x, y)) {
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

void Keyboard(int key, int action, bool shift, bool control) {
	// keyboard logic to control car movement
	if (gameover) {
		return;
	}
	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) {
			isLeftKeyPressed = true;
		}

		if (action == GLFW_RELEASE) {
			isLeftKeyPressed = false;
		}
	}

	if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) {
			isRightKeyPressed = true;
		}

		if (action == GLFW_RELEASE) {
			isRightKeyPressed = false;
		}
	}
}

void Outline(Sprite& s, float width = 2, vec3 color = vec3(1, 1, 0)) {
	UseDrawShader(mat4());
	vec2 pts[] = { s.PtTransform({-1,-1}), s.PtTransform({-1,1}), s.PtTransform({1,1}), s.PtTransform({1,-1}) };
	for (int i = 0; i < 4; i++)
		Line(pts[i], pts[(i + 1) % 4], width, color);
}

void DisplayMenu() {

	menuBG.Display();
	startBt.Display();
	endBt.Display();

	if (hoveringStartBt) {
		Outline(startBt);
	}

	if (hoveringEndBt) {
		Outline(endBt);
	}
}

void DisplayGame() {
	if (!gameover) {
		Scroll();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glClear(GL_DEPTH_BUFFER_BIT);
	// glEnable(GL_DEPTH_TEST);
	
	if (isLeftKeyPressed) {
		vec2 p = player.GetPosition();
		if (p.x > leftBoundary) {
			p.x -= 0.007;
			player.SetPosition(p);
		}
	}

	if (isRightKeyPressed) {
		vec2 p = player.GetPosition();
		if (p.x < rightBoundary) {
			p.x += 0.007;
			player.SetPosition(p);
		}
	}

	time_t now = clock();
	elapsedTime = (float)(now - startTime) / CLOCKS_PER_SEC; // update elapsed time
	
	background.Display();
	player.Display();
	if (elapsedTime > obstacleDelay) {
		obstacle.Display();
		if (obstacle.Intersect(player)){
			// display red outline of obstacle sprite object if collision occurs
			Outline(obstacle, 2, vec3(1, 0, 0));
			explosion.Display();
			replayBt.Display();
			returnToMenuBt.Display();
			isLeftKeyPressed = false;
			isRightKeyPressed = false;
			gameover = true;

			if (hoveringreplayBt) {
				Outline(replayBt);
			}

			if (hoveringreturnToMenuBt) {
				Outline(returnToMenuBt);
			}
		}
	}
	glFlush();
}

void InitializeMenuSprites() {
	menuBG.Initialize(menuBG_path);
	startBt.Initialize(startBt_path);
	endBt.Initialize(endBt_path);
	startBt.SetPosition(vec2(-0.5f, 0.0f));
	endBt.SetPosition(vec2(0.5f, 0.0f));
	startBt.SetScale(vec2(0.31f, 0.1f));
	endBt.SetScale(vec2(0.31f, 0.1f));
}

void InitializeGameSprites() {
	background.Initialize(backgroundImg_path);
	player.Initialize(playerImg_path);
	player.SetScale(.1f);
	player.SetPosition(vec2(startX, startY));
	obstacle.Initialize(obstacleImg_path);
	obstacle.SetScale(.1f);
	obstacle.SetPosition(vec2(startX, 1.0f));
	explosion.Initialize(explosionImg_path);
	explosion.SetPosition(vec2(.0f, .45f));
	explosion.SetScale(.5f);
	replayBt.Initialize(buttonreplay_path);
	replayBt.SetPosition(vec2(.0f, .0f));
	replayBt.SetScale(vec2(0.32f, 0.08f));
	returnToMenuBt.Initialize(buttonmenu_path);
	returnToMenuBt.SetPosition(vec2(.0f, -0.2f));
	returnToMenuBt.SetScale(vec2(0.32f, 0.08f));
}

// Application
void Resize(int width, int height) { glViewport(0, 0, width, height); }

int main(int ac, char** av) {
	GLFWwindow* w = InitGLFW(200, 200, windowWidth, windowHeight, "Doggy");
	
	// read background, foreground, and mat textures
	InitializeMenuSprites();
	InitializeGameSprites();

	// callbacks
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	// event loop
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w) && !terminateGame) {
		// display menu
		if (!gamerunning) {
			DisplayMenu();
		}
		else {
			DisplayGame();
		}
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	glfwDestroyWindow(w);
	glfwTerminate();
}
