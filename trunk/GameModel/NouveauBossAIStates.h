/**
 * NouveauBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __NOUVEAUBOSSAISTATES_H__
#define __NOUVEAUBOSSAISTATES_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

#include "../GameSound/SoundCommon.h"

#include "BossAIState.h"
#include "GameLevel.h"
#include "PrismBlock.h"

class NouveauBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace nouveaubossai {

/**
 * Abstract superclass for all of the NouveauBoss AI state machine classes.
 */
class NouveauBossAI : public BossAIState {
public:
    NouveauBossAI(NouveauBoss* boss);
    virtual ~NouveauBossAI();

    // Inherited functions
    Boss* GetBoss() const;
    bool CanHurtPaddleWithBody() const { return false; }
    virtual bool IsStateMachineFinished() const { return false; }
    Collision::AABB2D GenerateDyingAABB() const;
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {}; // There aren't any boss-paddle collisions in this battle

protected:
    NouveauBoss* boss;
    AnimationMultiLerp<Vector3D> angryMoveAnim;

    // State-related variables
    enum AIState { MoveToTargetStopAndShootAIState, ArcSprayFireAIState, RapidFireSweepAIState, 
                   MoveToLaserTargetableLocationAIState, PrepLaserBeamAttackAIState, LaserBeamAttackAIState,
                   BrutalMoveAndShootAIState, LostLeftArmAIState, LostRightArmAIState, BothArmsLostAIState, HurtTopAIState,
                   TopLostAIState, FinalDeathThroesAIState };
    AIState currState;

    // State-specific variables
    
    // MoveToTargetStopAndShootAIState
    int numLinearMovements;          // The number of linear movements the boss will make before going to the next state
    double waitingAtTargetCountdown; // Time to wait at the current targetPosition before choosing a new one and moving to it
    double timeUntilNextLaserWhileWaitingAtTarget; // Time until the boss fires the next laser while waiting at it's target
    
    // RapidFireSweepAIState
    double rapidFireSweepLaserFireCountdown; // Time to wait between each laser that fires in rapid succession
    Vector2D currRapidFireShootDir;          // Used to track the current direction of the lasers being fired
    double totalRapidFireAngleChange;        // Used to track the total angle of change that has occurred 
                                             // to currRapidFireShootDir so we know when to stop
    enum SweepDirection { LeftToRightDir = 0, RightToLeftDir, NumSweepDirections};
    SweepDirection rapidFireSweepDir; // The direction that the lasers move in when being fired in rapid succession

    // PrepLaserBeamAttackAIState, LaserBeamAttackAIState
    enum LaserBeamFireLocation { LeftSideTop = 0, LeftSideBottom, CenterBottom, RightSideTop, RightSideBottom, NumLaserBeamFiringLocations };
    std::list<LaserBeamFireLocation> firingLocations;
    double timeBetweenLaserBeamPrepsCountdown;
    std::list<std::pair<LaserBeamFireLocation, Collision::Ray2D> > laserBeamRaysToFire;
    double timeBetweenLastPrepAndFireCountdown;
    double countdownToNextLaserBeamFiring;
    double countdownWaitForLastBeamToFinish;

    std::list<SoundID> laserBeamTargetingSoundIDs;

    virtual void SetState(NouveauBossAI::AIState newState) = 0;

    // MoveToTargetStopAndShootAIState template methods
    virtual int GenerateNumMovements() const = 0;
    virtual double GenerateWaitAtTargetTime() const = 0;
    virtual double GenerateTimeBetweenLasersWhileWaitingAtTarget() const = 0;
    virtual float GetMaxMoveSpeedBetweenTargets() const = 0;

    // RapidFireSweepAIState template methods
    virtual double GenerateTimeBetweenRapidFireSweepLasers() const = 0;
    virtual double GetRapidFireAngleSpeedInDegsPerSec() const = 0;

    // PrepLaserBeamAttackAIState, LaserBeamAttackAIState template methods
    virtual int GenerateNumLaserBeamsToFire() const { return 1; }
    virtual double GetTimeBetweenLaserBeamPreps() const { return 0.75; } // This must be constant
    virtual double GenerateTimeBetweenLaserBeamLastPrepAndFire() const { return 1.5; }
    virtual double GetTimeBetweenLaserBeamFirings() const { return 0.25f; } // This must be constant
    
    virtual void GoToNextRandomAttackState() = 0;

    void ExecuteLaserArcSpray(const BossBodyPart* shootingBodyPart, const Point2D& originPos, GameModel* gameModel);

