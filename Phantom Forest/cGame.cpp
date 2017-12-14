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
	theGameState = MENU;  //the game starts in menu state
	theBtnType = EXIT;

	// Create text Textures
	gameTextNames = { "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt" };
	gameTextList = { "PHANTOM FOREST", "Catch the falling ingredients!", "Left and Right arrow keys to move", "Thanks for playing!", "See you again soon!" };
	for (int text = 1; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 }));
	}
	theTextureMgr->addTexture("TitleTxt", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 160,0,0,255 }, { 0,0,0,0 }));

	// Load game sounds
	soundList = { "theme", "fruitCollect", "fruitDrop" };
	soundTypes = { MUSIC, SFX, SFX, SFX };
	soundsToUse = { "Audio/BackgroundTheme.wav", "Audio/fruitcollect.wav", "Audio/fruitdrop.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);  //loops through sound list to add sounds to the game
	}

	theSoundMgr->getSnd("theme")->play(-1); //play the theme song throughout the game

	//set up the background of the game
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	//set up the wizard sprite and location
	theWizard.setSpritePos({ 470, 600 });
	theWizard.setTexture(theTextureMgr->getTexture("theWizard"));
	theWizard.setSpriteDimensions(theTextureMgr->getTexture("theWizard")->getTWidth(), theTextureMgr->getTexture("theWizard")->getTHeight());
	theWizard.setRocketVelocity({ 0, 0 });
	theWizard.setSpriteScale({ 1,1 });
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

	switch (theGameState) //checks what state the game is in and renders the approriate things on screen
	{
	case MENU:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); //render the background

		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Render the welcome message
		tempTextTexture = theTextureMgr->getTexture("CreateTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Render the instructions and controls
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// Render play and exit buttons in the main menu
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale()); //render the game background

		// Render each bullet in the vector array
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}

		// Render the time left
		string timeRemaining = "Time remaining: " + std::to_string(timeLeft);  //creates a string with a message and the time left (converted to string)
		theTextureMgr->addTexture("timeLeft", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, timeRemaining.c_str(), SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 255 }));  //creates the texture for the time left
		cTexture* timeLeftTexture = theTextureMgr->getTexture("timeLeft");
		SDL_Rect timePos = { 30, 10, timeLeftTexture->getTextureRect().w, timeLeftTexture->getTextureRect().h }; //position of the timer
		FPoint timeScale = { 1,1 };
		timeLeftTexture->renderTexture(theRenderer, timeLeftTexture->getTexture(), &timeLeftTexture->getTextureRect(), &timePos, timeScale);

		// Render the score
		string theScore = "Score: " + std::to_string(playerScore);  //converts players score to a string to be displayed
		theTextureMgr->addTexture("playerScore", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, theScore.c_str(), SOLID, { 155, 0, 0, 255 }, { 0, 0, 0, 255 }));
		cTexture* playerScoreTexture = theTextureMgr->getTexture("playerScore");
		SDL_Rect scorePos = { 800, 10, playerScoreTexture->getTextureRect().w, playerScoreTexture->getTextureRect().h };
		FPoint scoreScale = { 1,1 };
		playerScoreTexture->renderTexture(theRenderer, playerScoreTexture->getTexture(), &playerScoreTexture->getTextureRect(), &scorePos, scoreScale);

		theTextureMgr->deleteTexture("playerScore");  //this clears the texture before updating again, this is so if replayed the score displays as 0

		// render the rocket
		theWizard.render(theRenderer, &theWizard.getSpriteDimensions(), &theWizard.getSpritePos(), theWizard.getSpriteRotAngle(), &theWizard.getSpriteCentre(), theWizard.getSpriteScale());
	}
	break;
	case END:
	{

		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());  //renders the background again

		//renders the title on the screen
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//a simple thanks for playing message
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//a farewell message!
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//renders the players final score in the same was as when the game is playing
		string theScore = "Score: " + std::to_string(playerScore);
		theTextureMgr->addTexture("playerScore", theFontMgr->getFont("Amatic")->createTextTexture(theRenderer, theScore.c_str(), SOLID, { 155, 0, 0, 255 }, { 0, 0, 0, 255 }));
		cTexture* playerScoreTexture = theTextureMgr->getTexture("playerScore");
		SDL_Rect scorePos = { 300, 140, playerScoreTexture->getTextureRect().w, playerScoreTexture->getTextureRect().h };
		FPoint scoreScale = { 1,1 };
		playerScoreTexture->renderTexture(theRenderer, playerScoreTexture->getTexture(), &playerScoreTexture->getTextureRect(), &scorePos, scoreScale);

		//renders menu and exit buttons once the game is over
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT:
	{
		loop = false;  //ends the game
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
			bulletIterartor = theBullets.erase(bulletIterartor); //erases fruit from the screen
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
		theWizard.setSpritePos({ 470, 600 });  //wizard position resets to the centre
		timeLeft = 60;  //timer is reset when the player returns to menu
		playerScore = 0;  // score is reset when the player returns to menu

		//changes the game state when certain buttons are pressed
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
			if (theWizard.collidedWith(&(&theWizard)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
			{
				// if a collision set the bullet to false, play a confirming sound and increase player score by 1
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

		// Update the wizards position, stops the player moving off screen
		theWizard.update(deltaTime);
		if (theWizard.getSpritePos().x > 900)
		{
			theWizard.setSpritePos({ 900, theWizard.getSpritePos().y });
		}
		if (theWizard.getSpritePos().x < -50)
		{
			theWizard.setSpritePos({ -50, theWizard.getSpritePos().y });
		}

		//changes the game state to END once the timer hits 0
		if (timeLeft <= 0)
		{
			theGameState = END;
		}

		//SPAWN FRUIT AT A CERTAIN RATE
		spawnRate = (timeLeft * 16) + rand() % 100 - 0; //the spawn rate is proportional to the time remaining, also element of randomness added
		if (spawnRate < 300)
		{
			spawnRate = 300; //stops the spawn rate from getting too high
		}
		if (GetTickCount() - startTicks >= spawnRate) //compares the number of ticks passed since the last fruit spawn with the current tick count and if its over a certain amount, fruit will spawn
		{
			theSoundMgr->getSnd("fruitDrop")->play(0); // play fruit drop sound
			theBullets.push_back(new cBullet);
			int numBullets = theBullets.size() - 1;

			theBullets[numBullets]->setSpritePos({ rand() % (theWizard.getSpritePos().x + 470) + (theWizard.getSpritePos().x - 470) , 200 });  //spawns fruit within the wizards vicinity
			if (theBullets[numBullets]->getSpritePos().x < 0)
			{
				theBullets[numBullets]->setSpritePos({ rand() % 670 + 0, 200 });  //if fruit would spawn off screen, spawn it at a random point between the edge and centre of the screen
			}

			if (theBullets[numBullets]->getSpritePos().x > 940)
			{
				theBullets[numBullets]->setSpritePos({ rand() % 940 + 270, 200 });  //if fruit would spawn off screen, spawn it at a random point between the edge and centre of the screen
			}

			theBullets[numBullets]->setSpriteTranslation({ 2, -15 });  //fall speed of the fruit
			theBullets[numBullets]->setTexture(theTextureMgr->getTexture(textureName[rand() % 3 + 7]));  //gives the falling fruit one of three random textures
			theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("orange")->getTWidth(), theTextureMgr->getTexture("orange")->getTHeight());
			theBullets[numBullets]->setBulletVelocity({ 2, 2 });
			theBullets[numBullets]->setSpriteRotAngle(theWizard.getSpriteRotAngle());
			theBullets[numBullets]->setActive(true);
			cout << "Bullet added to Vector at position - x: " << theWizard.getBoundingRect().x << " y: " << theWizard.getBoundingRect().y << endl;

			startTicks = GetTickCount();  //reset startTicks back up to the current tick count
		}

		//a timer that loops every 1000 ticks, or 1 second
		if (GetTickCount() - timerTicks >= 1000)
		{
			timeLeft--; //reduce the time left by 1
			timerTicks = GetTickCount();  //reset timers tick count to the current tick count
			theTextureMgr->deleteTexture("timeLeft"); //clear texture before updating it
		}
	}
	break;
	case END:
	{
		//display exit and menu buttons on the screen
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

		//if the right arrow key is pressed, move the wizard 15 pixels to the right per update
		if (keystate[SDL_SCANCODE_RIGHT])  
		{
			theWizard.setSpritePos({ theWizard.getSpritePos().x + 15, theWizard.getSpritePos().y });
		}

		//if the left arrow key is pressed, move the wizard 15 pixels to the left per update
		if (keystate[SDL_SCANCODE_LEFT])
		{
		theWizard.setSpritePos({ theWizard.getSpritePos().x - 15, theWizard.getSpritePos().y });
		}


	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		//if esc is pressed exit the game
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				
				//set the area clicked to the location on the screen that the left mouse button is pressed
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
				}
				break;
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
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

