/**
 * InTutorialGameDisplayState.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "InTutorialGameDisplayState.h"
#include "InGameMenuState.h"
#include "GameDisplay.h"
#include "GameAssets.h"
#include "GameFBOAssets.h"
#include "TutorialHint.h"
#include "TutorialEventsListener.h"
#include "ButtonTutorialHint.h"
#include "PopupTutorialHint.h"
#include "LivesLeftHUD.h"
#include "BallBoostHUD.h"
#include "GameViewEventManager.h"

#include "../GameModel/GameTransformMgr.h"

#include "../WindowManager.h"

InTutorialGameDisplayState::InTutorialGameDisplayState(GameDisplay* display) :
DisplayState(display), renderPipeline(display), beforeTutorialDifficulty(display->GetCachedDifficulty()),
tutorialListener(new TutorialEventsListener(display)), boostCountdownHUD(BallBoostModel::GetMaxBulletTimeDuration()),
tutorialAttentionEffect(display->GetAssets()->GetFBOAssets()->GetFinalFullScreenFBO()) {

    // If arcade mode is enabled then we aren't as nice to the player during the tutorial...
    bool arcadeMode = GameDisplay::IsArcadeModeEnabled();

    GameModel* model = this->display->GetModel();
    assert(model != NULL);

    // Disable the paddle release timer for the tutorial
    PlayerPaddle::SetEnablePaddleReleaseTimer(arcadeMode);
    // Make the boost time longer for the tutorial
    BallBoostModel::SetMaxBulletTimeDuration(10.0);
    
    // Set the life HUD to display an infinite number of lives
    this->display->GetAssets()->GetLifeHUD()->ToggleInfiniteLivesDisplay(!arcadeMode);

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

    if (!WindowManager::GetInstance()->GetIsFullscreen()) {
        WindowManager::GetInstance()->ShowCursor(true);
    }
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

    for (HintListIter iter = this->noDepthTutorialHints.begin(); iter != this->noDepthTutorialHints.end(); ++iter) {

        EmbededTutorialHint* hint = *iter;
        hint->Tick(actualDt);
        hint->Draw(camera);
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
        hint->Tick(actualDt);
        hint->Draw(camera);
    }

    const BallBoostModel* boostModel = this->display->GetModel()->GetBallBoostModel();
    if (boostModel != NULL && boostModel->GetBulletTimeState() == BallBoostModel::BulletTime) {
        this->boostCountdownHUD.SetTotalTimeUntilCountdownIsOver(BallBoostModel::GetMaxBulletTimeDuration());
        this->boostCountdownHUD.DrawUsingTimeElapsed(camera, actualDt, boostModel->GetTotalBulletTimeElapsed());
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
    if (GameDisplay::IsArcadeModeEnabled()) {
        movePaddleHint->SetArcadeButton(GameViewConstants::ArcadeJoystick, "", Colour(1,1,1));
    }
    else {
        movePaddleHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Left Analog", Colour(1,1,1));
        keyboardButtonTypes.clear();
        keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
        keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
        buttonTexts.clear();
        buttonTexts.push_back("A");
        buttonTexts.push_back("D");
        movePaddleHint->SetKeyboardButtons(keyboardButtonTypes, buttonTexts);
    }
    movePaddleHint->SetTopLeftCorner((Camera::GetWindowWidth() - movePaddleHint->GetWidth()) / 2.0f, 
        movePaddleHint->GetHeight() + 150.0f);
    movePaddleHint->SetListener(new MovePaddleHintListener(this));

    this->tutorialListener->SetMovePaddleHint(movePaddleHint);
    this->noDepthTutorialHints.push_back(movePaddleHint);
    
    // Tutorial hint for firing the ball
    ButtonTutorialHint* shootBallHint = new ButtonTutorialHint(tutorialAssets, "Shoot Ball");
    if (GameDisplay::IsArcadeModeEnabled()) {
        shootBallHint->SetArcadeButton(GameViewConstants::ArcadeFireButton, "Fire", 
            GameViewConstants::GetInstance()->ARCADE_FIRE_BUTTON_COLOUR);
    }
    else {
        keyboardButtonTypes.clear();
        keyboardButtonTypes.push_back(GameViewConstants::KeyboardSpaceBar);
        keyboardButtonTypes.push_back(GameViewConstants::KeyboardChar);
        buttonTexts.clear();
        buttonTexts.push_back("Space");
        buttonTexts.push_back("W");
        shootBallHint->SetKeyboardButtons(keyboardButtonTypes, buttonTexts);

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

        shootBallHint->SetXBoxButtons(xboxButtonTypes, buttonTexts, buttonColours);
    }
    shootBallHint->SetTopLeftCorner((Camera::GetWindowWidth() - shootBallHint->GetWidth()) / 2.0f, shootBallHint->GetHeight() + 150.0f);
    shootBallHint->SetListener(new HintListener(this));
    
    this->tutorialListener->SetShootBallHint(shootBallHint);
    this->noDepthTutorialHints.push_back(shootBallHint);

    // Tutorial hint for firing weapons
    ButtonTutorialHint* fireWeaponHint = new ButtonTutorialHint(tutorialAssets, "Fire Weapons");
    if (GameDisplay::IsArcadeModeEnabled()) {
        fireWeaponHint->SetArcadeButton(GameViewConstants::ArcadeFireButton, "Fire", 
            GameViewConstants::GetInstance()->ARCADE_FIRE_BUTTON_COLOUR);
    }
    else {
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
    }
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
    if (GameDisplay::IsArcadeModeEnabled()) {
        startingToBoostHint->SetArcadeButton(GameViewConstants::ArcadeBoostButton, "Boost", 
            Colour(GameViewConstants::GetInstance()->ARCADE_BOOST_BUTTON_COLOUR));
    }
    else {
        startingToBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
        startingToBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    }
    startingToBoostHint->SetTopLeftCorner((Camera::GetWindowWidth() - startingToBoostHint->GetWidth()) / 2.0f,
        startingToBoostHint->GetHeight() + 200.0f);
    startingToBoostHint->SetListener(new SlowBallHintListener(this));

    this->tutorialListener->SetStartBoostHint(startingToBoostHint);
    this->noDepthTutorialHints.push_back(startingToBoostHint);

    ButtonTutorialHint* holdBoostHint = new ButtonTutorialHint(tutorialAssets, "");
    if (GameDisplay::IsArcadeModeEnabled()) {
        holdBoostHint->SetActionNameWithSeparator("Move ");
        holdBoostHint->SetArcadeButton(GameViewConstants::ArcadeJoystick, "", Colour(1,1,1));
    }
    else {
        holdBoostHint->SetActionNameWithSeparator("Hold and move ");
        holdBoostHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
        holdBoostHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    }
    holdBoostHint->SetTopLeftCorner((Camera::GetWindowWidth() - holdBoostHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f + 150.0f);

    this->tutorialListener->SetHoldBoostHint(holdBoostHint);
    this->noDepthTutorialHints.push_back(holdBoostHint);

    ButtonTutorialHint* doBoostPressToReleaseHint = new ButtonTutorialHint(tutorialAssets, "");
    doBoostPressToReleaseHint->SetActionNameWithSeparator("Perform Boost = ");
    if (GameDisplay::IsArcadeModeEnabled()) {
        doBoostPressToReleaseHint->SetArcadeButton(GameViewConstants::ArcadeFireButton, "Fire", 
            GameViewConstants::GetInstance()->ARCADE_FIRE_BUTTON_COLOUR);
    }
    else {
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
    }
    doBoostPressToReleaseHint->SetTopLeftCorner((Camera::GetWindowWidth() - doBoostPressToReleaseHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f - 150.0f);

    this->tutorialListener->SetDoBoostPressToReleaseHint(doBoostPressToReleaseHint);
    this->noDepthTutorialHints.push_back(doBoostPressToReleaseHint);

    ButtonTutorialHint* doBoostSlingshotHint = new ButtonTutorialHint(tutorialAssets, "");
    doBoostSlingshotHint->SetActionNameWithSeparator("Perform Boost = Release ");
    if (GameDisplay::IsArcadeModeEnabled()) {
        startingToBoostHint->SetArcadeButton(GameViewConstants::ArcadeBoostButton, "Boost", 
            Colour(GameViewConstants::GetInstance()->ARCADE_BOOST_BUTTON_COLOUR));
    }
    else {
        doBoostSlingshotHint->SetXBoxButton(GameViewConstants::XBoxAnalogStick, "Right Analog", Colour(1,1,1));
        doBoostSlingshotHint->SetMouseButton(GameViewConstants::LeftMouseButton, "LMB");
    }
    doBoostSlingshotHint->SetTopLeftCorner((Camera::GetWindowWidth() - doBoostPressToReleaseHint->GetWidth()) / 2.0f,
        Camera::GetWindowHeight()/2.0f - 150.0f);

    this->tutorialListener->SetDoBoostSlingshotHint(doBoostSlingshotHint);
    this->noDepthTutorialHints.push_back(doBoostSlingshotHint);

    const float multiplierHintScale = 0.75f * scaleFactor;
    const float multiplierHintWidth = 300 * scaleFactor;
    const Colour ballFollowHintColour(1, 0.8f, 0);

    double timeBetweenLabels = 4.0;
    double multiplierLostTime = 4.0;
    if (GameDisplay::IsArcadeModeEnabled()) {
        timeBetweenLabels = 2.0;
        multiplierLostTime = 2.0;
    }
    
    BasicMultiTutorialHint* multiplierHints = new BasicMultiTutorialHint(timeBetweenLabels, 1.0, 1.0);
    multiplierHints->PushHint("Destroying Blocks Increases Your Multiplier", multiplierHintScale, multiplierHintWidth);
    multiplierHints->PushHint("Higher Multipliers = More Points", multiplierHintScale, multiplierHintWidth);
    multiplierHints->SetColour(ballFollowHintColour);
    multiplierHints->SetListener(new SlowBallHintListener(this));
    this->ballFollowTutorialHints.push_back(multiplierHints);
    this->tutorialListener->SetMultiplierHints(multiplierHints);

    BasicMultiTutorialHint* multiplierLostHint = new BasicMultiTutorialHint(multiplierLostTime, 1.0, 1.0);
    multiplierLostHint->PushHint("Your Multiplier is Lost When a Ball Hits the Paddle", multiplierHintScale, multiplierHintWidth);
    multiplierLostHint->SetColour(ballFollowHintColour);
    multiplierLostHint->SetListener(new SlowBallHintListener(this));
    this->ballFollowTutorialHints.push_back(multiplierLostHint);
    this->tutorialListener->SetMultiplierLostHint(multiplierLostHint);
}

void InTutorialGameDisplayState::HintListener::OnTutorialHintShown() {
    this->state->tutorialAttentionEffect.CrossFadeIn(1.0);
}

void InTutorialGameDisplayState::HintListener::OnTutorialHintUnshown() {
    this->state->tutorialAttentionEffect.CrossFadeOut(0.5);
}

void InTutorialGameDisplayState::ShootBallHintListener::OnTutorialHintShown() {
    HintListener::OnTutorialHintShown();
    GameViewEventManager::Instance()->ActionShootBallTutorialHintShown(true);
}
void InTutorialGameDisplayState::ShootBallHintListener::OnTutorialHintUnshown() {
    HintListener::OnTutorialHintUnshown();
    GameViewEventManager::Instance()->ActionShootBallTutorialHintShown(false);
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
        this->SetBallSpeed(GameBall::GetSlowestAllowableSpeed());
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
        ball->TurnOffImpulse();
        ball->TurnOffBoost();
    }
}