    Point2D ChooseBossPositionForPlayerToHitDomeWithLasers(bool inMiddleXOfLevel) const;
    Point2D ChooseTargetPosition(const Point2D& startPos) const;
    void ShootRandomLaserBullet(GameModel* gameModel);
    void ShootRandomBottomSphereLaserBullet(GameModel* gameModel);
    void ShootRandomSideLaserBullet(GameModel* gameModel);
    void ShootRandomLeftSideLaserBullet(GameModel* gameModel);
    void ShootRandomRightSideLaserBullet(GameModel* gameModel);

    static bool IsWorthwhileShotAtSplitterPrism(const Vector2D& shootDir) {
        return Trig::radiansToDegrees(acos(std::max<float>(-1.0f, std::min<float>(1.0f, 
            Vector2D::Dot(shootDir, Vector2D(0,-1)))))) <= PrismBlock::REFLECTION_REFRACTION_SPLIT_ANGLE;
    }

    void BossWasHurt();

    void OnSetStateMoveToTargetStopAndShoot();
    void OnSetRapidFireSweep();
    void OnSetMoveToLaserTargetableLocation();
    void OnSetStatePrepLaserBeamAttack();
    void OnSetStateLaserBeamAttack();

    // State-specific methods
    void ExecuteMoveToTargetStopAndShootState(double dT, GameModel* gameModel);
    void ExecuteRapidFireSweepState(double dT, GameModel* gameModel);
    void ExecuteMoveToLaserTargetableLocationState();
    void ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel);
    void ExecuteLaserBeamAttackState(double dT, GameModel* gameModel);

private:
    DISALLOW_COPY_AND_ASSIGN(NouveauBossAI);
};


class SideSphereAI : public NouveauBossAI {
public:
    SideSphereAI(NouveauBoss* boss);
    ~SideSphereAI();

    // Inherited from NouveauBossAI/BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine);

private:
    static const float SIDE_SPHERE_LIFE_POINTS;

    static const float MAX_MOVE_SPEED;
    static const float DEFAULT_ACCELERATION;
    static const float RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND;

    static const double SIDE_FRILL_FADEOUT_TIME;
    static const double ARM_FADE_TIME;

    // The composite parts that hold the weak points in this state, 
    // they are destroyed when the weak points are destroyed
    BossCompositeBodyPart* leftSideSphereHoldingArm;
    BossCompositeBodyPart* rightSideSphereHoldingArm;

    // Spheres that take damage, once exposed
    BossWeakpoint* leftSideSphereWeakpt;
    BossWeakpoint* rightSideSphereWeakpt;

    // Frills that protect the sphere from damage until they're blown up by a mine
    BossBodyPart* leftSideSphereFrill;
    BossBodyPart* rightSideSphereFrill;
    
    BossBodyPart* leftSideSphereHolderCurl;
    BossBodyPart* rightSideSphereHolderCurl;

    // State-specific variables
    AnimationMultiLerp<Vector3D> lostLeftArmAnim;
    AnimationMultiLerp<Vector3D> lostRightArmAnim;
    double arcLaserSprayFireCountdown;

    int GenerateNumLaserBeamsToFire() const { return 1; }
    double GetTimeBetweenLaserBeamPreps() const { return 0.5; } // This must be constant
    double GenerateTimeBetweenLaserBeamLastPrepAndFire() const { return 2.5; }
    double GetTimeBetweenLaserBeamFirings() const { return 1.0f; } // This must be constant

    double GenerateTimeBetweenArcSprayLasers() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.33) + 0.75; }
    double GenerateTimeBetweenRapidFireSweepLasers() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.2) + 0.09; }
    double GetRapidFireAngleSpeedInDegsPerSec() const { return RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND; }

    void KillLeftArm();
    void KillRightArm();

    float GetTotalLifePercent() const;

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 8) + 4; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 4.0) + 1.0; }
    double GenerateTimeBetweenLasersWhileWaitingAtTarget() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.3) + 0.33; }
    float GetMaxMoveSpeedBetweenTargets() const { return MAX_MOVE_SPEED; }
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }

    void GoToNextRandomAttackState();

    void CheckForBothArmsDeadAndChangeState();

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    // State-specific methods
    void ExecuteArcSprayFireState(double dT, GameModel* gameModel);
    void ExecuteLostLeftArmState(double dT);
    void ExecuteLostRightArmState(double dT);
    void ExecuteBothArmsLostState(double dT);

    AnimationMultiLerp<Vector3D> GenerateArmDeathTranslationAnimation(bool isLeftArm) const;
    AnimationMultiLerp<float> GenerateArmDeathRotationAnimation(bool isLeftArm) const;

    DISALLOW_COPY_AND_ASSIGN(SideSphereAI);
};


