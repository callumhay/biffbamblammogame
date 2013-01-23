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

InGameBossLevelDisplayState::InGameBossLevelDisplayState(GameDisplay* display) :
InGameDisplayState(display) {

    // Don't allow the player to launch the ball just yet, the boss has a dramatic intro that
    // needs to be animated first...
    this->display->GetModel()->ToggleAllowPaddleBallLaunching(false);
	this->display->GetCamera().ClearCameraShake();


	debug_opengl_state();

    this->SetBossState(InGameBossLevelDisplayState::IntroBossState);
}

InGameBossLevelDisplayState::~InGameBossLevelDisplayState() {
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
        
        case InGameBossLevelDisplayState::FadeInBossState:
            this->fadeInCountdown = this->display->GetAssets()->ToggleLights(true);
            break;

        case InGameBossLevelDisplayState::InPlayBossState:
            // Make sure that everything in the game is unpaused and
            // that the player can now launch the ball from the paddle
            this->display->GetModel()->SetPauseState(GameModel::NoPause);
            this->display->GetModel()->ToggleAllowPaddleBallLaunching(true);
            break;

        case InGameBossLevelDisplayState::VictoryBossState:
            break;
        
        default:
            assert(false);
            return;
    }

    this->currBossState = newState;
}

void InGameBossLevelDisplayState::RenderBossState(double dT) {
    this->renderPipeline.RenderFrame(dT);

    switch (this->currBossState) {
        
        case InGameBossLevelDisplayState::IntroBossState:
            this->ExecuteIntroBossState(dT);
            break;
        
        case InGameBossLevelDisplayState::FadeInBossState:
            this->ExecuteFadeInBossState(dT);
            break;

        case InGameBossLevelDisplayState::InPlayBossState:
            // Basic render state, no extraneous animations
            break;

        case InGameBossLevelDisplayState::VictoryBossState:
            break;
        
        default:
            assert(false);
            break;
    }

}

void InGameBossLevelDisplayState::ExecuteIntroBossState(double dT) {
    if (this->introCountdown <= 0.0) {
        this->SetBossState(InGameBossLevelDisplayState::FadeInBossState);
    }
    else {
        this->introCountdown -= dT;
    }
}

void InGameBossLevelDisplayState::ExecuteFadeInBossState(double dT) {
    if (this->fadeInCountdown <= 0.0) {
        this->SetBossState(InGameBossLevelDisplayState::InPlayBossState);
    }
    else {
        this->fadeInCountdown -= dT;
    }
}