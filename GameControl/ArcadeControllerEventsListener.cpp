#include "ArcadeControllerEventsListener.h"
#include "../GameView/PlayerHurtHUD.h"
#include "../GameView/GameDisplay.h"

typedef ArcadeControllerEventsListener ACEL;

ACEL::ArcadeControllerEventsListener(GameDisplay* display, ArcadeSerialComm& serialComm) : display(display),
serialComm(serialComm), defaultMarqueeColour(25.0/255.0, 25.0/255.0, 25.0/255.0) {
    this->SetMarqueeColour(this->defaultMarqueeColour, ArcadeSerialComm::InstantTransition);
}

ACEL::~ArcadeControllerEventsListener() {
}

void ACEL::PaddleHitWallEvent(const PlayerPaddle&, const Point2D&) {
    this->serialComm.SetMarqueeFlash(1.25f*this->currMarqueeColour, ArcadeSerialComm::VeryFastMarqueeFlash);
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
}

void ACEL::AllBallsDeadEvent(int) {
    if (this->display->GetModel()->IsGameOver()) {
        //this->currMarqueeColour = Colour(1,0,0);
        //this->serialComm.SetMarqueeColour(this->currMarqueeColour, ArcadeSerialComm::InstantTransition);
        // TODO: When do we put the marquee back to the default colour???
    }
}

void ACEL::BallSpawnEvent(const GameBall&) {
    // TODO: Boss levels are more complicated, we need to wait until the player is allowed to launch the ball...
    //this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::SlowButtonFlash);
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
    if (this->display->GetModel()->GetPlayerPaddle()->HasSomethingToShoot()) {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::FastButtonFlash);
    }
}

void ACEL::ItemDeactivatedEvent(const GameItem&) {
    // Check to see if the paddle can shoot anything, if it can't then we deactivate the fire button
    if (!this->display->GetModel()->GetPlayerPaddle()->HasSomethingToShoot()) {
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
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

void ACEL::RocketExplodedEvent(const RocketProjectile&) {
    this->serialComm.SetMarqueeFlash(Colour(1,1,1), ArcadeSerialComm::FastMarqueeFlash, ArcadeSerialComm::OneFlash);
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
        this->serialComm.SetButtonCadence(ArcadeSerialComm::FireButton, ArcadeSerialComm::Off);
        this->serialComm.SetButtonCadence(ArcadeSerialComm::BoostButton, ArcadeSerialComm::Off);
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

void ACEL::SetMarqueeColour(const Colour& c, ArcadeSerialComm::TransitionTimeType timeType) {
    this->currMarqueeColour = c;
    this->serialComm.SetMarqueeColour(c, timeType);
}