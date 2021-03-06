#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>
#include <dos.h>
#include <time.h>

// Game specific includes
#include "asteroidsGame.h"


using namespace std;

class cGame
{
public:
	cGame();

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();
	bool loop;

	double startTicks = GetTickCount();
	double timerTicks = GetTickCount();

	

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;



	// Sprites for displaying background and rocket textures
	cSprite spriteBkgd;
	cRocket theWizard;
	cBullet theBullet;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	vector<LPCSTR> gameTextNames;
	vector<cBullet*> theBullets;
	vector<cRocket*> theWizardVector;
	int playerScore = 0;
	int timeLeft = 60;
	int spawnRate;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextList;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Game objects
	// Define the elements and there position in/on the array/map
	int renderWidth, renderHeight;
	// Create vector array of button textures
	vector<LPCSTR> btnNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector <cButton> theButtons;
	gameState theGameState;
	cTexture* tempTextTexture;
	btnTypes theBtnType;
	SDL_Rect pos;
	FPoint scale;
	SDL_Point theAreaClicked;
};

#endif
