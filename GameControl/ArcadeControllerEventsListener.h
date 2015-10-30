/**
 * ArcadeControllerEventsListener.h
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

#ifndef __ARCADECONTROLLEREVENTSLISTENER_H__
#define __ARCADECONTROLLEREVENTSLISTENER_H__

#include "../GameModel/GameEvents.h"
#include "../GameView/GameViewEventListener.h"
#include "ArcadeSerialComm.h"

class GameDisplay;

class ArcadeControllerEventsListener : public GameEvents, public GameViewEventListener {
public:
    explicit ArcadeControllerEventsListener(GameDisplay* display, ArcadeSerialComm& serialComm);
    ~ArcadeControllerEventsListener();

    // GameModel Events
    void PaddleHitWallEvent(const PlayerPaddle& paddle, const Point2D& hitLoc);
    void PaddlePortalBlockTeleportEvent(const PlayerPaddle& paddle, const PortalBlock& enterPortal);
    void PaddleHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
    void PaddleShieldHitByProjectileEvent(const PlayerPaddle& paddle, const Projectile& projectile);
    void ProjectileDeflectedByPaddleShieldEvent(const Projectile& projectile, const PlayerPaddle& paddle);
    void PaddleHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void PaddleShieldHitByBeamEvent(const PlayerPaddle& paddle, const Beam& beam, const BeamSegment& beamSegment);
    void PaddleHitByBossEvent(const PlayerPaddle& paddle, const BossBodyPart& bossPart);
    void PaddleStatusUpdateEvent(const PlayerPaddle& paddle, PlayerPaddle::PaddleSpecialStatus status, bool isActive);
    void FrozenPaddleCanceledByFireEvent(const PlayerPaddle& paddle);
    void OnFirePaddleCanceledByIceEvent(const PlayerPaddle& paddle);
    void PaddleFlippedEvent(const PlayerPaddle& paddle, bool isUpsideDown);
    void PaddleWeaponFiredEvent(PlayerPaddle::PaddleType weaponType);

    void LastBallExplodedEvent(const GameBall& explodedBall, bool wasSkipped);
    void AllBallsDeadEvent(int livesLeft);
    void BallSpawnEvent(const GameBall& spawnedBall);
    void BallShotEvent(const GameBall& shotBall);
    void BallPaddleCollisionEvent(const GameBall& ball, const PlayerPaddle& paddle, bool hitPaddleUnderside);

    void ItemActivatedEvent(const GameItem& item);
    void ItemDeactivatedEvent(const GameItem& item);
    void ItemPaddleCollsionEvent(const GameItem& item, const PlayerPaddle& paddle);

    void BallBoostGainedEvent();
    void BallBoostLostEvent(bool allBoostsLost);
    void BulletTimeStateChangedEvent(const BallBoostModel& boostModel);

    void RocketExplodedEvent(const RocketProjectile& rocket);


    // GameView Events
    void GameModelUpdated();
    void DisplayStateChanged(const DisplayState::DisplayStateType& prevState, const DisplayState::DisplayStateType& newState);
    void BiffBamBlammoSlamEvent(const GameViewEventListener::SlamType& slamType);
    void ArcadeWaitingForPlayerState(bool entered);
    void ArcadePlayerHitStartGame();
    void ArcadePlayerSelectedWorld();
    void ArcadePlayerHitContinue();
    void ArcadePlayerHitSummaryConfirm();
    void ShootBallTutorialHintShown(bool shown);

private:
    GameDisplay* display; // NOT OWNED BY THIS

    const Colour defaultMarqueeColour;
    Colour currMarqueeColour;

    ArcadeSerialComm& serialComm;

    static const unsigned long MARQUEE_COLOUR_MAX_NO_UPDATE_TIME_MS;
    unsigned long timeOfLastMarqueeColourUpdate;

    void SetMarqueeColour(const Colour& c, ArcadeSerialComm::TransitionTimeType timeType);

    DISALLOW_COPY_AND_ASSIGN(ArcadeControllerEventsListener);
};



#endif // __ARCADECONTROLLEREVENTSLISTENER_H__