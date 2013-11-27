/**
 * MainMenuDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "MainMenuDisplayState.h"
#include "SelectLevelMenuState.h"
#include "InGameDisplayState.h"
#include "CreditsDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameMenuItem.h"
#include "GameAssets.h"
#include "CgFxBloom.h"
#include "MenuBackgroundRenderer.h"

#include "../BlammoEngine/FBObj.h"
#include "../GameModel/GameModel.h"
#include "../GameModel/GameProgressIO.h"
#include "../GameControl/GameControllerManager.h"
#include "../GameSound/GameSound.h"
#include "../ESPEngine/ESPOnomataParticle.h"
#include "../WindowManager.h"

const int MainMenuDisplayState::MENU_SEL_ON_INDEX	= 0;
const int MainMenuDisplayState::MENU_SEL_OFF_INDEX	= 1;

const size_t MainMenuDisplayState::TOTAL_NUM_BANG_EFFECTS = 10;

// Menu items
const char* MainMenuDisplayState::NEW_GAME_MENUITEM		= "New Game";
const char* MainMenuDisplayState::CONTINUE_MENUITEM     = "Continue";
const char* MainMenuDisplayState::PLAY_LEVEL_MENUITEM	= "Level Select";
const char* MainMenuDisplayState::OPTIONS_MENUITEM		= "Options";
const char* MainMenuDisplayState::BLAMMOPEDIA_MENUITEM  = "Blammopedia";
const char* MainMenuDisplayState::CREDITS_MENUITEM      = "Credits";
const char* MainMenuDisplayState::EXIT_MENUITEM			= "Exit Game";

const Colour MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR		= Colour(1, 0.65f, 0);
const Colour MainMenuDisplayState::MENU_ITEM_SEL_COLOUR			= Colour(1, 1, 0);
const Colour MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR	    = Colour(0.49f, 0.98f, 1.0f);
const Colour MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR	    = Colour(0.5f, 0.5f, 0.5f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_IDLE_COLOUR		= Colour(0.75f, 0.75f, 0.75f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_ACTIVE_COLOUR	= Colour(1, 1, 1);

const float MainMenuDisplayState::CAM_DIST_FROM_ORIGIN = 20.0f;

const double MainMenuDisplayState::FADE_IN_TIME_IN_SECS = 3.0;
const double MainMenuDisplayState::FADE_OUT_TIME_IN_SECS = 1.25;

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display, const DisplayStateInfo& info) : 
DisplayState(display), mainMenu(NULL), startGameMenuItem(NULL), optionsSubMenu(NULL), selectListItemsHandler(NULL),
mainMenuEventHandler(NULL), optionsMenuEventHandler(NULL), quitVerifyHandler(NULL), particleEventHandler(NULL),
eraseProgVerifyHandler(NULL), volItemHandler(NULL), postMenuFBObj(NULL),
changeToPlayGameState(false), changeToBlammopediaState(false), changeToLevelSelectState(false), changeToCreditsState(false),
menuFBO(NULL), bloomEffect(NULL), eraseSuccessfulPopup(NULL), eraseFailedPopup(NULL),
particleSmallGrowth(1.0f, 1.3f), particleMediumGrowth(1.0f, 1.6f), bgLoopedSoundID(INVALID_SOUND_ID),
titleDisplay(display->GetTextScalingFactor(), 0.25, display->GetSound()),
blammopediaItemIndex(-1), creditsItemIndex(-1), doAnimatedFadeIn(info.GetDoAnimatedFadeIn()),
madeByTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
                GameViewConstants::GetInstance()->GAME_CREDITS_TEXT),
licenseLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),
             GameViewConstants::GetInstance()->LICENSE_TEXT) {

    this->madeByTextLabel.SetColour(Colour(1,1,1));
    this->madeByTextLabel.SetScale(this->display->GetTextScalingFactor());
    this->licenseLabel.SetColour(Colour(0.66f, 0.66f, 0.66f));
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
	this->selectListItemsHandler  = new SelectListItemsEventHandler(this);
    this->volItemHandler          = new VolumeItemEventHandler(this);
	this->particleEventHandler    = new BangParticleEventHandler(this);
    this->quitVerifyHandler       = new QuitVerifyEventHandler(this);
    this->eraseProgVerifyHandler  = new EraseProgressVerifyEventHandler(this);
	this->InitializeMainMenu();

	// Setup the fade-in animation
	this->fadeAnimation.SetLerp(0.0, MainMenuDisplayState::FADE_IN_TIME_IN_SECS, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

	// Setup any full-screen effects
    this->menuFBO       = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
    this->postMenuFBObj = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();

	// Allocate memory...
	for (size_t i = 0; i < TOTAL_NUM_BANG_EFFECTS; i++) {
		this->deadBangEffects.push_back(new ESPPointEmitter());
		this->deadOnoEffects.push_back(new ESPPointEmitter());
	}

    static const int POPUP_WIDTH = 750;
    std::vector<std::string> selectableOptions;
    selectableOptions.push_back("Ok");
    this->eraseSuccessfulPopup = new PopupTutorialHint(POPUP_WIDTH);
    DecoratorOverlayPane* eraseSuccessPopupPane = this->eraseSuccessfulPopup->GetPane();
    eraseSuccessPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    eraseSuccessPopupPane->AddText("Progress erase successful.", Colour(1,1,1), 1.0f);
    eraseSuccessPopupPane->SetSelectableOptions(selectableOptions, 0);
 
    this->eraseFailedPopup = new PopupTutorialHint(POPUP_WIDTH);
    DecoratorOverlayPane* eraseFailedPopupPane = this->eraseFailedPopup->GetPane();
    eraseFailedPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    eraseFailedPopupPane->AddText("Progress erase failed.", Colour(1,1,1), 1.0f);
    eraseFailedPopupPane->SetSelectableOptions(selectableOptions, 0);
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
	delete this->selectListItemsHandler;
	this->selectListItemsHandler = NULL;
    delete this->volItemHandler;
    this->volItemHandler = NULL;
	delete this->particleEventHandler;
	this->particleEventHandler = NULL;
    delete this->quitVerifyHandler;
    this->quitVerifyHandler = NULL;
    delete this->eraseProgVerifyHandler;
    this->eraseProgVerifyHandler = NULL;

	delete this->menuFBO;
	this->menuFBO = NULL;
    delete this->postMenuFBObj;
    this->postMenuFBObj = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;

	// Release texture assets that we no longer need
    bool success = false;
	for (size_t i = 0; i < this->bangTextures.size(); i++) {
		success = ResourceManager::GetInstance()->ReleaseTextureResource(this->bangTextures[i]);
		assert(success);
	}

    UNUSED_VARIABLE(success);

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

    delete this->eraseSuccessfulPopup;
    this->eraseSuccessfulPopup = NULL;
    delete this->eraseFailedPopup;
    this->eraseFailedPopup = NULL;
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
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, Camera::GetWindowHeight() - MENU_Y_INDENT);

	// Setup the main menu (top-most menu)
	this->mainMenu = new GameMenu(menuTopLeftCorner);
	this->mainMenu->AddEventHandler(this->mainMenuEventHandler);
	this->mainMenu->SetColourScheme(MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
	                                MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Setup sub-menus
	this->InitializeOptionsSubMenu();

	// Scale the labels based on the height/width of the window
	float textScaleFactor = this->display->GetTextScalingFactor();

	const float dropShadowAmtSm = textScaleFactor * 0.15f;
	const float dropShadowAmtLg = textScaleFactor * 0.15f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	// Add items to the menu in their order (first to last)

	TextLabel2D tempLabelSm;
    TextLabel2D tempLabelLg;

    // "New Game" if no game has already been started, otherwise we use "Continue"...
    int furthestWorldIdx, furthestLevelIdx;
    this->display->GetModel()->GetFurthestProgressWorldAndLevel(furthestWorldIdx, furthestLevelIdx);

    bool isNewGame = this->display->GetModel()->IsTutorialLevel(furthestWorldIdx, furthestLevelIdx);
    if (isNewGame) {
        tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), NEW_GAME_MENUITEM);
        tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), NEW_GAME_MENUITEM);
    }
    else {
        tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), CONTINUE_MENUITEM);
        tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), CONTINUE_MENUITEM);
    }

	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelSm.SetScale(textScaleFactor);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);
	tempLabelLg.SetScale(textScaleFactor);

    this->startGameMenuItem = new GameMenuItem(tempLabelSm, tempLabelLg, NULL);
    this->startGameMenuItemIndex = this->mainMenu->AddMenuItem(this->startGameMenuItem);

    if (!isNewGame) {
        tempLabelSm.SetText(PLAY_LEVEL_MENUITEM);
        tempLabelLg.SetText(PLAY_LEVEL_MENUITEM);
        this->playLevelMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);
    }
    else {
        this->playLevelMenuItemIndex = -1;
    }

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

    // Add the credits item to the main menu
    tempLabelSm.SetText(CREDITS_MENUITEM);
    tempLabelLg.SetText(CREDITS_MENUITEM);
    this->creditsItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	tempLabelSm.SetText(EXIT_MENUITEM);
	tempLabelLg.SetText(EXIT_MENUITEM);

	VerifyMenuItem* exitMenuItem = new VerifyMenuItem(tempLabelSm, tempLabelLg, 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));

	exitMenuItem->SetVerifyMenuColours(Colour(1,1,1), MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR, Colour(1,1,1));
	exitMenuItem->SetVerifyMenuText("Are you sure you want to quit?", "Well, obviously.", "Hell no.");
	exitMenuItem->SetEventHandler(this->quitVerifyHandler);
	this->exitGameMenuItemIndex = this->mainMenu->AddMenuItem(exitMenuItem);

	this->mainMenu->SetSelectedMenuItem(this->startGameMenuItemIndex, false);
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

	// Add the toggle full screen item
	std::vector<std::string> fullscreenOptions;
	fullscreenOptions.reserve(2);
	fullscreenOptions.push_back("On");
	fullscreenOptions.push_back("Off");

	this->fullscreenMenuItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, fullscreenOptions);
	this->fullscreenMenuItem->SetSelectedItem(this->cfgOptions.GetIsFullscreenOn() ? MENU_SEL_ON_INDEX : MENU_SEL_OFF_INDEX);
	this->fullscreenMenuItem->SetEventHandler(this->selectListItemsHandler);
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
	this->vSyncMenuItem->SetEventHandler(this->selectListItemsHandler);
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
	this->resolutionMenuItem->SetEventHandler(this->selectListItemsHandler);
	this->optionsResolutionIndex = this->optionsSubMenu->AddMenuItem(this->resolutionMenuItem);

	// Add a sound volume option
	subMenuLabelSm.SetText("Volume");
	subMenuLabelLg.SetText("Volume");

	int currentVolume = cfgOptions.GetVolume(); // Determine the volume of the sound...
	this->soundVolumeMenuItem = new AmountScrollerMenuItem(subMenuLabelSm, subMenuLabelLg,
		ConfigOptions::MIN_VOLUME, ConfigOptions::MAX_VOLUME, currentVolume, 5.0f);
	this->soundVolumeMenuItem->SetConstantChangeFeedback(true);
	this->soundVolumeMenuItem->SetEventHandler(this->volItemHandler);
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
    this->invertBallBoostItem->SetEventHandler(this->selectListItemsHandler);
    this->optionsInvertBallBoostIndex = this->optionsSubMenu->AddMenuItem(this->invertBallBoostItem);


    // Ball boosting mode option
	subMenuLabelSm.SetText("Ball Boost Mode");
	subMenuLabelLg.SetText("Ball Boost Mode");
    std::vector<std::string> boostModeOptions = ConfigOptions::GetBallBoostModeItems();
    
    this->ballBoostModeItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, boostModeOptions);
    this->ballBoostModeItem->SetSelectedItem(static_cast<int>(this->cfgOptions.GetBallBoostMode()));
    this->ballBoostModeItem->SetEventHandler(this->selectListItemsHandler);
    this->optionsBallBoostModeIndex = this->optionsSubMenu->AddMenuItem(this->ballBoostModeItem);


    // Difficulty option
	subMenuLabelSm.SetText("Difficulty");
	subMenuLabelLg.SetText("Difficulty");
    std::vector<std::string> difficultyOptions = ConfigOptions::GetDifficultyItems();

    this->difficultyItem = new SelectionListMenuItem(subMenuLabelSm, subMenuLabelLg, difficultyOptions);
    this->difficultyItem->SetSelectedItem(static_cast<int>(this->cfgOptions.GetDifficulty()));
	this->difficultyItem->SetEventHandler(this->selectListItemsHandler);
	this->optionsDifficultyIndex = this->optionsSubMenu->AddMenuItem(this->difficultyItem);


    // Erase game progress data option
	subMenuLabelSm.SetText("Erase Progress Data...");
	subMenuLabelLg.SetText("Erase Progress Data...");

    VerifyMenuItem* eraseProgressItem = new VerifyMenuItem(subMenuLabelSm, subMenuLabelLg,
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small), 
		GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium));
	eraseProgressItem->SetVerifyMenuColours(Colour(1,1,1), Colour(0.5f, 0.5f, 0.5f), Colour(1,1,1));
	eraseProgressItem->SetVerifyMenuText("If you do this all of your progress will be lost. Are you sure you want to proceed? ", "Yes", "No");
    eraseProgressItem->SetEventHandler(this->eraseProgVerifyHandler);
    this->optionsEraseProgressIndex = this->optionsSubMenu->AddMenuItem(eraseProgressItem);


	// Add an option for getting out of the menu
	subMenuLabelSm.SetText("Back");
	subMenuLabelLg.SetText("Back");
	this->optionsBackIndex = this->optionsSubMenu->AddMenuItem(subMenuLabelSm, subMenuLabelLg, NULL);

	// Set the selected index to be the first item in the menu
	this->optionsSubMenu->SetSelectedMenuItem(this->optionsFullscreenIndex, false);
}

void MainMenuDisplayState::SetupBloomEffect() {
	if (this->bloomEffect != NULL) {
		delete this->bloomEffect;
	}

	// Create the new bloom effect and set its parameters appropriately
	this->bloomEffect = new CgFxBloom(this->menuFBO);
    this->bloomEffect->SetMenuBloomSettings();
}

/**
 * Render the menu and any other stuff associated with it.
 */
