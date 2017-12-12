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
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();




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
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 64);
	}


	// Store the button textures
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" };
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" };
	btnPos = { { 400, 375 },{ 400, 300 },{ 400, 300 },{ 500, 500 },{ 400, 300 },{ 740, 500 },{ 400, 300 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = MENU;
	theBtnType = EXIT;

	// Create text Textures
	gameTextNames = { "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt" };
	gameTextList = { "PHANTOM FOREST", "Catch the falling ingredients!", "Left and Right arrow keys to move", "Thanks for playing!", "See you again soon!" };
	for (int text = 0; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 }));
	}

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
	loop = true;


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

	switch (theGameState)
	{
	case MENU:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CreateTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Button
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render each bullet in the vector array

		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}

		// Render the time left
		string timeRemaining = "Time remaining: " + std::to_string(timeLeft);
		theTextureMgr->addTexture("timeLeft", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, timeRemaining.c_str(), SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 255 }));
		cTexture* timeLeftTexture = theTextureMgr->getTexture("timeLeft");
		SDL_Rect timePos = { 30, 10, timeLeftTexture->getTextureRect().w, timeLeftTexture->getTextureRect().h };
		FPoint timeScale = { 1,1 };
		timeLeftTexture->renderTexture(theRenderer, timeLeftTexture->getTexture(), &timeLeftTexture->getTextureRect(), &timePos, timeScale);


		// Render the score
		string theScore = "Score: " + std::to_string(playerScore);
		theTextureMgr->addTexture("playerScore", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, theScore.c_str(), SOLID, { 155, 0, 0, 255 }, { 0, 0, 0, 255 }));
		cTexture* playerScoreTexture = theTextureMgr->getTexture("playerScore");
		SDL_Rect scorePos = { 800, 10, playerScoreTexture->getTextureRect().w, playerScoreTexture->getTextureRect().h };
		FPoint scoreScale = { 1,1 };
		playerScoreTexture->renderTexture(theRenderer, playerScoreTexture->getTexture(), &playerScoreTexture->getTextureRect(), &scorePos, scoreScale);

		theTextureMgr->deleteTexture("playerScore");

		// render the rocket
		theWizard.render(theRenderer, &theWizard.getSpriteDimensions(), &theWizard.getSpritePos(), theWizard.getSpriteRotAngle(), &theWizard.getSpriteCentre(), theWizard.getSpriteScale());
	}
	break;
	case END:
	{
		string theScore = "Score: " + std::to_string(playerScore);
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theTextureMgr->addTexture("playerScore", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, theScore.c_str(), SOLID, { 155, 0, 0, 255 }, { 0, 0, 0, 255 }));
		cTexture* playerScoreTexture = theTextureMgr->getTexture("playerScore");
		SDL_Rect scorePos = { 300, 140, playerScoreTexture->getTextureRect().w, playerScoreTexture->getTextureRect().h };
		FPoint scoreScale = { 1,1 };
		playerScoreTexture->renderTexture(theRenderer, playerScoreTexture->getTexture(), &playerScoreTexture->getTextureRect(), &scorePos, scoreScale);

		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}

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

	switch (theGameState)
	{
	case MENU:
	{
		theWizard.setSpritePos({ 470, 600 });
		timeLeft = 60;
		playerScore = 0;
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	}
	break;
	case PLAYING:
	{



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
				// if a collision set the bullet to false
				(*bulletIterartor)->setActive(false);
				theSoundMgr->getSnd("fruitCollect")->play(0);
				playerScore++;

			}

			//remove any ingredients that reach the bottom of the screen without colliding with player
			if ((*bulletIterartor)->getBoundingRect().y > 1000 || timeLeft <= 0)
			{
				(*bulletIterartor)->setActive(false);
			}

		}

		// Update the wizards position
		theWizard.update(deltaTime);
		if (theWizard.getSpritePos().x > 900)
		{
			theWizard.setSpritePos({ 900, theWizard.getSpritePos().y });
		}
		if (theWizard.getSpritePos().x < -50)
		{
			theWizard.setSpritePos({ -50, theWizard.getSpritePos().y });
		}

		if (timeLeft <= 0)
		{
			theGameState = END;
		}

		OutputDebugString(std::to_string(spawnRate).c_str());
		OutputDebugString("\n");

		
		//SPAWN FRUIT AT A CERTAIN RATE
		//rate of spawn is proportional to the time remaining up until a certain point
		spawnRate = timeLeft * 16;
		if (spawnRate < 300)
		{
			spawnRate = 300;
		}
		if (GetTickCount() - startTicks >= spawnRate)
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

			theBullets[numBullets]->setSpriteTranslation({ 2, -15 });
			theBullets[numBullets]->setTexture(theTextureMgr->getTexture(textureName[rand() % 3 + 7]));
			theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("orange")->getTWidth(), theTextureMgr->getTexture("orange")->getTHeight());
			theBullets[numBullets]->setBulletVelocity({ 2, 2 });
			theBullets[numBullets]->setSpriteRotAngle(theWizard.getSpriteRotAngle());
			theBullets[numBullets]->setActive(true);
			cout << "Bullet added to Vector at position - x: " << theWizard.getBoundingRect().x << " y: " << theWizard.getBoundingRect().y << endl;

			theSoundMgr->getSnd("shot")->play(0);

			startTicks = GetTickCount();  //reset startTicks back up to the current tick count

		}

		//Timer
		if (GetTickCount() - timerTicks >= 1000)
		{
			timeLeft--;
			timerTicks = GetTickCount();
			theTextureMgr->deleteTexture("timeLeft");
		}
	}
	break;
	case END:
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
	}
	break;
	case QUIT:
	{
	}
	break;
	default:
		break;
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
					theAreaClicked = { event.motion.x, event.motion.y };
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

