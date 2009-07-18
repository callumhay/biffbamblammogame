/**
 * MainMenuDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "MainMenuDisplayState.h"
#include "StartGameDisplayState.h"
#include "GameFontAssetsManager.h"
#include "GameDisplay.h"
#include "GameMenu.h"
#include "GameMenuItem.h"
#include "GameAssets.h"
#include "CgFxBloom.h"

#include "../BlammoEngine/FBObj.h"

#include "../GameController.h"
#include "../WindowManager.h"


// 3 sprites for "Biff" "Bam" and "Blammo" respectively
const std::string MainMenuDisplayState::TITLE_BIFF_TEXT			= "Biff!";
const std::string MainMenuDisplayState::TITLE_BAM_TEXT			= "Bam!!";
const std::string MainMenuDisplayState::TITLE_BLAMMO_TEXT		= "Blammo!?!";

// Menu items
const std::string MainMenuDisplayState::NEW_GAME_MENUITEM		= "New Game";
const std::string MainMenuDisplayState::PLAY_LEVEL_MENUITEM	= "Play Level";
const std::string MainMenuDisplayState::OPTIONS_MENUITEM		= "Options";
const std::string MainMenuDisplayState::EXIT_MENUITEM				= "Exit Game";

const Colour MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR		= Colour(1, 0.65f, 0);
const Colour MainMenuDisplayState::MENU_ITEM_SEL_COLOUR			= Colour(1, 1, 0);
const Colour MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR	= Colour(0.49f, 0.98f, 1.0f);
const Colour MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR	= Colour(0.5f, 0.5f, 0.5f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_IDLE_COLOUR		= Colour(0.75f, 0.75f, 0.75f);
const Colour MainMenuDisplayState::SUBMENU_ITEM_ACTIVE_COLOUR	= Colour(1, 1, 1);

MainMenuDisplayState::MainMenuDisplayState(GameDisplay* display) : 
DisplayState(display), mainMenu(NULL), optionsSubMenu(NULL), 
mainMenuEventHandler(NULL), optionsMenuEventHandler(NULL), changeToPlayGameState(false),
menuFBO(NULL), bloomEffect(NULL),
particleSmallGrowth(1.0f, 1.3f), particleMediumGrowth(1.0f, 1.6f)
{

	// Setup any textures for rendering the menu screen
	this->bangTextures.reserve(3);
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture2D::Trilinear, GL_TEXTURE_2D));

	// Setup any emitter/sprite/particle effects
	this->InitializeESPEffects();

	// Read the configuration file to figure out how to initialize each of the options
	this->cfgOptions = ResourceManager::GetInstance()->ReadConfigurationOptions(true);

	// Setup handlers for menus and initialize the menus
	this->mainMenuEventHandler		= new MainMenuEventHandler(this);
	this->optionsMenuEventHandler = new OptionsSubMenuEventHandler(this);	
	this->InitializeMainMenu();

	// Setup the fade-in animation
	this->fadeAnimation.SetLerp(0.0, 3.0, 1.0f, 0.0f);
	this->fadeAnimation.SetRepeat(false);
	this->fadeAnimation.SetInterpolantValue(1.0f);

	// Setup any fullscreen effects
	this->menuFBO = new FBObj(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
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

	delete this->menuFBO;
	this->menuFBO = NULL;
	delete this->bloomEffect;
	this->bloomEffect = NULL;

	// Release texture assets that we no longer need
	for (size_t i = 0; i < this->bangTextures.size(); i++) {
		bool releaseTexSuccess = ResourceManager::GetInstance()->ReleaseTextureResource(this->bangTextures[i]);
		assert(releaseTexSuccess);
	}

	// Clean up any left over emitters
	for (std::list<ESPPointEmitter*>::iterator iter = this->randomBGParicles.begin(); iter != this->randomBGParicles.end(); iter++) {
		delete *iter;
	}
	this->randomBGParicles.clear();
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

	// "Biff!" Bang Title Text
	this->biffEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->biffEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->biffEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->biffEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->biffEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->biffEmitter.SetParticleRotation(ESPInterval(-10));
	this->biffEmitter.SetParticleSize(ESPInterval(10), ESPInterval(5));
	this->biffEmitter.SetParticles(1, dynamic_cast<Texture2D*>(this->bangTextures[0]));

	this->biffTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->biffTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->biffTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->biffTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->biffTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->biffTextEmitter.SetParticleRotation(ESPInterval(-10));
	this->biffTextEmitter.SetParticleSize(ESPInterval(1), ESPInterval(1));
	
	TextLabel2D biffTitleText(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), MainMenuDisplayState::TITLE_BIFF_TEXT);
	biffTitleText.SetDropShadow(Colour(0,0,0), 0.1f);
	this->biffTextEmitter.SetParticles(1, biffTitleText);

	// "Bam!!" Bang title text
	this->bamEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->bamEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->bamEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->bamEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->bamEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->bamEmitter.SetParticleRotation(ESPInterval(-15));
	this->bamEmitter.SetParticleSize(ESPInterval(11.0f), ESPInterval(5.0f));
	this->bamEmitter.SetParticles(1, dynamic_cast<Texture2D*>(this->bangTextures[1]));

	this->bamTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->bamTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->bamTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->bamTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->bamTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->bamTextEmitter.SetParticleRotation(ESPInterval(-15));
	this->bamTextEmitter.SetParticleSize(ESPInterval(1), ESPInterval(1));

	TextLabel2D bamTitleText(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), MainMenuDisplayState::TITLE_BAM_TEXT);
	bamTitleText.SetDropShadow(Colour(0,0,0), 0.1f);
	this->bamTextEmitter.SetParticles(1, bamTitleText);

	// "Blammo!?!" Bang title text
	this->blammoEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->blammoEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->blammoEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->blammoEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->blammoEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->blammoEmitter.SetParticleRotation(ESPInterval(-8));
	this->blammoEmitter.SetParticleSize(ESPInterval(12.0f), ESPInterval(6.5f));
	this->blammoEmitter.SetParticles(1, dynamic_cast<Texture2D*>(this->bangTextures[2]));

	this->blammoTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->blammoTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->blammoTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->blammoTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->blammoTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->blammoTextEmitter.SetParticleRotation(ESPInterval(-8));
	this->blammoTextEmitter.SetParticleSize(ESPInterval(1), ESPInterval(1));

	TextLabel2D blammoTitleText(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), 
		MainMenuDisplayState::TITLE_BLAMMO_TEXT);
	blammoTitleText.SetDropShadow(Colour(0,0,0), 0.1f);
	this->blammoTextEmitter.SetParticles(1, blammoTitleText);
}

/**
 * Used as a private helper method to initialize the menu and other related display stuffs.
 */