void MainMenuDisplayState::RenderFrame(double dT) {
	bool finishFadeAnim = false;
    
    if (this->titleDisplay.IsFinishedAnimating()) {
        // Play the main menu background music...
        if (this->bgLoopedSoundID == INVALID_SOUND_ID) {
            this->bgLoopedSoundID = this->display->GetSound()->PlaySound(GameSound::MainMenuBackgroundLoop, true);
        }
        finishFadeAnim = this->fadeAnimation.Tick(dT);
    }

    Camera& camera = this->display->GetCamera();
    Vector3D shakeAmt = camera.TickAndGetCameraShakeTransform(dT);

    GameSound* sound = this->display->GetSound();
    GameModel* model = this->display->GetModel();
    MenuBackgroundRenderer* bgRenderer = this->display->GetMenuBGRenderer();
    
	// Check to see if we're switching game states...
    if (finishFadeAnim) {
        static const double SOUND_FADE_OUT_TIME = 0.5;
	    if (this->changeToPlayGameState) {
    		
		    // Turn off the background music...
            sound->StopSound(this->bgLoopedSoundID);
		    
            // Figure out the furthest world and level that the player has progressed to
            int furthestWorldIdx = -1;
            int furthestLevelIdx = -1;
            model->GetFurthestProgressWorldAndLevel(furthestWorldIdx, furthestLevelIdx);
            
            // Probably because I'm still developing the game and I added a world...
            // This should never happen in production!!
            if (furthestWorldIdx == -1) {
                model->OverrideGetFurthestProgress(furthestWorldIdx, furthestLevelIdx);
            }

            // Figure out whether the level is locked and requires a certain number of stars to unlock...
            GameWorld* furthestWorld = model->GetWorldByIndex(furthestWorldIdx);
            assert(furthestWorld != NULL);
            GameLevel* furthestLevel = furthestWorld->GetLevelByIndex(furthestLevelIdx);
            assert(furthestLevel != NULL);

            if (furthestLevel->GetAreUnlockStarsPaidFor()) {
                // Start the game at the furthest level of player progress - this will queue up the next states that we need to go to
                model->StartGameAtWorldAndLevel(furthestWorldIdx, furthestLevelIdx);
                // Place the view into the proper state to play the game
                this->display->SetCurrentStateAsNextQueuedState();
            }
            else {
                // Check to see if the player has enough stars to break the lock...
                int totalStars = model->GetTotalStarsCollectedInGame();
                if (totalStars >= furthestLevel->GetNumStarsRequiredToUnlock()) {
                   
                    model->StartGameAtWorldAndLevel(furthestWorldIdx, furthestLevelIdx);

                    // Pay for the stars automatically, start up the new level and save progress
                    furthestLevel = furthestWorld->GetLevelByIndex(furthestLevelIdx);
                    furthestLevel->SetAreUnlockStarsPaidFor(true);
                    GameProgressIO::SaveGameProgress(model);

                    
                    this->display->SetCurrentStateAsNextQueuedState();
                }
                else {
                    // The next state will need to be the level selection screen since the player can't unlock the level
                    this->display->SetCurrentState(new SelectLevelMenuState(this->display, 
                        DisplayStateInfo::BuildSelectLevelInfo(furthestWorldIdx)));
                }
            }
		    return;
	    }
        else if (this->changeToBlammopediaState) {
		    
            // Turn off the background music...
            sound->StopSound(this->bgLoopedSoundID, SOUND_FADE_OUT_TIME);
            this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::BlammopediaMenu, DisplayStateInfo(), this->display));
            return;
        }
        else if (this->changeToCreditsState) {
            // Turn off the background music...
            sound->StopSound(this->bgLoopedSoundID, SOUND_FADE_OUT_TIME);
            this->display->SetCurrentState(new CreditsDisplayState(this->display));
            return;
        }
        else if (this->changeToLevelSelectState) {

            // Turn off the background music...
            sound->StopSound(this->bgLoopedSoundID, SOUND_FADE_OUT_TIME);
            this->display->SetCurrentState(DisplayState::BuildDisplayStateFromType(DisplayState::SelectWorldMenu, DisplayStateInfo(), this->display));
            return;
        }
    }

	const int DISPLAY_WIDTH	 = Camera::GetWindowWidth();
	const int DISPLAY_HEIGHT = Camera::GetWindowHeight();

	this->menuFBO->BindFBObj();

	// Render background stuffs (behind the menu)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the background...
    bgRenderer->DrawShakeBG(camera, shakeAmt[0], shakeAmt[1]);

    Camera menuCamera;
	menuCamera.SetPerspective();
	menuCamera.Move(Vector3D(0, 0, CAM_DIST_FROM_ORIGIN));

	this->RenderBackgroundEffects(dT, menuCamera);
	
    // Figure out where the menu needs to be to ensure it doesn't clip with the bottom of the screen...
    this->mainMenu->SetTopLeftCorner(MENU_X_INDENT, DISPLAY_HEIGHT - MENU_Y_INDENT * this->display->GetTextScalingFactor());
	// Render the menu
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

    // Render any extra pop-up stuff
    this->eraseSuccessfulPopup->Tick(dT);
    this->eraseFailedPopup->Tick(dT);
    this->eraseSuccessfulPopup->Draw(menuCamera);
    this->eraseFailedPopup->Draw(menuCamera);

	// Fade-in/out overlay
    if (this->fadeAnimation.GetTargetValue() == 0.0f) {
        if (this->doAnimatedFadeIn) {
            bgRenderer->DrawShakeBG(camera, shakeAmt[0], shakeAmt[1], this->fadeAnimation.GetInterpolantValue());
        }
        else {
            bgRenderer->DrawNonAnimatedFadeShakeBG(shakeAmt[0], shakeAmt[1], this->fadeAnimation.GetInterpolantValue());
        }
        this->titleDisplay.Draw(dT, camera, this->postMenuFBObj->GetFBOTexture());
    }
    else {
        this->titleDisplay.Draw(dT, camera, this->postMenuFBObj->GetFBOTexture());

        if (this->changeToPlayGameState) {
            bgRenderer->DrawNonAnimatedFadeShakeBG(shakeAmt[0], shakeAmt[1], this->fadeAnimation.GetInterpolantValue());
        }
        else {
            bgRenderer->DrawShakeBG(camera, shakeAmt[0], shakeAmt[1], this->fadeAnimation.GetInterpolantValue());
        }
    }

	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu screen and draw it
	this->bloomEffect->Draw(DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
	
    this->postMenuFBObj->BindFBObj();
    this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();
    this->postMenuFBObj->UnbindFBObj();
    this->postMenuFBObj->GetFBOTexture()->RenderTextureToFullscreenQuad();

	debug_opengl_state();
}

