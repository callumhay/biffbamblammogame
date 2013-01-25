/**
 * InGameBossLevelDisplayState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "InGameBossLevelDisplayState.h"
#include "GameDisplay.h"
#include "GameAssets.h"

#include "../GameModel/Boss.h"

// The time that the game (boss and paddle) are unpaused right before the intro ends
const double InGameBossLevelDisplayState::TIME_OF_UNPAUSE_BEFORE_INTRO_END = 0.85;

// Time for background to turn black leaving only the boss and the paddle when the boss
// is dying in the outro state
const double InGameBossLevelDisplayState::FADE_TO_BLACK_TIME_IN_SECS = Boss::WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME / 1.25;

// We wait a brief amount of time at the end of the outro (for dramatic pause)
const double InGameBossLevelDisplayState::WAIT_TIME_AT_END_OF_OUTRO_IN_SECS = 1.0;

InGameBossLevelDisplayState::InGameBossLevelDisplayState(GameDisplay* display) :
InGameDisplayState(display) {

    // Don't allow the player to launch the ball just yet, the boss has a dramatic intro that
    // needs to be animated first...
    this->display->GetModel()->ToggleAllowPaddleBallLaunching(false);
	this->display->GetCamera().ClearCameraShake();

    this->fadeObjectsAnim.SetLerp(0.0, FADE_TO_BLACK_TIME_IN_SECS, 1.0f, 0.0f);

	debug_opengl_state();

    this->SetBossState(InGameBossLevelDisplayState::IntroBossState);
}

InGameBossLevelDisplayState::~InGameBossLevelDisplayState() {
    this->display->GetAssets()->ToggleLightsForBossDeath(true, 0.01);
}

void InGameBossLevelDisplayState::RenderFrame(double dT) {
    this->RenderBossState(dT);

#ifdef _DEBUG
	this->DebugDrawBounds();
	this->display->GetAssets()->DebugDrawLights();
#endif
}

void InGameBossLevelDisplayState::SetBossState(BossState newState) {
    switch (newState) {
        
        case InGameBossLevelDisplayState::IntroBossState:
            // Make sure the boss is paused
            this->display->GetModel()->SetPauseState(GameModel::PauseAI);
            this->introCountdown = this->display->GetAssets()->ActivateBossIntro();
            break;
        
        case InGameBossLevelDisplayState::FadeInBossState: {
            // Turn on the lights!
            static const double LIGHT_ON_TIME = 1.0;
            this->fadeInCountdown = LIGHT_ON_TIME;
            this->display->GetAssets()->ToggleLights(true, LIGHT_ON_TIME);
            break;
        }

        case InGameBossLevelDisplayState::InPlayBossState:
            // Make sure that everything in the game is unpaused for play
            this->display->GetModel()->SetPauseState(GameModel::NoPause);
            this->display->GetModel()->ToggleAllowPaddleBallLaunching(true);
            break;

        case InGameBossLevelDisplayState::OutroBossState: {

            // Cancel all effects...
            this->display->GetAssets()->GetItemAssets()->ClearTimers();
            this->display->GetAssets()->GetESPAssets()->KillAllActiveEffects(false);
            this->display->GetAssets()->DeactivateMiscEffects();

		    // Stop world background music (if it's still going)
		    this->display->GetAssets()->GetSoundAssets()->StopWorldSound(GameSoundAssets::WorldBackgroundMusic);
            
            // The outro consists of 'explosive' white lines emitting from the boss
            // and then an animation to the whole screen going white...
            this->outroFinishCountdown =
                this->display->GetAssets()->ActivateBossExplodingFlashEffects(
                Boss::WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME, this->display->GetModel());
            this->outroFinishCountdown += WAIT_TIME_AT_END_OF_OUTRO_IN_SECS;
            
            this->display->GetAssets()->ToggleLightsForBossDeath(false, FADE_TO_BLACK_TIME_IN_SECS);

            this->fadeObjectsAnim.ResetToStart();

            PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
            float levelWidthTimes2 = 2*this->display->GetModel()->GetCurrentLevel()->GetLevelUnitWidth();
            paddle->UpdatePaddleBounds(-levelWidthTimes2, levelWidthTimes2);
            this->paddlePosGetTheHellOutAnim.SetLerp(0.0, Boss::TOTAL_DEATH_ANIM_TIME, 
                paddle->GetCenterPosition()[0], levelWidthTimes2);

            break;
        }

        case InGameBossLevelDisplayState::VictoryBossState:
            break;
        
        default:
            assert(false);
            return;
    }

    this->currBossState = newState;
}

void InGameBossLevelDisplayState::RenderBossState(double dT) {

    switch (this->currBossState) {
        
        case InGameBossLevelDisplayState::IntroBossState:
            this->ExecuteIntroBossState(dT);
            break;
        
        case InGameBossLevelDisplayState::FadeInBossState:
            this->ExecuteFadeInBossState(dT);
            break;

        case InGameBossLevelDisplayState::InPlayBossState:
            // Basic render state, no extraneous animations
            this->ExecuteInPlayBossState(dT);
            break;

        case InGameBossLevelDisplayState::OutroBossState:
            this->ExecuteOutroBossState(dT);
            break;

        case InGameBossLevelDisplayState::VictoryBossState:
            this->ExecuteVictoryBossState(dT);
            break;
        
        default:
            assert(false);
            break;
    }

}

void InGameBossLevelDisplayState::ExecuteIntroBossState(double dT) {
    this->renderPipeline.RenderFrame(dT);

    if (this->introCountdown <= 0.0) {
        this->SetBossState(InGameBossLevelDisplayState::FadeInBossState);
    }
    else {
        if (this->introCountdown <= TIME_OF_UNPAUSE_BEFORE_INTRO_END) {

            // Make sure that everything in the game is unpaused and
            // that the player can now launch the ball from the paddle
            this->display->GetModel()->SetPauseState(GameModel::NoPause);
            this->display->GetModel()->ToggleAllowPaddleBallLaunching(true);
        }
        this->introCountdown -= dT;
    }
}

void InGameBossLevelDisplayState::ExecuteFadeInBossState(double dT) {
    this->renderPipeline.RenderFrame(dT);

    if (this->fadeInCountdown <= 0.0) {
        this->SetBossState(InGameBossLevelDisplayState::InPlayBossState);
    }
    else {
        this->fadeInCountdown -= dT;
    }
}

void InGameBossLevelDisplayState::ExecuteInPlayBossState(double dT) {

    this->renderPipeline.RenderFrame(dT);

    const GameModel* model = this->display->GetModel();
    const GameLevel* level = model->GetCurrentLevel();
    const Boss* boss = level->GetBoss();
    assert(boss != NULL);

    if (boss->GetIsStateMachineFinished()) {
        // Pause the paddle and ball, the boss is defeated!
        this->display->GetModel()->SetPauseState(GameModel::PausePaddleControls | GameModel::PauseBall);
        this->SetBossState(InGameBossLevelDisplayState::OutroBossState);
    }
}

void InGameBossLevelDisplayState::ExecuteOutroBossState(double dT) {
    this->fadeObjectsAnim.Tick(dT);
    this->paddlePosGetTheHellOutAnim.Tick(dT);

    float unimportantObjectsAlpha = this->fadeObjectsAnim.GetInterpolantValue();
    float paddleXPos = this->paddlePosGetTheHellOutAnim.GetInterpolantValue();

    GameBall* ball  = this->display->GetModel()->GetGameBalls().front();
    PlayerPaddle* paddle = this->display->GetModel()->GetPlayerPaddle();
    GameAssets* gameAssets = this->display->GetAssets();

    paddle->SetCenterPosition(Point2D(paddleXPos, paddle->GetCenterPosition()[1]));

    ball->SetAlpha(unimportantObjectsAlpha);
	gameAssets->GetCurrentLevelMesh()->SetLevelAlpha(unimportantObjectsAlpha);
	gameAssets->GetESPAssets()->SetTeslaLightiningAlpha(unimportantObjectsAlpha);

    this->renderPipeline.RenderFrameWithoutHUD(dT);
    this->renderPipeline.RenderHUDWithAlpha(dT, unimportantObjectsAlpha);

    if (this->outroFinishCountdown <= 0.0) {
        //this->SetBossState(InGameBossLevelDisplayState::VictoryBossState);
    }
    else {
        this->outroFinishCountdown -= dT;
    }
}

void InGameBossLevelDisplayState::ExecuteVictoryBossState(double dT) {
    const GameModel* model = this->display->GetModel();
    const GameLevel* level = model->GetCurrentLevel();
    Boss* boss = level->GetBoss();
    assert(boss != NULL);
    boss->SetIsLevelCompleteDead(true);
}