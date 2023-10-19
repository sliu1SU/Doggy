#pragma once
#include <glad.h>                                         
#include <glfw3.h>                                        
#include "GLXtras.h"  
#include "IO.h"
#include "Config.h"
using namespace std;

class Menu {
private:
	const char *vertexShader = R"(
		#version 330 core

		in vec3 aPos;
		in vec2 uv;
		
		out vec2 vuv;
		
		
		void main()
		{
		    gl_Position = vec4(aPos, 1.0f);
		    vuv = uv;
		}
	)";
	const char *pixelShader = R"(
		#version 330 core

		out vec4 FragColor;
		
		in vec2 vuv;
		uniform sampler2D textureImage;
		void main()
		{
		    FragColor = texture(textureImage, vuv);
		}
	)";
	const char *pixFiles[3] = {
		"Image/start-bg-w.png",
		"Image/button-play.png",
		"Image/button-exit.png"
	};
	GLuint program = 0;
	GLuint textureNames[3] = { 0, 0, 0 }, textureUnit = 0;
	GLuint vao, vbo, ebo;
	const unsigned int indices[6] = {
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	// draw rectangle; (x,y): upper left coordinates;
	void display(float x, float y, float width, float height, GLuint textureName);
public:
	Menu();
	~Menu();
	void draw();
	bool isClickStart(float x, float y);
	bool isClickExit(float x, float y);
};