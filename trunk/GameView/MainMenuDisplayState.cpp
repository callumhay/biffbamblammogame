/**
 * MainMenuDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MainMenuDisplayState.h"
#include "InGameDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameMenuItem.h"
#include "GameAssets.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/FBObj.h"
#include "../GameModel/GameModel.h"
#include "../GameControl/GameControllerManager.h"
#include "../GameSound/GameSoundAssets.h"
#include "../ESPEngine/ESPOnomataParticle.h"
#include "../WindowManager.h"

const int MainMenuDisplayState::MENU_SEL_ON_INDEX	= 0;
const int MainMenuDisplayState::MENU_SEL_OFF_INDEX	= 1;

const size_t MainMenuDisplayState::TOTAL_NUM_BANG_EFFECTS = 10;

// Menu items
const char* MainMenuDisplayState::NEW_GAME_MENUITEM		= "New Game";
const char* MainMenuDisplayState::PLAY_LEVEL_MENUITEM	= "Level Select";
const char* MainMenuDisplayState::OPTIONS_MENUITEM		= "Options";
const char* MainMenuDisplayState::BLAMMOPEDIA_MENUITEM  = "Blammopedia";
const char* MainMenuDisplayState::EXIT_MENUITEM			= "Exit Game";

const Colour MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR		= Colour(1, 0.65f, 0);
const Colour MainMenuDisplayState::MENU_ITEM_SEL_COLOUR			= Colour(1, 1, 0);
const Colour MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR	    = Colour(0.49f, 0.98f, 1.0f);
const Colour MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR	    = Colour(0.5f, 0.5f, 0.5f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_IDLE_COLOUR		= Colour(0.75f, 0.75f, 0.75f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_ACTIVE_COLOUR	= Colour(1, 1, 1);

const float MainMenuDisplayState::CAM_DIST_FROM_ORIGIN = 20.0f;

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : 
DisplayState(display), mainMenu(NULL), optionsSubMenu(NULL), 
mainMenuEventHandler(NULL), optionsMenuEventHandler(NULL), itemsEventHandler(NULL), particleEventHandler(NULL),
changeToPlayGameState(false), changeToBlammopediaState(false), changeToLevelSelectState(false),
menuFBO(NULL), bloomEffect(NULL),
particleSmallGrowth(1.0f, 1.3f), particleMediumGrowth(1.0f, 1.6f),
madeByTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
                GameViewConstants::GetInstance()->GAME_CREDITS_TEXT),
licenseLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
             GameViewConstants::GetInstance()->LICENSE_TEXT),
titleDisplay(1.0f)
{

    this->madeByTextLabel.SetColour(Colour(0,0,0));
    this->madeByTextLabel.SetScale(this->display->GetTextScalingFactor());
    this->licenseLabel.SetColour(Colour(0.33f, 0.33f, 0.33f));
    this->licenseLabel.SetScale(0.75f * this->display->GetTextScalingFactor());

	// Make sure the game state is cleared
	this->display->GetModel()->ClearGameState();

	// Setup any textures for rendering the menu screen
	this->bangTextures.reserve(3);
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture2D::Trilinear, GL_TEXTURE_2D));

	// Setup any emitter/sprite/particle effects
	this->InitializeESPEffects();

	// Read the configuration file to figure out how to initialize each of the options
	this->cfgOptions = ResourceManager::ReadConfigurationOptions(true);

	// Setup handlers for menus and initialize the menus
	this->mainMenuEventHandler    = new MainMenuEventHandler(this);
	this->optionsMenuEventHandler = new OptionsSubMenuEventHandler(this);
	this->itemsEventHandler       = new AllMenuItemsEventHandler(this);
	this->particleEventHandler    = new BangParticleEventHandler(this);
	this->InitializeMainMenu();

	// Setup the fade-in animation
	this->fadeAnimation.SetLerp(0.0, 3.0, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

	// Setup any fullscreen effects
	const Camera& camera = this->display->GetCamera();
	this->menuFBO = new FBObj(camera.GetWindowWidth(), camera.GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();

	// Load all the sound assets associated with the main menu
	GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
	soundAssets->LoadSoundPallet(GameSoundAssets::MainMenuSoundPallet);
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuBackgroundMusic);

	// Allocate memory...
	for (size_t i = 0; i < TOTAL_NUM_BANG_EFFECTS; i++) {
		this->deadBangEffects.push_back(new ESPPointEmitter());
		this->deadOnoEffects.push_back(new ESPPointEmitter());
	}
}

MainMenuDisplayState::~MainMenuDisplayState() {
	// Dispose of the menu object
	if (this->mainMenu != NULL) {
		delete this->mainMenu;
		this->mainMenu = NULL;
	}

	if (this->optionsSubMenu != NULL) {
		delete this->optionsSubMenu;
		this->optionsSubMenu = NULL;
	}

	delete this->mainMenuEventHandler;
	this->mainMenuEventHandler = NULL;
	delete this->optionsMenuEventHandler;
	this->optionsMenuEventHandler = NULL;
	delete this->itemsEventHandler;
	this->itemsEventHandler = NULL;
	delete this->particleEventHandler;
	this->particleEventHandler = NULL;

	delete this->menuFBO;
	this->menuFBO = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;

	// Release texture assets that we no longer need
	for (size_t i = 0; i < this->bangTextures.size(); i++) {
		bool releaseTexSuccess = ResourceManager::GetInstance()->ReleaseTextureResource(this->bangTextures[i]);
        UNUSED_VARIABLE(releaseTexSuccess);
		assert(releaseTexSuccess);
	}

	// Clean up any left over emitters
	for (std::list<ESPPointEmitter*>::iterator iter = this->aliveBangEffects.begin(); iter != this->aliveBangEffects.end(); ++iter) {
		delete *iter;
	}
	this->aliveBangEffects.clear();
	for (std::list<ESPPointEmitter*>::iterator iter = this->aliveOnoEffects.begin(); iter != this->aliveOnoEffects.end(); ++iter) {
		delete *iter;
	}
	this->aliveOnoEffects.clear();
	for(std::list<ESPPointEmitter*>::iterator iter = this->deadBangEffects.begin(); iter != this->deadBangEffects.end(); ++iter) {
		delete *iter;
	}
	this->deadBangEffects.clear();
	for(std::list<ESPPointEmitter*>::iterator iter = this->deadOnoEffects.begin(); iter != this->deadOnoEffects.end(); ++iter) {
		delete *iter;
	}
	this->deadOnoEffects.clear();
}

/**
 * Private helper to initialize any particle, sprite or general effects for the foreground
 * and background of the menu screen.
 */