void MainMenuDisplayState::InitializeMainMenu() {
	const float dropShadowAmtSm = 0.15f;
	const float dropShadowAmtLg = 0.15f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	// Setup the main menu attributes
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, this->display->GetDisplayHeight() - MENU_Y_INDENT);

	// Setup the main menu (top-most menu)
	this->mainMenu = new GameMenu(menuTopLeftCorner);
	this->mainMenu->AddEventHandler(this->mainMenuEventHandler);
	this->mainMenu->SetColourScheme(MainMenuDisplayState::MENU_ITEM_IDLE_COLOUR, MainMenuDisplayState::MENU_ITEM_SEL_COLOUR, 
																	MainMenuDisplayState::MENU_ITEM_ACTIVE_COLOUR, MainMenuDisplayState::MENU_ITEM_GREYED_COLOUR);

	// Setup submenues
	this->InitializeOptionsSubMenu();

	// Add items to the menu in their order (first to last)
	TextLabel2D tempLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium),  NEW_GAME_MENUITEM);
	TextLabel2D tempLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Big), NEW_GAME_MENUITEM);

	tempLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	tempLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);

	this->newGameMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	tempLabelSm.SetText(PLAY_LEVEL_MENUITEM);
	tempLabelLg.SetText(PLAY_LEVEL_MENUITEM);
	this->playLevelMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	// Setup the options item in the main menu and its submenu
	tempLabelSm.SetText(OPTIONS_MENUITEM);
	tempLabelLg.SetText(OPTIONS_MENUITEM);
	this->optionsMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, this->optionsSubMenu);
	
	tempLabelSm.SetText(EXIT_MENUITEM);
	tempLabelLg.SetText(EXIT_MENUITEM);
	this->exitGameMenuItemIndex = this->mainMenu->AddMenuItem(tempLabelSm, tempLabelLg, NULL);

	this->mainMenu->SetSelectedMenuItem(this->newGameMenuItemIndex);
	debug_opengl_state();
}

/**
 * Private helper function for initializing the options sub menu.
 */
