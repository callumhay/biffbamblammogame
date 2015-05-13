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

    void RocketExplodedEvent(const RocketProjectile& rocket);


    // GameView Events
    void DisplayStateChanged(const DisplayState::DisplayStateType& prevState, const DisplayState::DisplayStateType& newState);
    void BiffBamBlammoSlamEvent(const GameViewEventListener::SlamType& slamType);

private:
    GameDisplay* display; // NOT OWNED BY THIS

    const Colour defaultMarqueeColour;
    Colour currMarqueeColour;

    ArcadeSerialComm& serialComm;

    void SetMarqueeColour(const Colour& c, ArcadeSerialComm::TransitionTimeType timeType);

    DISALLOW_COPY_AND_ASSIGN(ArcadeControllerEventsListener);
};



#endif // __ARCADECONTROLLEREVENTSLISTENER_H__