void MainMenuDisplayState::InitializeESPEffects() {
	assert(this->bangTextures.size() == 3);
	
	// Setup any particle effectors
	std::vector<ColourRGBA> fadeInOutColours;
	fadeInOutColours.reserve(3);
	fadeInOutColours.push_back(ColourRGBA(1, 1, 1, 0));
	fadeInOutColours.push_back(ColourRGBA(1, 1, 1, 1));
	fadeInOutColours.push_back(ColourRGBA(1, 1, 1, 0));
	this->particleFadeInAndOut.SetColours(fadeInOutColours);
}

/**
 * Used as a private helper method to initialize the menu and other related display stuffs.
 */
void MainMenuDisplayState::InitializeMainMenu()  {
	// Setup the main menu attributes
	const Camera& camera = this->display->GetCamera();
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, camera.GetWindowHeight() - MENU_Y_INDENT);

	// Setup the main menu (top-most menu)
	this->mainMenu = new GameMenu(menuTopLeftCorner);
	this->mainMenu->AddEventHandler(this->mainMenuEventHandler);
	this->mainMenu->SetColourScheme(MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
	                                MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Setup submenues
	this->InitializeOptionsSubMenu();

	// Scale the labels based on the height/width of the window
	float textScaleFactor = this->display->GetTextScalingFactor();

	const float dropShadowAmtSm = textScaleFactor * 0.15f;
	const float dropShadowAmtLg = textScaleFactor * 0.15f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	// Add items to the menu in their order (first to last)
	TextLabel2D tempLabelSm(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),  NEW_GAME_MENUITEM);
	TextLabel2D tempLabelLg(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), NEW_GAME_MENUITEM);

	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelSm.SetScale(textScaleFactor);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);
	tempLabelLg.SetScale(textScaleFactor);

	this->newGameMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	tempLabelSm.SetText(PLAY_LEVEL_MENUITEM);
	tempLabelLg.SetText(PLAY_LEVEL_MENUITEM);
	this->playLevelMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

    // Place an item for the blammopedia
    tempLabelSm.SetText(BLAMMOPEDIA_MENUITEM);
    tempLabelLg.SetText(BLAMMOPEDIA_MENUITEM);

    TextLabel2D newLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "NEW");
    newLabel.SetDropShadow(dropShadowColour, 0.05f);
    newLabel.SetScale(0.8f * textScaleFactor);

    Blammopedia* blammopedia = ResourceManager::GetInstance()->GetBlammopedia();
    if (blammopedia->HasUnviewed()) {
        GameMenuItemWithFlashLabel* blammopediaMenuItem = new GameMenuItemWithFlashLabel(tempLabelSm, tempLabelLg, newLabel, NULL);
	    this->blammopediaItemIndex = this->mainMenu->AddMenuItem(blammopediaMenuItem);
    }
    else {
        this->blammopediaItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
    }

	// Setup the options item in the main menu and its submenu
	tempLabelSm.SetText(OPTIONS_MENUITEM);
	tempLabelLg.SetText(OPTIONS_MENUITEM);
	this->optionsMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, this->optionsSubMenu);

	tempLabelSm.SetText(EXIT_MENUITEM);
	tempLabelLg.SetText(EXIT_MENUITEM);

	VerifyMenuItem* exitMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	exitMenuItem->SetVerifyMenuColours(Colour(1,1,1), MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	exitMenuItem->SetVerifyMenuText("Are you sure you want to quit?", "Well, obviously.", "Hell no.");
	exitMenuItem->SetEventHandler(this->itemsEventHandler);
	this->exitGameMenuItemIndex = this->mainMenu->AddMenuItem(exitMenuItem);

	this->mainMenu->SetSelectedMenuItem(this->newGameMenuItemIndex);
	debug_opengl_state();
}