void MainMenuDisplayState::InitializeOptionsSubMenu() {
	// Setup the options menu attributes
	const float dropShadowAmtSm = 0.10f;
	const float dropShadowAmtLg = 0.15f;
	const Colour dropShadowColour = Colour(0, 0, 0);

	TextLabel2D subMenuLabelSm = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Small),  "Fullscreen");
	TextLabel2D subMenuLabelLg = TextLabel2D(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::AllPurpose, GameFontAssetsManager::Medium), "Fullscreen");
	subMenuLabelSm.SetDropShadow(dropShadowColour, dropShadowAmtSm);
	subMenuLabelLg.SetDropShadow(dropShadowColour, dropShadowAmtLg);

	// Options Menu
	this->optionsSubMenu = new GameSubMenu();
	this->optionsSubMenu->AddEventHandler(this->optionsMenuEventHandler);
	this->optionsSubMenu->SetPaddingBetweenMenuItems(MENU_ITEM_PADDING);
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
	this->optionsVSyncIndex = this->optionsSubMenu->AddMenuItem(this->vSyncMenuItem);

	// Add the resolution selection item
	subMenuLabelSm.SetText("Resolution");
	subMenuLabelLg.SetText("Resolution");
	
	std::vector<std::string> resolutionOptions;
	resolutionOptions.reserve(12);
	resolutionOptions.push_back("800 x 600");
	resolutionOptions.push_back("1024 x 768");
	resolutionOptions.push_back("1152 x 864");
	resolutionOptions.push_back("1280 x 800");
	resolutionOptions.push_back("1280 x 960");
	resolutionOptions.push_back("1280 x 1024");
	resolutionOptions.push_back("1360 x 768");
	resolutionOptions.push_back("1440 x 900");
	resolutionOptions.push_back("1600 x 1200");
	resolutionOptions.push_back("1680 x 1050");
	resolutionOptions.push_back("1920 x 1080");
	resolutionOptions.push_back("1920 x 1200");

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
	this->optionsResolutionIndex = this->optionsSubMenu->AddMenuItem(this->resolutionMenuItem);

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
	if (this->changeToPlayGameState) {
		this->display->SetCurrentState(new StartGameDisplayState(this->display));
		return;
	}
	
	const int DISPLAY_WIDTH		= this->display->GetDisplayWidth();
	const int DISPLAY_HEIGHT	= this->display->GetDisplayHeight();

	this->menuFBO->BindFBObj();

	// Render background stuffs (behind the menu)
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->RenderBackgroundEffects(dT);

	// Render the title
	this->RenderTitle();

	// Render the menu
	Point2D menuTopLeftCorner = Point2D(MENU_X_INDENT, DISPLAY_HEIGHT - MENU_Y_INDENT);
	this->mainMenu->SetTopLeftCorner(menuTopLeftCorner);
	this->mainMenu->Draw(dT);

	// Fade-in/out overlay
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(DISPLAY_WIDTH, DISPLAY_HEIGHT, 1.0f, 
																									 ColourRGBA(1, 1, 1, this->fadeAnimation.GetInterpolantValue()));
	glDisable(GL_BLEND);
	
	this->menuFBO->UnbindFBObj();

	// Do bloom on the menu screen and draw it
	this->bloomEffect->Draw(DISPLAY_WIDTH, DISPLAY_HEIGHT, dT);
	this->menuFBO->GetFBOTexture()->RenderTextureToFullscreenQuad();

	debug_opengl_state();

	this->fadeAnimation.Tick(dT);
}

/**
 * Renders the title for the menu screen.
 */
