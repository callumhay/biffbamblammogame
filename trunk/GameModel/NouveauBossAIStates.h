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

#include "BossAIState.h"
#include "GameLevel.h"

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
    enum AIState { MoveToTargetStopAndShootAIState, RapidFireAIState, PrepLaserBeamAttackAIState, LaserBeamAttackAIState,
                   LostLeftArmAIState, LostRightArmAIState, BothArmsLostAIState, HurtTopAIState };
    AIState currState;

    // State-specific variables
    Point2D startPosition;           // The position the boss started in before its current movement
    Point2D targetPosition;          // The position the boss is moving towards
    int numLinearMovements;          // The number of linear movements the boss will make before going to the next state
    double waitingAtTargetCountdown; // Time to wait at the current targetPosition before choosing a new one and moving to it
    double timeUntilNextLaserWhileWaitingAtTarget; // Time until the boss fires the next laser while waiting at it's target

    virtual void SetState(NouveauBossAI::AIState newState) = 0;

    virtual int GenerateNumMovements() const = 0;
    virtual double GenerateWaitAtTargetTime() const = 0;
    virtual double GetTimeBetweenLasersWhileWaitingAtTarget() const = 0;
    virtual float GetMaxMoveSpeedBetweenTargets() const = 0;

    virtual void GoToNextRandomAttackState() = 0;

    Point2D ChooseTargetPosition(const Point2D& startPos) const;
    void ShootRandomLaserBullet(GameModel* gameModel);

    // State-specific methods
    void OnSetStateMoveToTargetStopAndShoot();
    void ExecuteMoveToTargetStopAndShootState(double dT, GameModel* gameModel);

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

    void KillLeftArm();
    void KillRightArm();

    float GetTotalLifePercent() const;

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 8) + 4; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 4.0) + 1.0; }
    double GetTimeBetweenLasersWhileWaitingAtTarget() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.2) + 0.25; }
    float GetMaxMoveSpeedBetweenTargets() const { return MAX_MOVE_SPEED; }
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }

    void GoToNextRandomAttackState();

    void CheckForBothArmsDeadAndChangeState();

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    // State-specific methods
    void ExecuteRapidFireState(double dT, GameModel* gameModel);
    void ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel);
    void ExecuteLaserBeamAttackState(double dT, GameModel* gameModel);
    void ExecuteLostLeftArmState(double dT, GameModel* gameModel);
    void ExecuteLostRightArmState(double dT, GameModel* gameModel);
    void ExecuteBothArmsLostState(double dT, GameModel* gameModel);

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
    static const float TOP_SPHERE_LIFE_POINTS;
    static const float MAX_MOVE_SPEED;
    static const float DEFAULT_ACCELERATION;

    BossWeakpoint* topSphereWeakpt;

    float GetTotalLifePercent() const;

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 5) + 4; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 3.5) + 0.75; }
    double GetTimeBetweenLasersWhileWaitingAtTarget() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 0.2) + 0.2; }
    float GetMaxMoveSpeedBetweenTargets() const { return MAX_MOVE_SPEED; }
    float GetAccelerationMagnitude() const { return DEFAULT_ACCELERATION; }

    void GoToNextRandomAttackState();

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(GlassDomeAI);
};

/*
class TopBottomSphereAI : public NouveauBossAI {
public:
private:
    bool IsStateMachineFinished() const;
};

*/

}; // namespace nouveaubossai

#endif // __NOUVEAUBOSSAISTATES_H__