/**
 * Private helper function for initializing the options sub menu.
 */
void MainMenuDisplayState::InitializeOptionsSubMenu() {
	// Scale the labels based on the height/width of the window
	float textScaleFactor = this->display->GetTextScalingFactor();

	// Setup the options menu attributes
	const float dropShadowAmtSm = textScaleFactor * 0.10f;
	const float dropShadowAmtLg = textScaleFactor * 0.15f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	TextLabel2D subMenuLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),  "Fullscreen");
	TextLabel2D subMenuLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "Fullscreen");
	subMenuLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	subMenuLabelSm.SetScale(textScaleFactor);
	subMenuLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);
	subMenuLabelLg.SetScale(textScaleFactor);
	
	// Options Menu
	this->optionsSubMenu = new GameSubMenu();
	this->optionsSubMenu->AddEventHandler(this->optionsMenuEventHandler);
	this->optionsSubMenu->SetPaddingBetweenMenuItems(textScaleFactor*MENU_ITEM_PADDING);
	this->optionsSubMenu->SetColourScheme(MainMenuDisplayState::SUBMENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
	                                      MainMenuDisplayState::SUBMENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Add the toggle fullscreen item
	//subMenuLabelSm.SetText("TODO: Fullscreen:");
	//subMenuLabelLg.SetText("TODO: Fullscreen:");
	std::vector<std::string> fullscreenOptions;
	fullscreenOptions.reserve(2);
	fullscreenOptions.push_back("On");
	fullscreenOptions.push_back("Off");

	this->fullscreenMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, fullscreenOptions);
	this->fullscreenMenuItem->SetSelectedItem(this->cfgOptions.GetIsFullscreenOn() ? MENU_SEL_ON_INDEX : MENU_SEL_OFF_INDEX);
	this->fullscreenMenuItem->SetEventHandler(this->itemsEventHandler);
	this->optionsFullscreenIndex = this->optionsSubMenu->AddMenuItem(this->fullscreenMenuItem);

	// Add the vertical sync toggle item
	subMenuLabelSm.SetText("Vertical Sync");
	subMenuLabelLg.SetText("Vertical Sync");
	std::vector<std::string> vSyncOptions;
	vSyncOptions.reserve(2);
	vSyncOptions.push_back("On");
	vSyncOptions.push_back("Off");
	
	this->vSyncMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, vSyncOptions);
	this->vSyncMenuItem->SetSelectedItem(this->cfgOptions.GetIsVSyncOn() ? MENU_SEL_ON_INDEX : MENU_SEL_OFF_INDEX);
	this->vSyncMenuItem->SetEventHandler(this->itemsEventHandler);
	this->optionsVSyncIndex = this->optionsSubMenu->AddMenuItem(this->vSyncMenuItem);

	// Add the resolution selection item
	subMenuLabelSm.SetText("Resolution");
	subMenuLabelLg.SetText("Resolution");
	
	std::vector<std::string> resolutionOptions = WindowManager::GetInstance()->GetPossibleResolutionsList();

	// Try to figure out what index the resolution is set to (so we can set the menu item
	// to the appropriate index
	std::string cfgResolution = this->cfgOptions.GetResolutionString();
	int resIndex = -1;
	for (size_t i = 0; i < resolutionOptions.size(); i++) {
		if (resolutionOptions[i] == cfgResolution) {
			resIndex = i;
			break;
		}
	}

	// If we couldn't find the index to set then add a new
	// option to the menu and set it
	if (resIndex == -1) {
		resolutionOptions.reserve(resolutionOptions.size() + 1);
		resolutionOptions.push_back(cfgResolution);
		resIndex = resolutionOptions.size() - 1;
	}

	this->resolutionMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, resolutionOptions);
	this->resolutionMenuItem->SetSelectedItem(resIndex);
	this->resolutionMenuItem->SetEventHandler(this->itemsEventHandler);
	this->optionsResolutionIndex = this->optionsSubMenu->AddMenuItem(this->resolutionMenuItem);

	// Add a sound volume option
	subMenuLabelSm.SetText("Volume");
	subMenuLabelLg.SetText("Volume");

	int currentVolume = cfgOptions.GetVolume(); // Determine the volume of the sound...
	this->soundVolumeMenuItem = new AmountScrollerMenuItem(subMenuLabelSm, subMenuLabelLg,
		ConfigOptions::MIN_VOLUME, ConfigOptions::MAX_VOLUME, currentVolume, 5.0f);
	this->soundVolumeMenuItem->SetConstantChangeFeedback(true);
	this->soundVolumeMenuItem->SetEventHandler(this->itemsEventHandler);
	this->optionsSoundVolumeIndex = this->optionsSubMenu->AddMenuItem(this->soundVolumeMenuItem);

	// Add a controller sensitivity option
	//subMenuLabelSm.SetText("Controller Sensitivity");
	//subMenuLabelLg.SetText("Controller Sensitivity");

    //int currentSensitivity = 100;//cfgOptions.GetControllerSensitivity();
    //this->controllerSensitivityItem = new AmountScrollerMenuItem(subMenuLabelSm, subMenuLabelLg,
	//	ConfigOptions::MIN_CONTROLLER_SENSITIVITY, ConfigOptions::MAX_CONTROLLER_SENSITIVITY, currentSensitivity, 5.0f);
    //this->controllerSensitivityItem->SetConstantChangeFeedback(true);
    //this->optionsControllerSensitivityIndex = this->optionsSubMenu->AddMenuItem(this->controllerSensitivityItem);


    // Inverting ball boost option
	subMenuLabelSm.SetText("Invert Ball Boost");
	subMenuLabelLg.SetText("Invert Ball Boost");
    std::vector<std::string> invertOptions = ConfigOptions::GetOnOffItems();

    this->invertBallBoostItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, invertOptions);
    this->invertBallBoostItem->SetSelectedItem(this->cfgOptions.GetInvertBallBoost() ? 1 : 0);
    this->invertBallBoostItem->SetEventHandler(this->itemsEventHandler);

    this->optionsInvertBallBoostIndex = this->optionsSubMenu->AddMenuItem(this->invertBallBoostItem);


    // Difficulty option
	subMenuLabelSm.SetText("Difficulty");
	subMenuLabelLg.SetText("Difficulty");
    std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();

    this->difficultyItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, difficultyOptions);
    this->difficultyItem->SetSelectedItem(static_cast<int>(this->cfgOptions.GetDifficulty()));
	this->difficultyItem->SetEventHandler(this->itemsEventHandler);
	this->optionsDifficultyIndex = this->optionsSubMenu->AddMenuItem(this->difficultyItem);

	// Add an option for getting out of the menu
	subMenuLabelSm.SetText("Back");
	subMenuLabelLg.SetText("Back");
	this->optionsBackIndex = this->optionsSubMenu->AddMenuItem(subMenuLabelSm, subMenuLabelLg, NULL);

	// Set the selected index to be the first item in the menu
	this->optionsSubMenu->SetSelectedMenuItem(this->optionsFullscreenIndex);
}

