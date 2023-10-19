#include "Menu.h"

Menu::Menu()
{
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	glUseProgram(program);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	textureNames[0] = ReadTexture(pixFiles[0]);
	textureNames[1] = ReadTexture(pixFiles[1]);
	textureNames[2] = ReadTexture(pixFiles[2]);

	VertexAttribPointer(program, "aPos", 3, 5 * sizeof(float), (void*)0);
	VertexAttribPointer(program, "uv", 2, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

Menu::~Menu()
{
}

void Menu::display(float x, float y, float width, float height, GLuint textureName)
{
	glBindVertexArray(vao);

	int constNum = 20;
	int bufferSize = sizeof(float) * constNum;
	float* vertices = (float*)malloc(bufferSize);

	// upper right
	vertices[0] = x + width;
	vertices[1] = y;
	vertices[2] = 0;
	vertices[3] = 1;
	vertices[4] = 1;

	// bottom right
	vertices[5] = x + width;
	vertices[6] = y - height;
	vertices[7] = 0;
	vertices[8] = 1;
	vertices[9] = 0;

	// bottom left
	vertices[10] = x;
	vertices[11] = y - height;
	vertices[12] = 0;
	vertices[13] = 0;
	vertices[14] = 0;

	// upper left
	vertices[15] = x;
	vertices[16] = y;
	vertices[17] = 0;
	vertices[18] = 0;
	vertices[19] = 1;

	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);
	VertexAttribPointer(program, "aPos", 3, 5 * sizeof(float), (void*)0);
	VertexAttribPointer(program, "uv", 2, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	SetUniform(program, "textureImage", (int)textureUnit);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureName);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	free(vertices);

	glBindVertexArray(0);
}

void Menu::draw()
{
	display(MENU_BG_X, MENU_BG_Y, MENU_BG_W, MENU_BG_H, textureNames[0]);
	display(MENU_BTN_START_X, MENU_BTN_START_Y, MENU_BTN_START_W, MENU_BTN_START_H, textureNames[1]);
	display(MENU_BTN_EXIT_X, MENU_BTN_EXIT_Y, MENU_BTN_EXIT_W, MENU_BTN_EXIT_H, textureNames[2]);
}

bool Menu::isClickStart(float x, float y) {
	float mx = (MENU_BTN_START_X + 1) * windowWidth / 2;
	float my = (MENU_BTN_START_Y + 1) * windowHeight / 2;
	float ex = (MENU_BTN_START_X + MENU_BTN_START_W + 1) * windowWidth / 2;
	float ey = (MENU_BTN_START_Y - MENU_BTN_START_H + 1) * windowHeight / 2;
	if (x >= mx && x <= ex && y >= ey && y <= my)
		return true;
	return false;
}

bool Menu::isClickExit(float x, float y) {
	float mx = (MENU_BTN_EXIT_X + 1) * windowWidth / 2;
	float my = (MENU_BTN_EXIT_Y + 1) * windowHeight / 2;
	float ex = (MENU_BTN_EXIT_X + MENU_BTN_EXIT_W + 1) * windowWidth / 2;
	float ey = (MENU_BTN_EXIT_Y - MENU_BTN_EXIT_H + 1) * windowHeight / 2;
	if (x >= mx && x <= ex && y >= ey && y <= my)
		return true;
	return false;
}
