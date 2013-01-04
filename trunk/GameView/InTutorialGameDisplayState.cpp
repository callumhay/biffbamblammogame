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
#include "BallBoostHUD.h"

InTutorialGameDisplayState::InTutorialGameDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display), beforeTutorialDifficulty(display->GetCachedDifficulty()),
tutorialListener(new TutorialEventsListener(display)), boostCountdownHUD() {

    GameModel* model = this->display->GetModel();
    assert(model != NULL);

    // Disable the paddle release timer for the tutorial
    PlayerPaddle::SetEnablePaddleReleaseTimer(false);
    // Make the boost time longer for the tutorial
    BallBoostModel::SetMaxBulletTimeDuration(5.0);
    
    // Set the life HUD to display an infinite number of lives
    this->display->GetAssets()->GetLifeHUD()->ToggleInfiniteLivesDisplay(true);

	// Clear up any stuff to an initial state in cases where things might still 
	// be set unchanged from a previously loaded game
    model->SetPauseState(GameModel::NoPause);
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
    for (std::vector<TutorialHint*>::iterator iter = this->noDepthTutorialHints.begin();
         iter != this->noDepthTutorialHints.end(); ++iter) {
        TutorialHint* hint = *iter;
        delete hint;
        hint = NULL;
    }
    this->noDepthTutorialHints.clear();

    // Unregister and delete the tutorial events listener
    GameEventManager::Instance()->UnregisterGameEventListener(this->tutorialListener);
    delete this->tutorialListener;
    this->tutorialListener = NULL;

    // Place the player difficulty back to what it was before the tutorial (the level complete page
    // will ask the player to switch their difficulty if they feel so inclined)
    this->display->GetModel()->SetDifficulty(this->beforeTutorialDifficulty);
}

void InTutorialGameDisplayState::RenderFrame(double dT) {
    GameModel* gameModel = this->display->GetModel();
	double actualDt = dT / gameModel->GetTimeDialationFactor();
    Camera& camera = this->display->GetCamera();

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
    
    for (std::vector<TutorialHint*>::iterator iter = this->noDepthTutorialHints.begin();
         iter != this->noDepthTutorialHints.end(); ++iter) {
        TutorialHint* hint = *iter;
        hint->Draw(camera);
        hint->Tick(actualDt);
    }

    this->boostCountdownHUD.Draw(camera, *this->display->GetModel(), actualDt);

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
    assert(this->noDepthTutorialHints.empty());
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
    this->noDepthTutorialHints.push_back(movePaddleHint);
    
    // Tutorial hint for firing the ball
    ButtonTutorialHint* shootBallHint = new ButtonTutorialHint(tutorialAssets, "Shoot Ball");
    shootBallHint->SetXBoxButton(GameViewConstants::XBoxPushButton, "A", GameViewConstants::GetInstance()->XBOX_CONTROLLER_A_BUTTON_COLOUR);
    shootBallHint->SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    shootBallHint->SetTopLeftCorner((camera.GetWindowWidth() - shootBallHint->GetWidth()) / 2.0f, shootBallHint->GetHeight() + 150.0f);
    
    this->tutorialListener->SetShootBallHint(shootBallHint);
    this->noDepthTutorialHints.push_back(shootBallHint);

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
    this->noDepthTutorialHints.push_back(fireWeaponHint);

    // Tutorial hint for falling items
    // TODO ?

    // Tutorial hints for boosting
    ButtonTutorialHint* boostAvailableHint = new ButtonTutorialHint(tutorialAssets, "");
    boostAvailableHint->SetActionName("Boost Available!", false);
    boostAvailableHint->SetFlashing(true);
    boostAvailableHint->SetTopLeftCorner(BallBoostHUD::H_BORDER_SPACING,
        camera.GetWindowHeight() - (BallBoostHUD::V_BORDER_SPACING + BallBoostHUD::BALL_BOOST_HUD_HEIGHT + 10));
    this->tutorialListener->SetBoostAvailableHint(boostAvailableHint);
    this->noDepthTutorialHints.push_back(boostAvailableHint);

    ButtonTutorialHint* startingToBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    startingToBoostHint->SetActionName("Boost Mode = Hold ", false);
    startingToBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    startingToBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    startingToBoostHint->SetTopLeftCorner((camera.GetWindowWidth() - startingToBoostHint->GetWidth()) / 2.0f,
        startingToBoostHint->GetHeight() + 200.0f);

    this->tutorialListener->SetStartBoostHint(startingToBoostHint);
    this->noDepthTutorialHints.push_back(startingToBoostHint);

    ButtonTutorialHint* holdBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    holdBoostHint->SetActionName("Now, hold and move ", false);
    holdBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    holdBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    holdBoostHint->SetTopLeftCorner((camera.GetWindowWidth() - holdBoostHint->GetWidth()) / 2.0f,
        camera.GetWindowHeight() - 100.0f);

    this->tutorialListener->SetHoldBoostHint(holdBoostHint);
    this->noDepthTutorialHints.push_back(holdBoostHint);

    ButtonTutorialHint* doBoostHint = new ButtonTutorialHint(tutorialAssets, "Perform Boost = ");

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
    this->noDepthTutorialHints.push_back(doBoostHint);

    // The Points hint is hidden behind the first layer of blocks and is uncovered as the level is played
    TextLabel2D pointHintTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
        GameFontAssetsManager::Small), "The longer the ball is in play = More points"); 
    pointHintTextLabel.SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    pointHintTextLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    
    //const GameLevel* level = this->display->GetModel()->GetCurrentLevel();
    ESPPointEmitter* pointsTutorialHintEmitter = this->renderPipeline.AddEmbededTutorialHint(
        Point3D(LevelPiece::PIECE_WIDTH * 5.0f, LevelPiece::PIECE_HEIGHT * 15.5f, LevelPiece::PIECE_DEPTH + 1.0f),
        pointHintTextLabel, true);
    assert(pointsTutorialHintEmitter != NULL);

    this->tutorialListener->SetPointsHintEmitter(pointsTutorialHintEmitter);

    // Add a hint about points...
    /*
    ButtonTutorialHint* pointsHint = new ButtonTutorialHint(tutorialAssets, "");
    pointsHint->SetActionName("The longer the ball is in play = More points", false);
    pointsHint->SetAlphaWhenShowing(0.75f);
    pointsHint->SetTopLeftCorner((camera.GetWindowWidth() - pointsHint->GetWidth()) / 2.0f,
        camera.GetWindowHeight() - 200.0f);

    this->tutorialListener->SetPointsHint(pointsHint);
    this->depthTutorialHints.push_back(pointsHint);
    */
}