void MainMenuDisplayState::SetupBloomEffect() {
	if (this->bloomEffect != NULL) {
		delete this->bloomEffect;
	}

	// Create the new bloom effect and set its parameters appropriately
	this->bloomEffect = new CgFxBloom(this->menuFBO);

	this->bloomEffect->SetHighlightThreshold(0.4f);
	this->bloomEffect->SetSceneIntensity(0.70f);
	this->bloomEffect->SetGlowIntensity(0.3f);
	this->bloomEffect->SetHighlightIntensity(0.1f);
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame(double dT) {
	bool finishFadeAnim = this->fadeAnimation.Tick(dT);

	// Check to see if we're switching game states...
    if (finishFadeAnim) {
	    if (this->changeToPlayGameState) {
    		
		    // Turn off all the sounds first (waiting for any unfinished sounds), then switch states
		    GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
		    soundAssets->StopAllSounds();

		    // Load all the initial stuffs for the game - this will queue up the next states that we need to go to
		    this->display->GetModel()->BeginOrRestartGame();

		    // Place the view into the proper state to play the game	
		    this->display->SetCurrentStateAsNextQueuedState();

		    return;
	    }
        else if (this->changeToBlammopediaState) {
		    // Turn off all the sounds first, then switch states
		    GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
		    soundAssets->StopAllSounds();
            // Change to the blammopedia state
            this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::BlammopediaMenu, this->display));
            return;
        }
        else if (this->changeToLevelSelectState) {
		    // Turn off all the sounds first, then switch states
		    GameSoundAssets* soundAssets = this->display->GetAssets()->GetSoundAssets();
		    soundAssets->StopAllSounds();
            // Change to the blammopedia state
            this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::SelectWorldMenu, this->display));
            return;
        }
    }
	
	const Camera& camera		= this->display->GetCamera();
	const int DISPLAY_WIDTH		= camera.GetWindowWidth();
	const int DISPLAY_HEIGHT	= camera.GetWindowHeight();

	this->menuFBO->BindFBObj();

	// Render background stuffs (behind the menu)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Camera menuCamera(camera.GetWindowWidth(), camera.GetWindowHeight());
	menuCamera.SetPerspective();
	menuCamera.Move(Vector3D(0, 0, CAM_DIST_FROM_ORIGIN));

	this->RenderBackgroundEffects(dT, menuCamera);
	this->RenderTitle(menuCamera);

	// Render the menu
	this->mainMenu->SetTopLeftCorner(MENU_X_INDENT, DISPLAY_HEIGHT - MENU_Y_INDENT);
	this->mainMenu->Draw(dT, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Render created by text
    static const int NAME_LICENCE_Y_PADDING = 5;
    static const int NAME_LICENCE_X_PADDING = 10;
    this->madeByTextLabel.SetTopLeftCorner(DISPLAY_WIDTH - (this->madeByTextLabel.GetLastRasterWidth() + NAME_LICENCE_X_PADDING),
        this->madeByTextLabel.GetHeight() + this->licenseLabel.GetHeight() + 2.5f*NAME_LICENCE_Y_PADDING);
    this->madeByTextLabel.Draw();
    this->licenseLabel.SetTopLeftCorner(DISPLAY_WIDTH - (this->licenseLabel.GetLastRasterWidth() + NAME_LICENCE_X_PADDING),
        this->licenseLabel.GetHeight() + NAME_LICENCE_Y_PADDING);
    this->licenseLabel.Draw();

	// Fade-in/out overlay
    this->DrawFadeOverlay(DISPLAY_WIDTH, DISPLAY_HEIGHT, this->fadeAnimation.GetInterpolantValue());
	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu screen and draw it
	this->bloomEffect->Draw(DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
	this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	debug_opengl_state();
}

/**
 * Renders the title for the menu screen.
 */
void MainMenuDisplayState::RenderTitle(Camera& menuCam) {
	const Camera& displayCam = this->display->GetCamera();

	const float MAX_Y_COORD = CAM_DIST_FROM_ORIGIN * tan(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS) / 2.0f);
	const float MAX_X_COORD = static_cast<float>(displayCam.GetWindowWidth()) / static_cast<float>(displayCam.GetWindowHeight()) * MAX_Y_COORD;
    const float MENU_WIDTH = 5.0f + (2*0.82f*this->titleDisplay.GetBlammoWidth()*MAX_X_COORD / static_cast<float>(displayCam.GetWindowWidth()));
	const float X_INDENT = -MAX_X_COORD + (2.0f * MAX_X_COORD - MENU_WIDTH) / 2.0f;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	menuCam.ApplyCameraTransform();
    this->titleDisplay.Draw(X_INDENT, MAX_Y_COORD, menuCam);
	glPopMatrix();
}