/**
 * Render the background effects for the menu screen. These include
 * random bangs and booms, etc. going off all over the place.
 */
void MainMenuDisplayState::RenderBackgroundEffects(double dT, Camera& menuCam) {
	const float MAX_Z_COORD = CAM_DIST_FROM_ORIGIN / 4.0f;
	const float MIN_Z_COORD = -MAX_Z_COORD;
	const float MAX_Y_COORD = 0.95f * CAM_DIST_FROM_ORIGIN * tan(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS) / 2.0f);
	const float MIN_Y_COORD = -MAX_Y_COORD;
	const float MAX_X_COORD = static_cast<float>(Camera::GetWindowWidth()) /  static_cast<float>(Camera::GetWindowHeight()) * MAX_Y_COORD;
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
void MainMenuDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                         const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);
	assert(this->mainMenu != NULL);

    if (!this->titleDisplay.IsFinishedAnimating()) {
        return;
    }

	// Tell the main menu about the key pressed event
    if (this->eraseFailedPopup->GetIsVisible() || this->eraseSuccessfulPopup->GetIsVisible()) {
        this->eraseSuccessfulPopup->ButtonPressed(pressedButton);
        this->eraseFailedPopup->ButtonPressed(pressedButton);
    }
    else {
	    this->mainMenu->ButtonPressed(pressedButton);
    }
}

void MainMenuDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
	assert(this->mainMenu != NULL);

    if (!this->titleDisplay.IsFinishedAnimating()) {
        return;
    }

	// Tell the main menu about the key pressed event
    if (this->eraseFailedPopup->GetIsVisible() || this->eraseSuccessfulPopup->GetIsVisible()) {
    }
    else {
        this->mainMenu->ButtonReleased(releasedButton);
    }
}

void MainMenuDisplayState::DisplaySizeChanged(int width, int height) {
	UNUSED_PARAMETER(width);
	UNUSED_PARAMETER(height);

	delete this->menuFBO;
	this->menuFBO = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
    delete this->postMenuFBObj;
    this->postMenuFBObj = new FBObj(Camera::GetWindowWidth(), Camera::GetWindowHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
}

void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);
    
    // Play the sound effect associated with menu item changing/being highlighted by the user
    GameSound* sound = this->mainMenuState->display->GetSound();
    sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false);
}

/**
 * Handle activation events for items in the main menu.
 */
void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	// Play the sound effect associated with menu item selection/activation (i.e., when the user actually presses a button
    // on a selected item to choose it)
	GameSound* sound = this->mainMenuState->display->GetSound();
	
	// Do the actual selection of the item
	if (itemIndex == this->mainMenuState->startGameMenuItemIndex) {
        
		debug_output("Selected " << this->mainMenuState->mainMenu->GetMenuItemAt(itemIndex)->GetCurrLabel()->GetText() << " from menu");
		
        sound->PlaySound(GameSound::MenuItemVerifyAndSelectStartGameEvent, false);
		
        this->mainMenuState->changeToPlayGameState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, MainMenuDisplayState::FADE_OUT_TIME_IN_SECS, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
	}
	else if (itemIndex == this->mainMenuState->playLevelMenuItemIndex) {

		debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
		
        sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false);
        
        this->mainMenuState->changeToLevelSelectState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, MainMenuDisplayState::FADE_OUT_TIME_IN_SECS, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
	}
	else if (itemIndex == this->mainMenuState->optionsMenuItemIndex) {
		sound->PlaySound(GameSound::MenuOpenSubMenuWindowEvent, false);
	}
    else if (itemIndex == this->mainMenuState->blammopediaItemIndex) {

        sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false);

        this->mainMenuState->changeToBlammopediaState = true;
		this->mainMenuState->fadeAnimation.SetLerp(0.0, MainMenuDisplayState::FADE_OUT_TIME_IN_SECS, 0.0f, 1.0f);
		this->mainMenuState->fadeAnimation.SetRepeat(false);
    }
    else if (itemIndex == this->mainMenuState->creditsItemIndex) {
        sound->PlaySound(GameSound::MenuItemVerifyAndSelectEvent, false);

        this->mainMenuState->changeToCreditsState = true;
        this->mainMenuState->fadeAnimation.SetLerp(0.0, MainMenuDisplayState::FADE_OUT_TIME_IN_SECS, 0.0f, 1.0f);
        this->mainMenuState->fadeAnimation.SetRepeat(false);
    }
	else if (itemIndex == this->mainMenuState->exitGameMenuItemIndex) {
		// We don't do anything since the user is currently being asked
		// for verification to quit the game
		debug_output("Selected " << EXIT_MENUITEM << " from menu");
	}
	else {
		assert(false);
	}
}

