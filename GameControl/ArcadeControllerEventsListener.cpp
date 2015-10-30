/**
 * ArcadeControllerEventsListener.cpp
 * 
 * Copyright (c) 2015, Callum Hay
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

#include "ArcadeControllerEventsListener.h"
#include "../GameView/PlayerHurtHUD.h"
#include "../GameView/GameDisplay.h"
#include "../GameView/GameAssets.h"
#include "../GameView/GameFBOAssets.h"

typedef ArcadeControllerEventsListener ACEL;

const unsigned long ACEL::MARQUEE_COLOUR_MAX_NO_UPDATE_TIME_MS = 5000;

ACEL::ArcadeControllerEventsListener(GameDisplay* display, ArcadeSerialComm& serialComm) : display(display),
serialComm(serialComm), defaultMarqueeColour(128.0/255.0, 128.0/255.0, 128.0/255.0), 
timeOfLastMarqueeColourUpdate(0) {

    this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
    this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
}

ACEL::~ArcadeControllerEventsListener() {
    this->SetMarqueeColour(Colour(0,0,0), ArcadeSerialComm::InstantTransition);
    this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
}

void ACEL::PaddleHitWallEvent(const PlayerPaddle&, const Point2D&) {
    this->serialComm.SetMarqueeFlash(2.0f*this->currMarqueeColour, ArcadeSerialComm::VeryFastMarqueeFlash);
}

void ACEL::PaddlePortalBlockTeleportEvent(const PlayerPaddle&, const PortalBlock& enterPortal) {
    this->serialComm.SetMarqueeFlash(enterPortal.GetColour(), ArcadeSerialComm::VeryFastMarqueeFlash);
}

void ACEL::PaddleHitByProjectileEvent(const PlayerPaddle&, const Projectile& projectile) {
    PlayerHurtHUD::PainIntensity intensity = PlayerHurtHUD::GetIntensityForProjectile(projectile);
    ArcadeSerialComm::MarqueeFlashType flashType = ArcadeSerialComm::VeryFastMarqueeFlash;
    switch (intensity) {
        case PlayerHurtHUD::NoPain:
            return;
        case PlayerHurtHUD::MinorPain:
            flashType = ArcadeSerialComm::VeryFastMarqueeFlash;
            break;
        case PlayerHurtHUD::ModeratePain:
            flashType = ArcadeSerialComm::FastMarqueeFlash;
            break;
        case PlayerHurtHUD::MoreThanModeratePain:
            flashType = ArcadeSerialComm::MediumMarqueeFlash;
            break;
        case PlayerHurtHUD::MajorPain:
            flashType = ArcadeSerialComm::SlowMarqueeFlash;
            break;
        default:
            assert(false);
            return;
    }

    this->serialComm.SetMarqueeFlash(Colour(0.75f,0.0f,0.0f), flashType);
}
void ACEL::PaddleShieldHitByProjectileEvent(const PlayerPaddle&, const Projectile&) {}
void ACEL::ProjectileDeflectedByPaddleShieldEvent(const Projectile&, const PlayerPaddle&) {}

void ACEL::PaddleHitByBeamEvent(const PlayerPaddle&, const Beam&, const BeamSegment&) {
    this->serialComm.SetMarqueeFlash(Colour(1,0,0), ArcadeSerialComm::VerySlowMarqueeFlash);
}

void ACEL::PaddleShieldHitByBeamEvent(const PlayerPaddle&, const Beam&, const BeamSegment&) {}

void ACEL::PaddleHitByBossEvent(const PlayerPaddle&, const BossBodyPart&) {
    this->serialComm.SetMarqueeFlash(Colour(1,0,0), ArcadeSerialComm::VerySlowMarqueeFlash);
}

void ACEL::PaddleStatusUpdateEvent(const PlayerPaddle&, PlayerPaddle::PaddleSpecialStatus status, bool isActive) {
    switch (status) {
        case PlayerPaddle::FrozenInIceStatus:
            if (isActive) {
                this->SetMarqueeColour(Colour(0.9f,0.9f,1.0f), ArcadeSerialComm::InstantTransition);
            }
            else {
                this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
            }
            break;

        case PlayerPaddle::ElectrocutedStatus:
            if (isActive) {
                this->serialComm.SetMarqueeFlash(Colour(1,1,0), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::ThreeFlashes);
            }
            else {
                this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
            }
            break;

        case PlayerPaddle::OnFireStatus:
            if (isActive) {
                this->serialComm.SetMarqueeFlash(Colour(1,0.1f,0), ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::TwoFlashes);
            }
            else {
                this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
            }
            break;

        default:
            return;
    }
}
void ACEL::FrozenPaddleCanceledByFireEvent(const PlayerPaddle&) {
    this->serialComm.SetMarqueeFlash(Colour(0.5f,0.5f,0.5f), 
        ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash);
}
void ACEL::OnFirePaddleCanceledByIceEvent(const PlayerPaddle&) {
    this->serialComm.SetMarqueeFlash(Colour(0.5f,0.5f,0.5f), 
        ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash);
}
void ACEL::PaddleFlippedEvent(const PlayerPaddle&, bool) {}

void ACEL::PaddleWeaponFiredEvent(PlayerPaddle::PaddleType) {
    if (!this->display->GetModel()->GetPlayerPaddle()->HasSomethingToShoot()) {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
    }
}

void ACEL::LastBallExplodedEvent(const GameBall&, bool wasSkipped) {
    if (!wasSkipped) {
        this->serialComm.SetMarqueeFlash(Colour(1,0,0), ArcadeSerialComm::MediumMarqueeFlash, 
            ArcadeSerialComm::ThreeFlashes, true);
    }
    this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
}

void ACEL::AllBallsDeadEvent(int) {
    this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
    this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);

    if (this->display->GetModel()->IsGameOver()) {
        //this->currMarqueeColour = Colour(1,0,0);
        //this->serialComm.SetMarqueeColour(this->currMarqueeColour, ArcadeSerialComm::InstantTransition);
        // TODO: When do we put the marquee back to the default colour???
    }
}

void ACEL::BallSpawnEvent(const GameBall&) {
    // TODO: Boss levels are more complicated, we need to wait until the player is allowed to launch the ball...
    //this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::SlowButtonFlash);
    this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
    this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
}

void ACEL::BallShotEvent(const GameBall&) {
    if (!this->display->GetModel()->GetPlayerPaddle()->HasSomethingToShoot()) {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
    }
}

void ACEL::BallPaddleCollisionEvent(const GameBall&, const PlayerPaddle&, bool) {
    this->serialComm.SetMarqueeFlash(1.15f*this->currMarqueeColour, ArcadeSerialComm::VeryFastMarqueeFlash);
}

void ACEL::ItemActivatedEvent(const GameItem&) {
    // Check to see if the paddle can shoot anything, if it can then we flash the fire button
    GameModel* model = this->display->GetModel();
    // Check to see if the paddle can shoot anything, if it can't then we deactivate the fire button
    if (GameState::IsGameInPlayState(*model)) {
        PlayerPaddle* paddle = model->GetPlayerPaddle();
        if (paddle != NULL && model->GetPlayerPaddle()->HasSomethingToShoot()) {
            this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::MediumButtonFlash);
        }
    }
}

void ACEL::ItemDeactivatedEvent(const GameItem&) {
    GameModel* model = this->display->GetModel();
    // Check to see if the paddle can shoot anything, if it can't then we deactivate the fire button
    if (GameState::IsGameInPlayState(*model)) {
        PlayerPaddle* paddle = model->GetPlayerPaddle();
        if (paddle != NULL && !model->GetPlayerPaddle()->HasSomethingToShoot()) {
            this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
        }
    }
}

void ACEL::ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle&) {
    this->serialComm.SetMarqueeFlash(
        GameViewConstants::GetInstance()->GetItemColourFromDisposition(item.GetItemDisposition()), 
        ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash);
}

void ACEL::BallBoostGainedEvent() {
    // Start flashing the boost button
    this->serialComm.SetButtonCadence(ArcadeSerialComm::BoostButton, ArcadeSerialComm::MediumButtonFlash);
}

void ACEL::BallBoostLostEvent(bool allBoostsLost) {
    if (allBoostsLost) {
        // Stop flashing the boost button
        this->serialComm.SetButtonCadence(ArcadeSerialComm::BoostButton, ArcadeSerialComm::Off);
    }
}

void ACEL::BulletTimeStateChangedEvent(const BallBoostModel& boostModel) {
    static ArcadeSerialComm::ButtonGlowCadenceType beforeBoostFireBtnCadence = this->serialComm.GetCurrentFireButtonCadence();

    switch (boostModel.GetBulletTimeState()) {
        case BallBoostModel::NotInBulletTime:
            beforeBoostFireBtnCadence = this->serialComm.GetCurrentFireButtonCadence();

            // Check to see if there's still a boost available and turn the boost button on if there is...
            if (boostModel.IsBoostAvailable()) {
                this->serialComm.SetButtonCadence(ArcadeSerialComm::BoostButton, ArcadeSerialComm::MediumButtonFlash);
            }
            break;

        case BallBoostModel::BulletTimeFadeIn:
            // Turn off the boost button
            this->serialComm.SetButtonCadence(ArcadeSerialComm::BoostButton, ArcadeSerialComm::Off);
        case BallBoostModel::BulletTime:
            // Make sure the fire button is flashing
            if (this->serialComm.GetCurrentFireButtonCadence() == ArcadeSerialComm::Off) {
                this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::FastButtonFlash);
            }
            break;

        case BallBoostModel::BulletTimeFadeOut:
            if (!this->display->GetModel()->GetPlayerPaddle()->HasSomethingToShoot()) {
                beforeBoostFireBtnCadence = ArcadeSerialComm::Off;
            }
            this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, beforeBoostFireBtnCadence);

            break;

        default:
            break;
    }
}

void ACEL::RocketExplodedEvent(const RocketProjectile&) {
    this->serialComm.SetMarqueeFlash(Colour(1,1,1), ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash);
}

// This is called EVERY FRAME so be quick!
void ACEL::GameModelUpdated() {
    const GameModel* model = this->display->GetModel();
    if (!GameState::IsGameInPlayState(*model)) {
        if (this->currMarqueeColour != this->defaultMarqueeColour) {
            this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
        }
        return;
    }

    // We're in a game play state... check a bunch of stuff to determine the current marquee colour...
    Colour newMarqueeColour = this->defaultMarqueeColour;
    ArcadeSerialComm::TransitionTimeType transitionType = ArcadeSerialComm::InstantTransition;

#define DO_MARQUEE_COLOUR_BLEND_MULT(marqueeColour, blendColour) if (marqueeColour == this->defaultMarqueeColour) { marqueeColour = blendColour; } else { marqueeColour *= blendColour; } 

    if (model->IsBlackoutEffectActive()) {
        newMarqueeColour = Colour(0,0,0);
    }
    else {
        const PlayerPaddle* paddle = model->GetPlayerPaddle();
        assert(paddle != NULL);

        // Poison paddle
        if (paddle->HasPaddleType(PlayerPaddle::PoisonPaddle)) {

            DO_MARQUEE_COLOUR_BLEND_MULT(newMarqueeColour, GameViewConstants::GetInstance()->POISON_LIGHT_LIGHT_COLOUR);

            transitionType = ArcadeSerialComm::MediumTransition;
            this->serialComm.SetMarqueeFlash(Colour(0.0f, 0.3f + Randomizer::GetInstance()->RandomNumZeroToOne()*0.7f, 
                Randomizer::GetInstance()->RandomNumZeroToOne()*0.2f),
                ArcadeSerialComm::RandomMarqueeFlashType(), ArcadeSerialComm::RandomNumMarqueeFlashes(), false);
        }

        // Is the ink splatter active?
        const GameFBOAssets* fboAssets = this->display->GetAssets()->GetFBOAssets();
        if (fboAssets->IsInkSplatterEffectActive()) {
            DO_MARQUEE_COLOUR_BLEND_MULT(newMarqueeColour, GameViewConstants::GetInstance()->INK_BLOCK_COLOUR);
        }
    }

#undef DO_MARQUEE_COLOUR_BLEND_MULT
    
    unsigned long currTime = BlammoTime::GetSystemTimeInMillisecs();
    if (newMarqueeColour != this->currMarqueeColour || (currTime - this->timeOfLastMarqueeColourUpdate) > MARQUEE_COLOUR_MAX_NO_UPDATE_TIME_MS) {
        this->SetMarqueeColour(newMarqueeColour, transitionType);
        this->timeOfLastMarqueeColourUpdate = currTime;
    }
}

void ACEL::DisplayStateChanged(const DisplayState::DisplayStateType& prevState, const DisplayState::DisplayStateType& newState) {

    // Ignore the pause menu... the player shouldn't have access to it in arcade mode,
    // someone must be debugging or something
    if (newState == DisplayState::InGameMenu) {
        return;
    }

    // Check to see if the player beat the game...
    // TODO

    bool wasInGameplayState = DisplayState::IsGameInPlayDisplayState(prevState);
    if (wasInGameplayState && !DisplayState::IsGameInPlayDisplayState(newState)) {
        // Stop flashing all buttons, the player is no longer in a game play state
        this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
    }

    // If we just went from an in-play state to a game over state then we make the marquee red
    if (newState == DisplayState::GameOver) {
        this->SetMarqueeColour(Colour(1,0,0), ArcadeSerialComm::InstantTransition);
    }
    else {
        // Otherwise return the marquee to it's default colour
        this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
    }
}

void ACEL::BiffBamBlammoSlamEvent(const GameViewEventListener::SlamType& slamType) {
    switch (slamType) {
        case GameViewEventListener::BiffSlam:
            this->serialComm.SetMarqueeFlash(Colour(1, 0.5f, 0), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
            break;
        case GameViewEventListener::BamSlam:
            this->serialComm.SetMarqueeFlash(Colour(1, 0.75f, 0), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
            break;
        case GameViewEventListener::BlammoSlam:
            this->serialComm.SetMarqueeFlash(Colour(1, 1, 0), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
            break;

        default:
            break;
    }
}

void ACEL::ArcadeWaitingForPlayerState(bool entered) {
    if (entered) {
        // In the main menu we flash the buttons
        this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::VerySlowButtonFlash);
        this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::VerySlowButtonFlash);
    }
    else {
        // Stop flashing the buttons when leaving the main menu
        this->serialComm.SetButtonCadence(ArcadeSerialComm::AllButtons, ArcadeSerialComm::Off);
    }
}

void ACEL::ArcadePlayerHitStartGame() {
    this->serialComm.SetMarqueeFlash(Colour(1, 1, 1), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
}

void ACEL::ArcadePlayerSelectedWorld() {
    this->serialComm.SetMarqueeFlash(Colour(1, 1, 1), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::ThreeFlashes, true);
}

void ACEL::ArcadePlayerHitContinue() {
    this->serialComm.SetMarqueeFlash(Colour(1, 1, 1), ArcadeSerialComm::VeryFastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
}

void ACEL::ArcadePlayerHitSummaryConfirm() {
    this->serialComm.SetMarqueeFlash(Colour(1, 1, 1), ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash, true);
}

void ACEL::ShootBallTutorialHintShown(bool shown) {
    if (shown) {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::MediumButtonFlash);
    }
    else {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
    }
}

void ACEL::SetMarqueeColour(const Colour& c, ArcadeSerialComm::TransitionTimeType timeType) {
    this->currMarqueeColour = c;
    this->serialComm.SetMarqueeColour(c, timeType);
}