/**
 * Renders the background effects for the menu screen. These include
 * random bangs and booms, etc. going off all over the place.
 */
void MainMenuDisplayState::RenderBackgroundEffects(double dT, Camera& menuCam) {
	const Camera& camera = this->display->GetCamera();

	static const float MAX_Z_COORD = CAM_DIST_FROM_ORIGIN / 4.0f;
	static const float MIN_Z_COORD = -MAX_Z_COORD;
	static const float MAX_Y_COORD = 0.95f * CAM_DIST_FROM_ORIGIN * tan(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS) / 2.0f);
	static const float MIN_Y_COORD = -MAX_Y_COORD;
	const float MAX_X_COORD = static_cast<float>(camera.GetWindowWidth()) /  static_cast<float>(camera.GetWindowHeight()) * MAX_Y_COORD;
	const float MIN_X_COORD = -MAX_X_COORD;	
	
	// Insert a bunch of bang-onomatopiea effects for drawing if we're running low on them
	static const float MAX_Y_COORD_BG_EFFECTS = (MAX_Y_COORD - 6.0f);
	while (this->aliveOnoEffects.size() < TOTAL_NUM_BANG_EFFECTS) {
		assert(this->aliveBangEffects.size() == this->aliveOnoEffects.size());
		assert(this->deadBangEffects.size() == this->deadOnoEffects.size());
		this->InsertBangEffectIntoBGEffects(MIN_X_COORD, MAX_X_COORD, MIN_Y_COORD, MAX_Y_COORD_BG_EFFECTS, MIN_Z_COORD, MAX_Z_COORD);
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	menuCam.ApplyCameraTransform();

	// Go through all the active emitters and tick/draw them
	for (std::list<ESPPointEmitter*>::iterator iter = this->aliveBangEffects.begin(); iter != this->aliveBangEffects.end();) {
		ESPPointEmitter* currEmitter = *iter;
		currEmitter->Tick(dT);
		currEmitter->Draw(menuCam);

		if (currEmitter->IsDead()) {
			iter = this->aliveBangEffects.erase(iter);
			this->deadBangEffects.push_back(currEmitter);
		}
		else {
			++iter;
		}
	}
	for (std::list<ESPPointEmitter*>::iterator iter = this->aliveOnoEffects.begin(); iter != this->aliveOnoEffects.end();) {
		ESPPointEmitter* currEmitter = *iter;
		currEmitter->Tick(dT);
		currEmitter->Draw(menuCam);

		if (currEmitter->IsDead()) {
			iter = this->aliveOnoEffects.erase(iter);
			this->deadOnoEffects.push_back(currEmitter);
		}
		else {
			++iter;
		}
	}

	glPopMatrix();
}

