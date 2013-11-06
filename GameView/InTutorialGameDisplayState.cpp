/**
 * InTutorialGameDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "InTutorialGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "TutorialHint.h"
#include "TutorialEventsListener.h"
#include "ButtonTutorialHint.h"
#include "PopupTutorialHint.h"
#include "LivesLeftHUD.h"
#include "BallBoostHUD.h"

InTutorialGameDisplayState::InTutorialGameDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display), beforeTutorialDifficulty(display->GetCachedDifficulty()),
tutorialListener(new TutorialEventsListener(display)), boostCountdownHUD(BallBoostModel::GetMaxBulletTimeDuration()),
tutorialAttentionEffect(display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()) {

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
    for (HintListIter iter = this->noDepthTutorialHints.begin();
         iter != this->noDepthTutorialHints.end(); ++iter) {
        TutorialHint* hint = *iter;
        delete hint;
        hint = NULL;
    }
    this->noDepthTutorialHints.clear();

    for (HintListIter iter = this->ballFollowTutorialHints.begin();
         iter != this->ballFollowTutorialHints.end(); ++iter) {

        TutorialHint* hint = *iter;
        delete hint;
        hint = NULL;
    }
    this->ballFollowTutorialHints.clear();

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
    GameBall* ball = gameModel->GetGameBalls().front();
	double actualDt = dT / gameModel->GetTimeDialationFactor();
    Camera& camera = this->display->GetCamera();

    this->renderPipeline.RenderFrameWithoutHUD(dT);

    if (gameModel->GetBallBoostModel() == NULL || gameModel->GetBallBoostModel()->GetBulletTimeState() == BallBoostModel::NotInBulletTime) {

        // Render a post effect for drawing attention to the tutorial hints...
        this->tutorialAttentionEffect.SetInputFBO(this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO());
        this->tutorialAttentionEffect.Draw(Camera::GetWindowWidth(), Camera::GetWindowHeight(), dT);
        this->display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()->GetFBOTexture()->RenderTextureToFullscreenQuad(1.0f);
    }
    else {
        this->tutorialAttentionEffect.SetAlpha(0.0f);
    }

    for (HintListIter iter = this->noDepthTutorialHints.begin(); iter != this->noDepthTutorialHints.end(); ++iter) {

        EmbededTutorialHint* hint = *iter;
        hint->Draw(camera);
        hint->Tick(actualDt);
    }

    static const float TEXT_BALL_BUFFER_SPACE_IN_PX = 2;
    Point3D ballHintPos(ball->GetCenterPosition2D() + Vector2D(ball->GetBounds().Radius(), 0.0f), 0.0f);
    ballHintPos += gameModel->GetCurrentLevelTranslation();
    
    float tempMVXfVals[16];
    glGetFloatv(GL_PROJECTION_MATRIX, tempMVXfVals);
    float viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);

    Vector4D temp = Matrix4x4(tempMVXfVals) * camera.GetViewTransform() * 
        gameModel->GetTransformInfo()->GetGameXYZTransform() * Vector4D(ballHintPos[0], ballHintPos[1], ballHintPos[2], 1.0f);

    double rhw = 1.0f / temp[3];
    ballHintPos[0] = (1 + temp[0] * rhw) * viewport[2] / 2.0f + viewport[0] + TEXT_BALL_BUFFER_SPACE_IN_PX;
    ballHintPos[1] = (1 + temp[1] * rhw) * viewport[3] / 2.0f + viewport[1];

    for (HintListIter iter = this->ballFollowTutorialHints.begin(); iter != this->ballFollowTutorialHints.end(); ++iter) {
        
        EmbededTutorialHint* hint = *iter;
        hint->SetTopLeftCorner(ballHintPos[0], ballHintPos[1] + hint->GetHeight()/2.0f);
        hint->Draw(camera);
        hint->Tick(actualDt);
    }

    const BallBoostModel* boostModel = this->display->GetModel()->GetBallBoostModel();
    if (boostModel != NULL && boostModel->GetBulletTimeState() == BallBoostModel::BulletTime) {
        this->boostCountdownHUD.SetTotalTimeUntilCountdownIsOver(BallBoostModel::GetMaxBulletTimeDuration());
        this->boostCountdownHUD.Draw(camera, actualDt, boostModel->GetTotalBulletTimeElapsed());
    }
    else {
        this->boostCountdownHUD.Reset();
    }

    this->renderPipeline.RenderHUD(dT);

    debug_opengl_state();

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif
}

void InTutorialGameDisplayState::ButtonPressed(const GameControl::ActionButton& pressedButton,
                                               const GameControl::ActionMagnitude& magnitude) {

    UNUSED_PARAMETER(magnitude);
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

    float scaleFactor = this->display->GetTextScalingFactor();

    GameTutorialAssets* tutorialAssets = this->display->GetAssets()->GetTutorialAssets();
    assert(tutorialAssets != NULL);

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

    movePaddleHint->SetListener(new MovePaddleHintListener(this));
    movePaddleHint->SetKeyboardButtons(keyboardButtonTypes, buttonTexts);
    movePaddleHint->SetTopLeftCorner((Camera::GetWindowWidth() - movePaddleHint->GetWidth()) / 2.0f, 
        movePaddleHint->GetHeight() + 150.0f);

    this->tutorialListener->SetMovePaddleHint(movePaddleHint);
    this->noDepthTutorialHints.push_back(movePaddleHint);
    
    // Tutorial hint for firing the ball
    ButtonTutorialHint* shootBallHint = new ButtonTutorialHint(tutorialAssets, "Shoot Ball");
    shootBallHint->SetXBoxButton(GameViewConstants::XBoxPushButton, "A", GameViewConstants::GetInstance()->XBOX_CONTROLLER_A_BUTTON_COLOUR);
    shootBallHint->SetKeyboardButton(GameViewConstants::KeyboardSpaceBar, "Space");
    shootBallHint->SetTopLeftCorner((Camera::GetWindowWidth() - shootBallHint->GetWidth()) / 2.0f, shootBallHint->GetHeight() + 150.0f);
    shootBallHint->SetListener(new HintListener(this));
    
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
    fireWeaponHint->SetTopLeftCorner((Camera::GetWindowWidth() - fireWeaponHint->GetWidth()) / 2.0f,
        fireWeaponHint->GetHeight() + 150.0f);

    this->tutorialListener->SetFireWeaponHint(fireWeaponHint);
    this->noDepthTutorialHints.push_back(fireWeaponHint);

    // Tutorial hints for boosting
    ButtonTutorialHint* boostAvailableHint = new ButtonTutorialHint(tutorialAssets, "");
    boostAvailableHint->SetActionNameWithSeparator("Boost Available!");
    boostAvailableHint->SetFlashing(true);
    boostAvailableHint->SetTopLeftCorner(BallBoostHUD::H_BORDER_SPACING,
        Camera::GetWindowHeight() - (BallBoostHUD::V_BORDER_SPACING + BallBoostHUD::BALL_BOOST_HUD_HEIGHT + 10));
    this->tutorialListener->SetBoostAvailableHint(boostAvailableHint);
    this->noDepthTutorialHints.push_back(boostAvailableHint);

    ButtonTutorialHint* startingToBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    startingToBoostHint->SetActionNameWithSeparator("Boost Mode = Hold ");
    startingToBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    startingToBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    startingToBoostHint->SetTopLeftCorner((Camera::GetWindowWidth() - startingToBoostHint->GetWidth()) / 2.0f,
        startingToBoostHint->GetHeight() + 200.0f);
    startingToBoostHint->SetListener(new SlowBallHintListener(this));

    this->tutorialListener->SetStartBoostHint(startingToBoostHint);
    this->noDepthTutorialHints.push_back(startingToBoostHint);

    ButtonTutorialHint* holdBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    holdBoostHint->SetActionNameWithSeparator("Now, hold and move ");
    holdBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    holdBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    holdBoostHint->SetTopLeftCorner((Camera::GetWindowWidth() - holdBoostHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f + 150.0f);

    this->tutorialListener->SetHoldBoostHint(holdBoostHint);
    this->noDepthTutorialHints.push_back(holdBoostHint);

    ButtonTutorialHint* doBoostPressToReleaseHint = new ButtonTutorialHint(tutorialAssets, "");
    doBoostPressToReleaseHint->SetActionNameWithSeparator("Perform Boost = ");
    xboxButtonTypes.clear();
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    xboxButtonTypes.push_back(GameViewConstants::XBoxTrigger);
    buttonTexts.clear();
    buttonTexts.push_back("RT");
    buttonTexts.push_back("LT");
    buttonColours.clear();
    buttonColours.push_back(Colour(1,1,1));
    buttonColours.push_back(Colour(1,1,1));

    doBoostPressToReleaseHint->SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    doBoostPressToReleaseHint->SetMouseButton(GameViewConstants::RightMouseButton, "RMB");
    doBoostPressToReleaseHint->SetTopLeftCorner((Camera::GetWindowWidth() - doBoostPressToReleaseHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f - 150.0f);

    this->tutorialListener->SetDoBoostPressToReleaseHint(doBoostPressToReleaseHint);
    this->noDepthTutorialHints.push_back(doBoostPressToReleaseHint);

    ButtonTutorialHint* doBoostSlingshotHint = new ButtonTutorialHint(tutorialAssets, "");
    doBoostSlingshotHint->SetActionNameWithSeparator("Perform Boost = Release ");
    doBoostSlingshotHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
    doBoostSlingshotHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    doBoostSlingshotHint->SetTopLeftCorner((Camera::GetWindowWidth() - doBoostPressToReleaseHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f - 150.0f);

    this->tutorialListener->SetDoBoostSlingshotHint(doBoostSlingshotHint);
    this->noDepthTutorialHints.push_back(doBoostSlingshotHint);

    const float multiplierHintScale = 0.70f * scaleFactor;
    const float multiplierHintWidth = 275;
    BasicMultiTutorialHint* multiplierHints = new BasicMultiTutorialHint(3.0, 1.0, 1.0);
    multiplierHints->PushHint("Destroying Blocks Increases Your Multiplier", multiplierHintScale, multiplierHintWidth);
    multiplierHints->PushHint("A Higher Multiplier Means More Points", multiplierHintScale, multiplierHintWidth);
    multiplierHints->PushHint("More Points Means More Stars", multiplierHintScale, multiplierHintWidth);
    this->ballFollowTutorialHints.push_back(multiplierHints);
    this->tutorialListener->SetMultiplierHints(multiplierHints);

    BasicTutorialHint* multiplierLostHint = new BasicTutorialHint(
        "Your Multiplier is Lost if a Ball Hits the Paddle", multiplierHintScale, multiplierHintWidth);
    this->ballFollowTutorialHints.push_back(multiplierLostHint);
    this->tutorialListener->SetMultiplierLostHint(multiplierLostHint);

    // The Points hint is hidden behind the first layer of blocks and is uncovered as the level is played
    TextLabel2D pointHintTextLabel(GameFontAssetsManager::GetInstance()->GetFont(GameFontAssetsManager::ExplosionBoom,
        GameFontAssetsManager::Small), "Less paddle hits = More points"); 
    pointHintTextLabel.SetColour(Colour(0.3882f, 0.72157f, 1.0f));
    pointHintTextLabel.SetDropShadow(Colour(0,0,0), 0.10f);
    
    //const GameLevel* level = this->display->GetModel()->GetCurrentLevel();
    ESPPointEmitter* pointsTutorialHintEmitter = this->renderPipeline.AddEmbededTutorialHint(
        Point3D(LevelPiece::PIECE_WIDTH * 5.0f, LevelPiece::PIECE_HEIGHT * 15.5f, LevelPiece::PIECE_DEPTH + 1.0f),
        pointHintTextLabel, true);
    assert(pointsTutorialHintEmitter != NULL);

    this->tutorialListener->SetPointsHintEmitter(pointsTutorialHintEmitter);
}

void InTutorialGameDisplayState::HintListener::OnTutorialHintShown() {
    this->state->tutorialAttentionEffect.CrossFadeIn(1.0);
}

void InTutorialGameDisplayState::HintListener::OnTutorialHintUnshown() {
    this->state->tutorialAttentionEffect.CrossFadeOut(0.5);
}

void InTutorialGameDisplayState::MovePaddleHintListener::OnTutorialHintShown() {
    HintListener::OnTutorialHintShown();
    // Disable the paddle's ability to release the ball until the player has moved
    PlayerPaddle::SetEnablePaddleRelease(false);
}
void InTutorialGameDisplayState::MovePaddleHintListener::OnTutorialHintUnshown() {
    HintListener::OnTutorialHintUnshown();
    PlayerPaddle::SetEnablePaddleRelease(true);
}

void InTutorialGameDisplayState::SlowBallHintListener::OnTutorialHintShown() {
    HintListener::OnTutorialHintShown();

    GameModel* gameModel = this->state->display->GetModel();
    if (gameModel->GetCurrentStateType() == GameState::BallInPlayStateType) {
        this->SetBallSpeed(0.5f * GameBall::GetSlowestSpeed());
    }
}
void InTutorialGameDisplayState::SlowBallHintListener::OnTutorialHintUnshown() {
    HintListener::OnTutorialHintUnshown();

    if (this->state->display->GetModel()->GetCurrentStateType() == GameState::BallInPlayStateType) {
        this->SetBallSpeed(GameBall::GetNormalSpeed());
    }
}

void InTutorialGameDisplayState::SlowBallHintListener::SetBallSpeed(float speed) {
    std::list<GameBall*>& balls = this->state->display->GetModel()->GetGameBalls();
    for (std::list<GameBall*>::iterator iter = balls.begin(); iter != balls.end(); ++iter) {
        GameBall* ball = *iter;
        ball->SetSpeed(speed);
    }
}