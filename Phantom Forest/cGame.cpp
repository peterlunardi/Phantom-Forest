/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"


cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{
	cout << getElapsedSeconds();
}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "asteroid1", "asteroid2", "asteroid3", "asteroid4", "bullet","theWizard","theBackground", "apple", "berry", "orange"};
	texturesToUse = { "Images\\asteroid1.png", "Images\\asteroid2.png", "Images\\asteroid3.png", "Images\\asteroid4.png", "Images\\bullet.png", "Images\\Wizard.png", "Images\\Background.png", "Images\\greenapple.png", "Images\\berry.png" , "Images\\orange.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge", "Amatic" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/Amatic-Bold.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48);
	}
	gameTextList = { "Time remaining:"  , "Score: " };
	
	theTextureMgr->addTexture("Title", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 }));


	// Load game sounds
	soundList = { "theme", "shot", "explosion", "fruitCollect" };
	soundTypes = { MUSIC, SFX, SFX, SFX };
	soundsToUse = { "Audio/BackgroundTheme.wav", "Audio/shot007.wav", "Audio/explosion2.wav", "Audio/fruitcollect.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theWizard.setSpritePos({ 470, 600 });
	theWizard.setTexture(theTextureMgr->getTexture("theWizard"));
	theWizard.setSpriteDimensions(theTextureMgr->getTexture("theWizard")->getTWidth(), theTextureMgr->getTexture("theWizard")->getTHeight());
	theWizard.setRocketVelocity({ 0, 0 });
	theWizard.setSpriteScale({ 1,1 });

	// Create vector array of textures

	for (int astro = 0; astro < 5; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 100 * (rand() % 5 + 1), 50 * (rand() % 5 + 1) });
		theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
		int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3, 3 });
		theAsteroids[astro]->setActive(false);
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	bool loop = true;


	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	// Render each asteroid in the vector array
	for (int draw = 0; draw < theAsteroids.size(); draw++)
	{
		theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
	}
	// Render each bullet in the vector array
	for (int draw = 0; draw < theBullets.size(); draw++)
	{
		theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
	}
	// Render the Title
	cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scale = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	// Render the score
	cTexture* tempScoreTexture = theTextureMgr->getTexture("Score");
	SDL_Rect posScore = { 600, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	FPoint scaleScore = { 1, 1 };
	tempTextTexture->renderTexture(theRenderer, tempScoreTexture->getTexture(), &tempTextTexture->getTextureRect(), &posScore, scaleScore);
	// render the rocket
	theWizard.render(theRenderer, &theWizard.getSpriteDimensions(), &theWizard.getSpritePos(), theWizard.getSpriteRotAngle(), &theWizard.getSpriteCentre(), theWizard.getSpriteScale());
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// Update the visibility and position of each asteriod
	vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
		}
	}



	// Update the visibility and position of each bullet
	vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	while (bulletIterartor != theBullets.end())
	{
		if ((*bulletIterartor)->isActive() == false)
		{
			bulletIterartor = theBullets.erase(bulletIterartor);
		}
		else
		{
			(*bulletIterartor)->update(deltaTime);
			++bulletIterartor;
		}
	}


	/*
	==============================================================
	| Check for collisions
	==============================================================
	*/
	for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	{
		//(*bulletIterartor)->update(deltaTime);

			if (theWizard.collidedWith(&(&theWizard)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*bulletIterartor)->setActive(false);
				theSoundMgr->getSnd("fruitCollect")->play(0);
			}

			//remove any ingredients that reach the bottom of the screen without colliding with player
			if ((*bulletIterartor)->getBoundingRect().y > 1000)
			{
				(*bulletIterartor)->setActive(false);
			}

	}

	// Update the Rockets position
	theWizard.update(deltaTime);

	//Spawn fruit every second
	for (int i = 0; i < 1; i++)
	{

		theBullets.push_back(new cBullet);
		int numBullets = theBullets.size() - 1;
		theBullets[numBullets]->setSpritePos({ rand() % (theWizard.getSpritePos().x + 470) + (theWizard.getSpritePos().x - 470) , 200 });
		if (theBullets[numBullets]->getSpritePos().x < 0)
		{
			theBullets[numBullets]->setSpritePos({ rand() % 670 + 0, 200 });
		}

		if (theBullets[numBullets]->getSpritePos().x > 940)
		{
			theBullets[numBullets]->setSpritePos({ rand() % 940 + 270, 200 });
		}

		theBullets[numBullets]->setSpriteTranslation({ 2, -6 });
		theBullets[numBullets]->setTexture(theTextureMgr->getTexture(textureName[rand() % 3 + 7]));
		theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("orange")->getTWidth(), theTextureMgr->getTexture("orange")->getTHeight());
		theBullets[numBullets]->setBulletVelocity({ 2, 2 });
		theBullets[numBullets]->setSpriteRotAngle(theWizard.getSpriteRotAngle());
		theBullets[numBullets]->setActive(true);
		cout << "Bullet added to Vector at position - x: " << theWizard.getBoundingRect().x << " y: " << theWizard.getBoundingRect().y << endl;

		theSoundMgr->getSnd("shot")->play(0);


	}
}



bool cGame::getInput(bool theLoop)
{

	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	if (keystate[SDL_SCANCODE_RIGHT])
	{
		theWizard.setSpritePos({ theWizard.getSpritePos().x + 15, theWizard.getSpritePos().y });
	}

	if (keystate[SDL_SCANCODE_LEFT])
	{
		theWizard.setSpritePos({ theWizard.getSpritePos().x - 15, theWizard.getSpritePos().y });
	}

	/*
	if (keystate[SDL_SCANCODE_SPACE])
	{
		theBullets.push_back(new cBullet);
		int numBullets = theBullets.size() - 1;
		theBullets[numBullets]->setSpritePos({ rand() % (theWizard.getSpritePos().x + 470) + (theWizard.getSpritePos().x - 470) , 200 });
		if (theBullets[numBullets]->getSpritePos().x < 0)
		{
			theBullets[numBullets]->setSpritePos({ rand() % 670 + 0, 200 });
		}

		if (theBullets[numBullets]->getSpritePos().x > 940)
		{
			theBullets[numBullets]->setSpritePos({ rand() % 940 + 270, 200 });
		}

		theBullets[numBullets]->setSpriteTranslation({ 2, -6 });
		theBullets[numBullets]->setTexture(theTextureMgr->getTexture(textureName[rand() % 3 + 7]));
		theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("orange")->getTWidth(), theTextureMgr->getTexture("orange")->getTHeight());
		theBullets[numBullets]->setBulletVelocity({ 2, 2 });
		theBullets[numBullets]->setSpriteRotAngle(theWizard.getSpriteRotAngle());
		theBullets[numBullets]->setActive(true);
		cout << "Bullet added to Vector at position - x: " << theWizard.getBoundingRect().x << " y: " << theWizard.getBoundingRect().y << endl;

		theSoundMgr->getSnd("shot")->play(0);
	}
	*/

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;

				case SDLK_SPACE:
				{

				}

				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