/**
 * Insert bang effect into the background.
 */
void MainMenuDisplayState::InsertBangEffectIntoBGEffects(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
	// Choose a random bang texture
	unsigned int randomBangTexIndex = Randomizer::GetInstance()->RandomUnsignedInt() % this->bangTextures.size();
	Texture2D* randomBangTex = static_cast<Texture2D*>(this->bangTextures[randomBangTexIndex]);
	
	// Establish some of the values we will use to create the emitter for a 'BANG!" effect
	static const ESPInterval bangLifeInterval(2.0f, 3.5f);
	static const ESPInterval randomSpawnInterval(0.1f, 3.25f);
	ESPInterval xCoordInterval(minX, maxX);
	ESPInterval yCoordInterval(minY, maxY);
	ESPInterval zCoordInterval(minZ, maxZ);
	Point3D emitCenter  = Point3D(xCoordInterval.RandomValueInInterval(), yCoordInterval.RandomValueInInterval(), zCoordInterval.RandomValueInInterval());
	
	float randomSpawn = randomSpawnInterval.RandomValueInInterval();
	float randomLife  = bangLifeInterval.RandomValueInInterval();

	// Create an emitter for the bang texture
	ESPPointEmitter* bangEffect = this->deadBangEffects.front();
	this->deadBangEffects.pop_front();
	//new ESPPointEmitter();
	
	// Set up the emitter...
	bangEffect->SetSpawnDelta(ESPInterval(randomSpawn));
	bangEffect->SetNumParticleLives(1);
	bangEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangEffect->SetParticleLife(ESPInterval(randomLife));
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	bangEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangEffect->SetEmitPosition(emitCenter);

	// Figure out some random proper orientation...
	// Two base rotations (for variety) : 180 or 0...
	float baseBangRotation = 0.0f;
	if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
		baseBangRotation = 180.0f;
	}
	bangEffect->SetParticleRotation(ESPInterval(baseBangRotation - 10.0f, baseBangRotation + 10.0f));

	ESPInterval sizeIntervalX(3.5f, 3.8f);
	ESPInterval sizeIntervalY(1.9f, 2.2f);
	bangEffect->SetParticleSize(sizeIntervalX, sizeIntervalY);

	// Add the bang particle...
	if (!bangEffect->GetHasParticles()) {
		// Add effectors to the bang effect
		bangEffect->AddEffector(&this->particleFadeInAndOut);
		bangEffect->AddEffector(&this->particleMediumGrowth);
		bangEffect->SetParticles(1, randomBangTex);
	}

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = this->deadOnoEffects.front();
	this->deadOnoEffects.pop_front();
	//new ESPPointEmitter();

	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(randomSpawn));
	bangOnoEffect->SetNumParticleLives(1);
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangOnoEffect->SetParticleLife(ESPInterval(randomLife));
	bangOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ScreenAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	if (!bangOnoEffect->GetHasParticles()) {
		bangOnoEffect->AddEffector(&this->particleFadeInAndOut);
		bangOnoEffect->AddEffector(&this->particleSmallGrowth);

		// Add event handler
		bangOnoEffect->AddEventHandler(this->particleEventHandler);

		// Add the single text particle to the emitter with the severity of the effect...
		TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
		bangTextLabel.SetColour(Colour(1, 1, 1));
		bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
		
		Onomatoplex::Extremeness randomExtremeness = Onomatoplex::Generator::GetInstance()->GetRandomExtremeness(Onomatoplex::WEAK, Onomatoplex::UBER);
		bangOnoEffect->SetParticles(1, bangTextLabel, Onomatoplex::EXPLOSION, randomExtremeness);
	}

	// Add the particles to the list of background effects
	this->aliveBangEffects.push_back(bangEffect);
	this->aliveOnoEffects.push_back(bangOnoEffect);
}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
	assert(this->mainMenu != NULL);
	// Tell the main menu about the key pressed event
	this->mainMenu->ButtonPressed(pressedButton);
}

void MainMenuDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	assert(this->mainMenu != NULL);
	// Tell the main menu about the key pressed event
	this->mainMenu->ButtonReleased(releasedButton);
}

void MainMenuDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);
	const Camera& camera = this->display->GetCamera();

	delete this->menuFBO;
	this->menuFBO = new FBObj(camera.GetWindowWidth(), camera.GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
}

void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);
}

/**
 * Handle activation events for items in the main menu.
 */