void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);
};

void MainMenuDisplayState::MainMenuEventHandler::EscMenu() {
	GameMenu* mainMenu = this->mainMenuState->mainMenu;

	// Select the exit game option from the main menu if not selected, if selected then activate it
	if (mainMenu->GetSelectedMenuItem() != this->mainMenuState->exitGameMenuItemIndex) {
		mainMenu->SetSelectedMenuItem(this->mainMenuState->exitGameMenuItemIndex, true);
	}
	else {
		this->mainMenuState->mainMenu->ActivateSelectedMenuItem();
	}
}

void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemHighlightedEvent(int itemIndex) {
	UNUSED_PARAMETER(itemIndex);

    // Play the sound effect assoicated with menu item changing/being highlighted by the user
    GameSound* sound = this->mainMenuState->display->GetSound();
    sound->PlaySound(GameSound::MenuItemChangedSelectionEvent, false);
}

/**
 * Handle activation events for items in the options menu.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->mainMenuState->optionsBackIndex) {
        GameSound* sound = this->mainMenuState->display->GetSound();
        sound->PlaySound(GameSound::MenuItemCancelEvent, false);

		// Go back to the main menu (deactivate any previously activated menu items)
		this->mainMenuState->optionsSubMenu->DeactivateSelectedMenuItem();
		this->mainMenuState->optionsSubMenu->SetSelectedMenuItem(0, false);
		this->mainMenuState->mainMenu->DeactivateSelectedMenuItem();
	}
	else {
		if (itemIndex == this->mainMenuState->optionsFullscreenIndex || 
            itemIndex == this->mainMenuState->optionsResolutionIndex ||
            itemIndex == this->mainMenuState->optionsVSyncIndex) {
		}
	}
}

/**
 * Handle changes in the options for the game.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {

	//GameSound* sound = this->mainMenuState->display->GetSound();

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
    else if (itemIndex == this->mainMenuState->optionsBallBoostModeIndex) {
        int currSelectionIdx = this->mainMenuState->ballBoostModeItem->GetSelectedItemIndex();
        BallBoostModel::BallBoostMode modeToSet = static_cast<BallBoostModel::BallBoostMode>(currSelectionIdx);

        this->mainMenuState->cfgOptions.SetBallBoostMode(modeToSet);
        this->mainMenuState->display->GetModel()->SetBallBoostMode(modeToSet);
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
	else {
		return;
	}

	// A configuration option has changed - rewrite the configuration file to accomodate the change
	ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->mainMenuState->cfgOptions);
}

void MainMenuDisplayState::OptionsSubMenuEventHandler::EscMenu() {
	GameSubMenu* optionsSubMenu = this->mainMenuState->optionsSubMenu;
	
	// Cause the menu to go back, if back isn't selected the select it...
	if (optionsSubMenu->GetSelectedMenuItem() != this->mainMenuState->optionsBackIndex) {
		optionsSubMenu->SetSelectedMenuItem(this->mainMenuState->optionsBackIndex, true);
	}
	else {
		this->mainMenuState->optionsSubMenu->ActivateSelectedMenuItem();
	}
}


MainMenuDisplayState::SelectListItemsEventHandler::SelectListItemsEventHandler(MainMenuDisplayState* mainMenuState) : 
SelectionListEventHandlerWithSound(mainMenuState->display->GetSound()), mainMenuState(mainMenuState) {
}

MainMenuDisplayState::VolumeItemEventHandler::VolumeItemEventHandler(MainMenuDisplayState* mainMenuState) :
ScrollerItemEventHandlerWithSound(mainMenuState->display->GetSound()), mainMenuState(mainMenuState) {
}

void MainMenuDisplayState::VolumeItemEventHandler::MenuItemScrolled()  { 
    ScrollerItemEventHandlerWithSound::MenuItemScrolled();
    
    // Set the global volume level for all sounds...
    int volumeLevel = static_cast<int>(this->mainMenuState->soundVolumeMenuItem->GetScrollerValue());
    this->UpdateGameMasterVolume(volumeLevel);
}
void MainMenuDisplayState::VolumeItemEventHandler::MenuItemConfirmed() {
    ScrollerItemEventHandlerWithSound::MenuItemConfirmed();

	int volumeLevel = static_cast<int>(this->mainMenuState->soundVolumeMenuItem->GetScrollerValue());
	
    // Set the global volume level for all sounds...
    this->UpdateGameMasterVolume(volumeLevel);

    // Set the configuration option
	this->mainMenuState->cfgOptions.SetVolume(volumeLevel);
	// A configuration option has changed - rewrite the configuration file to accommodate the change
	ResourceManager::GetInstance()->WriteConfigurationOptionsToFile(this->mainMenuState->cfgOptions);
}

void MainMenuDisplayState::VolumeItemEventHandler::MenuItemCancelled() {
    ScrollerItemEventHandlerWithSound::MenuItemCancelled();

    // Set the global volume level for all sounds...
    int volumeLevel = static_cast<int>(this->mainMenuState->soundVolumeMenuItem->GetScrollerValue());
    this->UpdateGameMasterVolume(volumeLevel);
    
    // Since we canceled, there should be no change to the configuration
	assert(volumeLevel == this->mainMenuState->cfgOptions.GetVolume());
}

void MainMenuDisplayState::VolumeItemEventHandler::UpdateGameMasterVolume(int volumeLevel) {
	// Set the global volume level for all sounds...
    GameSound* sound = this->mainMenuState->display->GetSound();
    sound->SetMasterVolume(static_cast<float>(volumeLevel) / 100.0f);
}

MainMenuDisplayState::QuitVerifyEventHandler::QuitVerifyEventHandler(MainMenuDisplayState *mainMenuState) : 
VerifyMenuEventHandlerWithSound(mainMenuState->display->GetSound()),
mainMenuState(mainMenuState) {
}

void MainMenuDisplayState::QuitVerifyEventHandler::MenuItemConfirmed() {
    VerifyMenuEventHandlerWithSound::MenuItemConfirmed();
    SDL_Delay(GameSound::MENU_CONFIRM_SOUND_DELAY_IN_MS);
    this->mainMenuState->display->QuitGame();
}

MainMenuDisplayState::EraseProgressVerifyEventHandler::EraseProgressVerifyEventHandler(MainMenuDisplayState *mainMenuState) :
VerifyMenuEventHandlerWithSound(mainMenuState->display->GetSound()),
mainMenuState(mainMenuState) {
}

void MainMenuDisplayState::EraseProgressVerifyEventHandler::MenuItemConfirmed() {
    // The player has decided to decimate all of their progress...
    bool success = GameProgressIO::WipeoutGameProgress(this->mainMenuState->display->GetModel());
    
    if (success) {
        this->mainMenuState->eraseSuccessfulPopup->Show(0.0, 0.5);
        // Set the start game menu item to be "New Game"
        this->mainMenuState->startGameMenuItem->SetText(MainMenuDisplayState::NEW_GAME_MENUITEM);
    }
    else {
        this->mainMenuState->eraseFailedPopup->Show(0.0, 0.5);
    }
}

/**
 * Triggered event when a sound onomatopiea particle spawns for the background 'bang' particle effects.
 */
void MainMenuDisplayState::BangParticleEventHandler::ParticleSpawnedEvent(const ESPParticle* particle) {
	const ESPOnomataParticle* soundParticle = static_cast<const ESPOnomataParticle*>(particle);
	assert(soundParticle != NULL);

	// Based on the extremeness of the onomata particle just spawned, we signify a sound event
	//GameSound* sound = this->mainMenuState->display->GetSound();
	switch (soundParticle->GetSoundExtremeness()) {

		// Small bang event...
		case Onomatoplex::WEAK:
		case Onomatoplex::NORMAL:
			//sound->PlaySound(GameSound::MainMenuBackgroundBangSmallEvent, 1);
			break;

		// Medium bang event...
		case Onomatoplex::PRETTY_GOOD:
		case Onomatoplex::GOOD:
		case Onomatoplex::AWESOME:
			//sound->PlaySound(GameSound::MainMenuBackgroundBangMediumEvent, 1);
			break;

		// Big bang event...
		case Onomatoplex::SUPER_AWESOME:
		case Onomatoplex::UBER:
			//sound->PlaySound(GameSound::MainMenuBackgroundBangBigEvent, 1);
			break;

		default:
			assert(false);
			break;
	}
}