void MainMenuDisplayState::RenderTitle() {
	const float CAM_DIST_FROM_ORIGIN = 20.0f;
	
	const float MAX_Y_COORD = CAM_DIST_FROM_ORIGIN * tan(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS) / 2.0f);
	const float MAX_X_COORD = static_cast<float>(this->display->GetDisplayWidth()) /  static_cast<float>(this->display->GetDisplayHeight()) * MAX_Y_COORD;
	const float MIN_X_COORD = -MAX_X_COORD;

	Camera tempCamera;
	tempCamera.SetPerspective(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	tempCamera.Move(Vector3D(0, 0, CAM_DIST_FROM_ORIGIN));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	tempCamera.ApplyCameraTransform(0.0);
	
	/*
	// Draw debug grid for coordinates system
	const int TEST_WIDTH = 15;
	const int TEST_HEIGHT = 15;
	glLineWidth(1.0f);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	for (int x = -TEST_WIDTH; x < TEST_WIDTH; x++) {
			glVertex2f(x, TEST_HEIGHT);
			glVertex2f(x, -TEST_HEIGHT);
	}
	for (int y = -TEST_HEIGHT; y < TEST_HEIGHT; y++) {
			glVertex2f(-TEST_WIDTH, y);
			glVertex2f(TEST_WIDTH, y);
	}
	glEnd();
	*/

	// Draw the "Biff!" Text
	const Point3D BIFF_EMIT_COORD = Point3D(MIN_X_COORD + 5.0f, MAX_Y_COORD - 2.5f, 0);

	this->biffEmitter.SetEmitPosition(BIFF_EMIT_COORD);
	this->biffEmitter.Tick(0.1);
	this->biffEmitter.Draw(tempCamera);

	this->biffTextEmitter.SetEmitPosition(BIFF_EMIT_COORD);
	this->biffTextEmitter.Tick(0.1);
	this->biffTextEmitter.Draw(tempCamera);

	// Draw the "Bam!!" Text
	const Point3D BAM_EMIT_COORD = Point3D(MIN_X_COORD + 10.5f, MAX_Y_COORD - 4.0f, 0);

	this->bamEmitter.SetEmitPosition(BAM_EMIT_COORD);
	this->bamEmitter.Tick(0.1);
	this->bamEmitter.Draw(tempCamera);
	
	this->bamTextEmitter.SetEmitPosition(BAM_EMIT_COORD);
	this->bamTextEmitter.Tick(0.1);
	this->bamTextEmitter.Draw(tempCamera);

	// Draw the "Blammo!?!" Text
	const Point3D BLAMMO_EMIT_COORD = Point3D(MIN_X_COORD + 16.0f, MAX_Y_COORD - 6.0f, 0);

	this->blammoEmitter.SetEmitPosition(BLAMMO_EMIT_COORD);
	this->blammoEmitter.Tick(0.1);
	this->blammoEmitter.Draw(tempCamera);

	this->blammoTextEmitter.SetEmitPosition(BLAMMO_EMIT_COORD);
	this->blammoTextEmitter.Tick(0.1);
	this->blammoTextEmitter.Draw(tempCamera);

	glPopMatrix();
}

/**
 * Renders the background effects for the menu screen. These include
 * random bangs and booms, etc. going off all over the place.
 */