void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	// Play the sound effect assoicated with menu item selection/activation
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	
	// Do the actual selection of the item
	if (itemIndex == this->mainMenuState->newGameMenuItemIndex) {
		debug_output("Selected " << NEW_GAME_MENUITEM << " from menu");
		soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);
		this->mainMenuState->changeToPlayGameState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, 2.0, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
	}
	else if (itemIndex == this->mainMenuState->playLevelMenuItemIndex) {
		debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
		soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);
        this->mainMenuState->changeToLevelSelectState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, 1.0, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
	}
	else if (itemIndex == this->mainMenuState->optionsMenuItemIndex) {
		debug_output("Selected " << OPTIONS_MENUITEM << " from menu");
		soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);
	}
    else if (itemIndex == this->mainMenuState->blammopediaItemIndex) {
        debug_output("Selected " << BLAMMOPEDIA_MENUITEM << " from menu");
        this->mainMenuState->changeToBlammopediaState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, 1.0, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
    }
	else if (itemIndex == this->mainMenuState->exitGameMenuItemIndex) {
		// We don't do anything since the user is currently being asked
		// for verification to quit the game
		debug_output("Selected " << EXIT_MENUITEM << " from menu");
		soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemEnteredEvent);
	}
	else {
		assert(false);
	}
}

void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);
	// Play the sound effect assoicated with menu item changing
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemChangedEvent);
};

void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemVerifiedEvent(int itemIndex) {
	if (itemIndex == this->mainMenuState->exitGameMenuItemIndex) {
		
		// Play the sound effect associated with menu item selection/activation
		// NOT WORKING PROPERLY...
		GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
		soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);

		// We exit the game if the exit game item has both been activated and verified...
		this->mainMenuState->display->QuitGame();
	}
}

void MainMenuDisplayState::MainMenuEventHandler::EscMenu() {
	GameMenu* mainMenu = this->mainMenuState->mainMenu;

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (mainMenu->GetSelectedMenuItem() != this->mainMenuState->exitGameMenuItemIndex) {
		mainMenu->SetSelectedMenuItem(this->mainMenuState->exitGameMenuItemIndex);
	}
	else {
		this->mainMenuState->mainMenu->ActivateSelectedMenuItem();
	}
}

void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);
}

/**
 * Handle activation events for items in the options menu.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->mainMenuState->optionsBackIndex) {
		// Go back to the main menu (deactivate any previously activated menu items)
		this->mainMenuState->optionsSubMenu->DeactivateSelectedMenuItem();
		this->mainMenuState->optionsSubMenu->SetSelectedMenuItem(0);
		this->mainMenuState->mainMenu->DeactivateSelectedMenuItem();
	}
	else {
		if (itemIndex == this->mainMenuState->optionsFullscreenIndex || 
				itemIndex == this->mainMenuState->optionsResolutionIndex ||
				itemIndex == this->mainMenuState->optionsVSyncIndex) {
			// Play the entered sound if it's an enterable item...
			GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
			soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemEnteredEvent);
		}
	}
}

/**
 * Handle changes in the options for the game.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
	// Play the sound effect assoicated with menu item changing
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemChangedEvent);

	if (itemIndex == this->mainMenuState->optionsFullscreenIndex) {
		int currSelectionIdx = this->mainMenuState->fullscreenMenuItem->GetSelectedItemIndex();

		// Make sure the changed fs option is the opposite of what is currently set
		assert(this->mainMenuState->cfgOptions.GetIsFullscreenOn() == (currSelectionIdx == MainMenuDisplayState::MENU_SEL_OFF_INDEX));
		
		// Set the fullscreen option and toggle fullscreen for the window
		this->mainMenuState->cfgOptions.SetIsFullscreenOn(currSelectionIdx == MainMenuDisplayState::MENU_SEL_ON_INDEX);
		bool toggleSuccess = WindowManager::GetInstance()->ToggleFullscreen();

		// The toggle usually fails - doesn't work in windows, so we need to reinitialize the whole windowing system
		if (!toggleSuccess) {
			this->mainMenuState->display->ReinitializeGame();
		}
	}
	else if (itemIndex == this->mainMenuState->optionsVSyncIndex) {
		int currSelectionIdx = this->mainMenuState->vSyncMenuItem->GetSelectedItemIndex();
		
		// Make sure the changed vSync option is the opposite of what is currently set
		assert(this->mainMenuState->cfgOptions.GetIsVSyncOn() == (currSelectionIdx == MainMenuDisplayState::MENU_SEL_OFF_INDEX));

		if (currSelectionIdx == MainMenuDisplayState::MENU_SEL_ON_INDEX) {
			BlammoTime::SetVSync(1);
			this->mainMenuState->cfgOptions.SetIsVSyncOn(true);
		}
		else {
			BlammoTime::SetVSync(0);
			this->mainMenuState->cfgOptions.SetIsVSyncOn(false);
		}
	}
    else if (itemIndex == this->mainMenuState->optionsInvertBallBoostIndex) {
        int currSelectionIdx = this->mainMenuState->invertBallBoostItem->GetSelectedItemIndex();
        bool isInverted = ConfigOptions::IsOnItemSelected(currSelectionIdx);

        GameModel* gameModel = this->mainMenuState->display->GetModel();
        gameModel->SetInvertBallBoostDir(isInverted);
        this->mainMenuState->cfgOptions.SetInvertBallBoost(isInverted);
    }
    else if (itemIndex == this->mainMenuState->optionsDifficultyIndex) {
        int currSelectionIdx = this->mainMenuState->difficultyItem->GetSelectedItemIndex();
        GameModel::Difficulty difficultyToSet = static_cast<GameModel::Difficulty>(currSelectionIdx);

        this->mainMenuState->cfgOptions.SetDifficulty(difficultyToSet);
        this->mainMenuState->display->GetModel()->SetDifficulty(difficultyToSet);
    }
	else if (itemIndex == this->mainMenuState->optionsResolutionIndex) {
		std::string currSelectionStr = this->mainMenuState->resolutionMenuItem->GetSelectedItemText();

		// Make sure the resolution option is different from what is currently set
		assert(this->mainMenuState->cfgOptions.GetResolutionString() != currSelectionStr);

		// Set the configuration options
		this->mainMenuState->cfgOptions.SetResolutionByString(currSelectionStr);
		// Restart the application
		this->mainMenuState->display->ReinitializeGame();
	}
	else if (itemIndex == this->mainMenuState->optionsSoundVolumeIndex) {
		int volumeLevel = static_cast<int>(this->mainMenuState->soundVolumeMenuItem->GetScrollerValue());
		
		// Set the configuration option
		this->mainMenuState->cfgOptions.SetVolume(volumeLevel);
		
		// Set the actual volume...
		//GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
		GameSoundAssets::SetGameVolume(volumeLevel);
	}
    /*
    else if (itemIndex == this->mainMenuState->optionsControllerSensitivityIndex) {
        // Controller sensitivity option...
        int sensitivityLvl = static_cast<int>(this->mainMenuState->controllerSensitivityItem->GetScrollerValue());
        
        // Set the configuration option
        //this->mainMenuState->cfgOptions.SetControllerSensitivity(sensitivityLvl);
        // Set the actual sensitivity...
        GameControllerManager::GetInstance()->SetControllerSensitivity(sensitivityLvl);
    }
    */
	else {
		assert(false);
	}

	// A configuration option has changed - rewrite the configuration file to accomodate the change
	ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->mainMenuState->cfgOptions);
}