class GlassDomeAI : public NouveauBossAI {
public:
    GlassDomeAI(NouveauBoss* boss);
    ~GlassDomeAI();

    // Inherited from NouveauBossAI/BossAIState
    void CollisionOccurred(GameModel*, GameBall&, BossBodyPart*) {}
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void MineExplosionOccurred(GameModel*, const MineProjectile*) {}

private:
    static const double INVULNERABLE_TIME_IN_SECS;
    static const float TOP_SPHERE_LIFE_POINTS;
    static const float MAX_MOVE_SPEED;
    static const float DEFAULT_ACCELERATION;
    static const float RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND;

    static const float TOP_DEATH_FADE_TIME;

    BossBodyPart* topDome;
    BossBodyPart* gazebo;
    BossWeakpoint* topSphereWeakpt;
    AnimationMultiLerp<Vector3D> topHurtAnim;

    float GetTotalLifePercent() const;

    int GenerateNumLaserBeamsToFire() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 2) + ((this->GetTotalLifePercent() < 1.0) ? 2 : 1); }
    double GetTimeBetweenLaserBeamPreps() const { return 1.0; } // This must be constant
    double GenerateTimeBetweenLaserBeamLastPrepAndFire() const { return 1.5; }
    double GetTimeBetweenLaserBeamFirings() const { return 0.75f; } // This must be constant

    int GenerateNumArcLaserFireMovements() { return (Randomizer::GetInstance()->RandomUnsignedInt() % 2) + 2; }
    double GenerateArcLaserTimeToWaitAtTarget() { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5) + 0.5; }
    double GenerateTimeBetweenRapidFireSweepLasers() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.16) + 0.08; }
    double GetRapidFireAngleSpeedInDegsPerSec() const { return RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND; }

    AnimationMultiLerp<float> GenerateTopDeathRotationAnimation();
    AnimationMultiLerp<Vector3D> GenerateTopDeathTranslationAnimation(GameModel* gameModel, bool goLeft);

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 4) + 3; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 3.5) + 0.75; }
    double GenerateTimeBetweenLasersWhileWaitingAtTarget() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.275) + 0.275; }
    float GetMaxMoveSpeedBetweenTargets() const { return MAX_MOVE_SPEED; }
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }

    void GoToNextRandomAttackState();

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    void ExecuteArcSprayFireState(double dT, GameModel* gameModel);
    void ExecuteHurtTopState(double dT, GameModel* gameModel);
    void ExecuteTopLostState(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(GlassDomeAI);
};


class TopSphereAI : public NouveauBossAI {
public:
    TopSphereAI(NouveauBoss* boss);
    ~TopSphereAI();

    // Inherited from NouveauBossAI/BossAIState
    void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine);

private:
    static const int NUM_HITS_UNTIL_DEAD = 5;
    static const float TOP_SPHERE_LIFE_POINTS;
    static const float TOP_SPHERE_DAMAGE_ON_HIT;
    static const float MAX_MOVE_SPEED;
    static const float DEFAULT_ACCELERATION;
    static const float RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND;

    BossWeakpoint* topSphereWeakpt;
    AnimationMultiLerp<Vector3D> topHurtAnim;

    void TopSphereWasHurt(const Point2D& impactPt);

    bool IsStateMachineFinished() const;
    float GetTotalLifePercent() const;

    int GenerateNumLaserBeamsToFire() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 3) + ((this->GetTotalLifePercent() < 1.0) ? 2 : 1); }
    double GetTimeBetweenLaserBeamPreps() const { return 0.85; } // This must be constant
    double GenerateTimeBetweenLaserBeamLastPrepAndFire() const { return 1.2; }
    double GetTimeBetweenLaserBeamFirings() const { return 0.66f; } // This must be constant

    double GenerateTimeBetweenRapidFireSweepLasers() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.14) + 0.07; }
    double GetRapidFireAngleSpeedInDegsPerSec() const { return RAPID_FIRE_ANGLE_CHANGE_SPEED_DEGS_PER_SECOND; }

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 4) + 3; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 2.5) + 0.75; }
    double GenerateTimeBetweenLasersWhileWaitingAtTarget() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.225) + 0.225; }
    float GetMaxMoveSpeedBetweenTargets() const { return MAX_MOVE_SPEED; }
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }

    void GoToNextRandomAttackState();

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    void ExecuteBrutalMoveAndShootState(double dT, GameModel* gameModel);
    void ExecuteHurtTopState(double dT, GameModel* gameModel);
    void ExecuteFinalDeathThroesState();

    DISALLOW_COPY_AND_ASSIGN(TopSphereAI);
};


}; // namespace nouveaubossai

#endif // __NOUVEAUBOSSAISTATES_H__