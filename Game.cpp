#include "Game.h"

Game::Game():menu()
{
	isPlay = false;
}

Game::~Game()
{
}

void Game::draw()
{
	menu.draw();
	drawGameStage();
}

void Game::drawGameStage()
{

}

void Game::startGame()
{
}

void Game::play()
{
}