void MainMenuDisplayState::OptionsSubMenuEventHandler::EscMenu() {
	GameSubMenu* optionsSubMenu = this->mainMenuState->optionsSubMenu;
	
	// Cause the menu to go back, if back isn't selected the select it...
	if (optionsSubMenu->GetSelectedMenuItem() != this->mainMenuState->optionsBackIndex) {
		optionsSubMenu->SetSelectedMenuItem(this->mainMenuState->optionsBackIndex);
	}
	else {
		this->mainMenuState->optionsSubMenu->ActivateSelectedMenuItem();
	}
}

void MainMenuDisplayState::AllMenuItemsEventHandler::MenuItemScrolled() {
	// Play the scroll sound...
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemScrolledEvent);
}

void MainMenuDisplayState::AllMenuItemsEventHandler::MenuItemEnteredAndSet() {
	// Play the verification sound...
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemVerifyAndSelectEvent);
}

void MainMenuDisplayState::AllMenuItemsEventHandler::MenuItemCancelled() {
	// Play the cancel/back sound..
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuItemBackAndCancelEvent);
}

/**
 * Triggered event when a sound onomatopiea particle spawns for the background 'bang' particle effects.
 */
void MainMenuDisplayState::BangParticleEventHandler::ParticleSpawnedEvent(const ESPParticle* particle) {
	const ESPOnomataParticle* soundParticle = static_cast<const ESPOnomataParticle*>(particle);
	assert(soundParticle != NULL);

	// Based on the extremeness of the onomata particle just spawned, we signify a sound event
	GameSoundAssets* soundAssets = this->mainMenuState->display->GetAssets()->GetSoundAssets();
	switch (soundParticle->GetSoundExtremeness()) {

		// Small bang event...
		case Onomatoplex::WEAK:
		case Onomatoplex::NORMAL:
			soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuBackgroundBangSmallEvent);
			//debug_output("Small bang");
			break;

		// Medium bang event...
		case Onomatoplex::PRETTY_GOOD:
		case Onomatoplex::GOOD:
		case Onomatoplex::AWESOME:
			soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuBackgroundBangMediumEvent);
			//debug_output("Medium bang");
			break;

		// Big bang event...
		case Onomatoplex::SUPER_AWESOME:
		case Onomatoplex::UBER:
			soundAssets->PlayMainMenuSound(GameSoundAssets::MainMenuBackgroundBangBigEvent);
			//debug_output("Big bang");
			break;

		default:
			assert(false);
			break;
	}
}