/**
 * InTutorialGameDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "InTutorialGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "TutorialHint.h"
#include "TutorialEventsListener.h"
#include "TutorialHintListeners.h"
#include "ButtonTutorialHint.h"
#include "PopupTutorialHint.h"
#include "LivesLeftHUD.h"

InTutorialGameDisplayState::InTutorialGameDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display),
tutorialListener(new TutorialEventsListener(display)), boostCountdownHUD() {

    // Disable the paddle release timer for the tutorial
    PlayerPaddle::SetEnablePaddleReleaseTimer(false);
    // Make the boost time longer for the tutorial
    BallBoostModel::SetMaxBulletTimeDuration(5.0);
    
    // Set the life HUD to display an infinite number of lives
    this->display->GetAssets()->GetLifeHUD()->ToggleInfiniteLivesDisplay(true);

	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
    this->display->GetModel()->SetPauseState(GameModel::NoPause);
	this->display->GetCamera().ClearCameraShake();

    // Initialize the tutorial hints
    this->InitTutorialHints();

    // Register the tutorial listener (for knowing when to show and hide tutorial tips)
    GameEventManager::Instance()->RegisterGameEventListener(this->tutorialListener);

    this->boostCountdownHUD.Reset();

	debug_opengl_state();
}

InTutorialGameDisplayState::~InTutorialGameDisplayState() {
    // Re-enable any unusual stuff that was disabled for the tutorial
    PlayerPaddle::SetEnablePaddleReleaseTimer(true);
    PlayerPaddle::SetEnablePaddleRelease(true);
    BallBoostModel::SetMaxBulletTimeDuration(BallBoostModel::DEFAULT_BULLET_TIME_DURATION);
    this->display->GetAssets()->GetLifeHUD()->ToggleInfiniteLivesDisplay(false);

    // Clean up all the tutorial hints
    for (std::vector<TutorialHint*>::iterator iter = this->tutorialHints.begin();
         iter != this->tutorialHints.end(); ++iter) {
        TutorialHint* hint = *iter;
        delete hint;
        hint = NULL;
    }
    this->tutorialHints.clear();

    // Unregister and delete the tutorial events listener
    GameEventManager::Instance()->UnregisterGameEventListener(this->tutorialListener);
    delete this->tutorialListener;
    this->tutorialListener = NULL;
}

void InTutorialGameDisplayState::RenderFrame(double dT) {
	this->renderPipeline.RenderFrame(dT);
    
    // Update and draw tutorial graphics hints
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    //Camera& camera = this->display->GetCamera();
    Camera::PushWindowCoords();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

    double actualDt = dT / this->display->GetModel()->GetTimeDialationFactor();
    for (std::vector<TutorialHint*>::iterator iter = this->tutorialHints.begin();
         iter != this->tutorialHints.end(); ++iter) {
        TutorialHint* hint = *iter;
        hint->Draw(actualDt, this->display->GetCamera());
    }

    this->boostCountdownHUD.Draw(this->display->GetCamera(), *this->display->GetModel(), actualDt);

    glPopMatrix();

    Camera::PopWindowCoords();

    glPopAttrib();
    debug_opengl_state();

    //this->tutorialListener->Tick(dT);

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif
}

void InTutorialGameDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton) {
    this->tutorialListener->ButtonPressed(pressedButton);
    
    // When the user wants to access the in-game menu...
	if (pressedButton == GameControl::EscapeButtonAction || pressedButton == GameControl::PauseButtonAction) {
		// Go to the next state
		this->display->SetCurrentStateNoDeletePreviousState(new InGameMenuState(this->display, this));
	}
}

void InTutorialGameDisplayState::ButtonReleased(const GameControl::ActionButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

void InTutorialGameDisplayState::MousePressed(const GameControl::MouseButton& pressedButton) {
    this->tutorialListener->MousePressed(pressedButton);
}

void InTutorialGameDisplayState::MouseReleased(const GameControl::MouseButton& releasedButton) {
    UNUSED_PARAMETER(releasedButton);
}

void InTutorialGameDisplayState::DisplaySizeChanged(int width, int height) {
    this->display->GetAssets()->GetFBOAssets()->ResizeFBOAssets(width, height);
}

void InTutorialGameDisplayState::InitTutorialHints() {
    assert(this->tutorialHints.empty());
    assert(this->tutorialListener != NULL);

    GameTutorialAssets* tutorialAssets = this->display->GetAssets()->GetTutorialAssets();
    assert(tutorialAssets != NULL);
    //Vector2D negHalfLevelDim = -0.5f * this->display->GetModel()->GetLevelUnitDimensions();
    const Camera& camera = this->display->GetCamera();

    std::list<GameViewConstants::KeyboardButtonType> keyboardButtonTypes;
    std::list<GameViewConstants::XBoxButtonType> xboxButtonTypes;
    std::list<std::string> buttonTexts;
    std::list<Colour> buttonColours;

    // Tutorial hints for moving the paddle around
    ButtonTutorialHint* movePaddleHint = new ButtonTutorialHint(tutorialAssets, "Move");
    movePaddleHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
    
    keyboardButtonTypes.clear();
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
    buttonTexts.clear();
    buttonTexts.push_back("A");
    buttonTexts.push_back("D");

    movePaddleHint->SetListener(new MovePaddleHintListener());
    movePaddleHint->SetKeyboardButtons(keyboardButtonTypes, buttonTexts);
    movePaddleHint->SetTopLeftCorner((camera.GetWindowWidth() - movePaddleHint->GetWidth()) / 2.0f, 
        movePaddleHint->GetHeight() + 150.0f);

    this->tutorialListener->SetMovePaddleHint(movePaddleHint);
    this->tutorialHints.push_back(movePaddleHint);
    
    // Tutorial hint for firing the ball
    ButtonTutorialHint* shootBallHint = new ButtonTutorialHint(tutorialAssets, "Shoot Ball");
    shootBallHint->SetXBoxButton(GameViewConstants::XBoxPushButton, "A", GameViewConstants::GetInstance()->XBOX_CONTROLLER_A_BUTTON_COLOUR);
    shootBallHint->SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    shootBallHint->SetTopLeftCorner((camera.GetWindowWidth() - shootBallHint->GetWidth()) / 2.0f, shootBallHint->GetHeight() + 150.0f);
    
    this->tutorialListener->SetShootBallHint(shootBallHint);
    this->tutorialHints.push_back(shootBallHint);

    // Tutorial hint for firing weapons
    ButtonTutorialHint* fireWeaponHint = new ButtonTutorialHint(tutorialAssets, "Fire Weapons");

    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxPushButton);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("A");
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(GameViewConstants::GetInstance()->XBOX_CONTROLLER_A_BUTTON_COLOUR);
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    fireWeaponHint->SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    fireWeaponHint->SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    fireWeaponHint->SetTopLeftCorner((camera.GetWindowWidth() - fireWeaponHint->GetWidth()) / 2.0f,
        fireWeaponHint->GetHeight() + 150.0f);
    
    this->tutorialListener->SetFireWeaponHint(fireWeaponHint);
    this->tutorialHints.push_back(fireWeaponHint);

    // Tutorial hint for falling items
    // TODO ?

    // Tutorial hints for boosting
    ButtonTutorialHint* startingToBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    startingToBoostHint->SetActionName("Enter Boost Mode : Hold ", false);
    startingToBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    startingToBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    startingToBoostHint->SetTopLeftCorner((camera.GetWindowWidth() - startingToBoostHint->GetWidth()) / 2.0f,
        startingToBoostHint->GetHeight() + 200.0f);

    this->tutorialListener->SetStartBoostHint(startingToBoostHint);
    this->tutorialHints.push_back(startingToBoostHint);

    ButtonTutorialHint* holdBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    holdBoostHint->SetActionName("Now, hold and move ", false);
    holdBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    holdBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    holdBoostHint->SetTopLeftCorner((camera.GetWindowWidth() - holdBoostHint->GetWidth()) / 2.0f,
        camera.GetWindowHeight() - 100.0f);

    this->tutorialListener->SetHoldBoostHint(holdBoostHint);
    this->tutorialHints.push_back(holdBoostHint);

    ButtonTutorialHint* doBoostHint = new ButtonTutorialHint(tutorialAssets, "Perform Boost");

    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    doBoostHint->SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    doBoostHint->SetMouseButton(GameViewConstants::RightMouseButton, "RMB");
    doBoostHint->SetTopLeftCorner((camera.GetWindowWidth() - doBoostHint->GetWidth()) / 2.0f,
        doBoostHint->GetHeight() + 100.0f);

    this->tutorialListener->SetDoBoostHint(doBoostHint);
    this->tutorialHints.push_back(doBoostHint);

/*
    // TODO: REMOVE ALL POP-UP HINTS AND REPLACE WITH BUTTON HINTS!!!!

    // Pop-up tutorial hints - these pause the game and explain to the user how to play
    static const float TITLE_TEXT_SCALE = 1.33f;
    static const float BODY_TEXT_SCALE  = 0.75f;
    static const size_t POPUP_TUTORIAL_HINT_WIDTH = 700;

    GameModel* gameModel = this->display->GetModel();
    
    // Boosting tutorial hint pop-up
    PopupTutorialHint* boostPopupHint = new PopupTutorialHint(gameModel, POPUP_TUTORIAL_HINT_WIDTH);
    boostPopupHint->SetListener(new BoostPopupHintListener(startingToBoostHint));
    DecoratorOverlayPane* boostPopupPane = boostPopupHint->GetPane();
    boostPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    boostPopupPane->AddText("Ball Boosting", Colour(1,1,1), TITLE_TEXT_SCALE);
    boostPopupPane->SetLayoutType(DecoratorOverlayPane::TwoColumn);
    boostPopupPane->AddText(
        std::string("During gameplay the boost gauge in the top-left will fill up making boosts available."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* boostHUDImg = tutorialAssets->GetBoostTutorialHUDTexture();
    boostPopupPane->AddImage(256, boostHUDImg);

    boostPopupPane->AddText(
        std::string("Activate it by holding down the right analog stick or the left mouse button. ") +
        std::string("This will momentarily slow down time so you can redirect the ball. Simply let go to cancel the boost."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* boostDirImg = tutorialAssets->GetBoostTutorialDirTexture();
    boostPopupPane->AddImage(256, boostDirImg);

    boostPopupPane->AddText(std::string("Either way, act quickly because the ball's got places to be."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    std::vector<std::string> continueOption;
    continueOption.reserve(1);
    continueOption.push_back("Continue");
    boostPopupPane->SetSelectableOptions(continueOption, 0);

    this->tutorialListener->SetBoostPopupHint(boostPopupHint);
    this->tutorialHints.push_back(boostPopupHint);

    // Multiplier tutorial hint pop-up
    PopupTutorialHint* multPopupHint  = new PopupTutorialHint(gameModel, POPUP_TUTORIAL_HINT_WIDTH);
    //multPopupHint->SetListener(...);
    DecoratorOverlayPane* multPopupPane = multPopupHint->GetPane();
    multPopupPane->SetLayoutType(DecoratorOverlayPane::Centered);
    multPopupPane->AddText(std::string("Points and Multipliers"), Colour(1,1,1), TITLE_TEXT_SCALE);
    multPopupPane->SetLayoutType(DecoratorOverlayPane::TwoColumn);
    multPopupPane->AddText(
        std::string("Maximize your pointage by consecutively destroying blocks to earn higher multipliers."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    const Texture2D* multiplierImg = tutorialAssets->GetMultiplierTutorialTexture();
    multPopupPane->AddImage(256, multiplierImg);

    multPopupPane->AddText(
        std::string("Any multiplier will reset when a ball hits the paddle or when you lose your balls."),
        Colour(1,1,1), BODY_TEXT_SCALE);
    multPopupPane->AddText(
        std::string("Though tempting to use, certain beneficial items (lasers!) will limit your score."),
        Colour(1,1,1), BODY_TEXT_SCALE);

    multPopupPane->SetSelectableOptions(continueOption, 0);

    this->tutorialListener->SetMultiplierPopupHint(multPopupHint);
    this->tutorialHints.push_back(multPopupHint);
    */
}