void MainMenuDisplayState::RenderBackgroundEffects(double dT) {
	const float CAM_DIST_FROM_ORIGIN = 20.0f;	
	
	const float MAX_Z_COORD = CAM_DIST_FROM_ORIGIN / 4.0f;
	const float MIN_Z_COORD = -MAX_Z_COORD;
	const float MAX_Y_COORD = CAM_DIST_FROM_ORIGIN * tan(Trig::degreesToRadians(Camera::FOV_ANGLE_IN_DEGS) / 2.0f);
	const float MIN_Y_COORD = -MAX_Y_COORD;
	const float MAX_X_COORD = static_cast<float>(this->display->GetDisplayWidth()) /  static_cast<float>(this->display->GetDisplayHeight()) * MAX_Y_COORD;
	const float MIN_X_COORD = -MAX_X_COORD;	
	
	// Insert a bunch of bang-onomatopiea effects for drawing if we're running low on them
	const float MAX_Y_COORD_BG_EFFECTS = (MAX_Y_COORD - 6.0f);
	while (this->randomBGParicles.size() < 20) {
		this->InsertBangEffectIntoBGEffects(MIN_X_COORD, MAX_X_COORD, MIN_Y_COORD, MAX_Y_COORD_BG_EFFECTS, MIN_Z_COORD, MAX_Z_COORD);
	}

	Camera tempCamera;
	tempCamera.SetPerspective(this->display->GetDisplayWidth(), this->display->GetDisplayHeight());
	tempCamera.Move(Vector3D(0, 0, CAM_DIST_FROM_ORIGIN));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	tempCamera.ApplyCameraTransform(0.0);

	// Go through all the active emitters and tick/draw them
	for (std::list<ESPPointEmitter*>::iterator iter = this->randomBGParicles.begin(); iter != this->randomBGParicles.end();) {
		ESPPointEmitter* currEmitter = *iter;
		currEmitter->Tick(dT);
		currEmitter->Draw(tempCamera);

		if (currEmitter->IsDead()) {
			iter = this->randomBGParicles.erase(iter);
			delete currEmitter;
			currEmitter = NULL;
		}
		else {
			iter++;
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
	Texture2D* randomBangTex = dynamic_cast<Texture2D*>(this->bangTextures[randomBangTexIndex]);
	
	// Establish some of the values we will use to create the emitter for a 'BANG!" effect
	ESPInterval bangLifeInterval(1.5f, 3.0f);
	ESPInterval randomSpawnInterval(0.0f, 2.5f);
	ESPInterval xCoordInterval(minX, maxX);
	ESPInterval yCoordInterval(minY, maxY);
	ESPInterval zCoordInterval(minZ, maxZ);
	Point3D emitCenter  = Point3D(xCoordInterval.RandomValueInInterval(), yCoordInterval.RandomValueInInterval(), zCoordInterval.RandomValueInInterval());
	
	float randomSpawn = randomSpawnInterval.RandomValueInInterval();
	float randomLife  = bangLifeInterval.RandomValueInInterval();

	// Create an emitter for the bang texture
	ESPPointEmitter* bangEffect = new ESPPointEmitter();
	
	// Set up the emitter...
	bangEffect->SetSpawnDelta(ESPInterval(randomSpawn));
	bangEffect->SetNumParticleLives(1);
	bangEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangEffect->SetParticleLife(ESPInterval(randomLife));
	bangEffect->SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	bangEffect->SetParticleAlignment(ESP::ViewPointAligned);
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

	// Add effectors to the bang effect
	bangEffect->AddEffector(&this->particleFadeInAndOut);
	bangEffect->AddEffector(&this->particleMediumGrowth);
	
	// Add the bang particle...
	bangEffect->SetParticles(1, randomBangTex);

	// Create an emitter for the sound of onomatopeia of the breaking block
	ESPPointEmitter* bangOnoEffect = new ESPPointEmitter();
	// Set up the emitter...
	bangOnoEffect->SetSpawnDelta(ESPInterval(randomSpawn));
	bangOnoEffect->SetNumParticleLives(1);
	bangOnoEffect->SetInitialSpd(ESPInterval(0.0f, 0.0f));
	bangOnoEffect->SetParticleLife(ESPInterval(randomLife));
	bangOnoEffect->SetParticleSize(ESPInterval(0.7f, 1.0f), ESPInterval(1.0f, 1.0f));
	bangOnoEffect->SetParticleRotation(ESPInterval(-20.0f, 20.0f));
	bangOnoEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f, 0.2f));
	bangOnoEffect->SetParticleAlignment(ESP::ViewPointAligned);
	bangOnoEffect->SetEmitPosition(emitCenter);
	
	// Add effectors...
	bangOnoEffect->AddEffector(&this->particleFadeInAndOut);
	bangOnoEffect->AddEffector(&this->particleSmallGrowth);

	// Add the single text particle to the emitter with the severity of the effect...
	TextLabel2D bangTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Medium), "");
	bangTextLabel.SetColour(Colour(1, 1, 1));
	bangTextLabel.SetDropShadow(Colour(0, 0, 0), 0.1f);
	
	Onomatoplex::Extremeness randomExtremeness = Onomatoplex::Generator::GetInstance()->GetRandomExtremeness(Onomatoplex::WEAK, Onomatoplex::UBER);
	bangOnoEffect->SetParticles(1, bangTextLabel, Onomatoplex::EXPLOSION, randomExtremeness);

	// Add the particles to the list of background effects
	this->randomBGParicles.push_back(bangEffect);
	this->randomBGParicles.push_back(bangOnoEffect);
}

/**
 * Directly read key presses to manipulate the selections
 * in the main menu.
 */
void MainMenuDisplayState::KeyPressed(SDLKey key) {
	// Tell the main menu about the key pressed event
	this->mainMenu->KeyPressed(key);
}

void MainMenuDisplayState::DisplaySizeChanged(int width, int height) {
	delete this->menuFBO;
	this->menuFBO = new FBObj(this->display->GetDisplayWidth(), this->display->GetDisplayHeight(), Texture::Nearest, FBObj::NoAttachment);
	this->SetupBloomEffect();
}

/**
 * Handle activation events for items in the main menu.
 */
void MainMenuDisplayState::MainMenuEventHandler::GameMenuItemActivatedEvent(int itemIndex) {
	if (itemIndex == this->mainMenuState->newGameMenuItemIndex) {
		debug_output("Selected " << NEW_GAME_MENUITEM << " from menu");
		this->mainMenuState->changeToPlayGameState = true;
	}
	else if (itemIndex == this->mainMenuState->playLevelMenuItemIndex) {
		debug_output("Selected " << PLAY_LEVEL_MENUITEM << " from menu");
	}
	else if (itemIndex == this->mainMenuState->optionsMenuItemIndex) {
		debug_output("Selected " << OPTIONS_MENUITEM << " from menu");
	}
	else if (itemIndex == this->mainMenuState->exitGameMenuItemIndex) {
		// TODO: Put a dialogue for "Are you sure..."
		this->mainMenuState->display->QuitGame();
	}
	else {
		assert(false);
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
}

/**
 * Handle changes in the options for the game.
 */
void MainMenuDisplayState::OptionsSubMenuEventHandler::GameMenuItemChangedEvent(int itemIndex) {
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