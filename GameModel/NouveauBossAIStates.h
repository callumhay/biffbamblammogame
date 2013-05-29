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

    float GetAccelerationMagnitude() const;

    // State-related variables
    enum AIState { BasicMoveAndShootAIState, RapidFireAIState, PrepLaserBeamAttackAIState, LaserBeamAttackAIState };
    AIState currState;

    virtual void SetState(NouveauBossAI::AIState newState) = 0;

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
    Point2D startPosition;           // The position the boss started in before its current movement
    Point2D targetPosition;          // The position the boss is moving towards
    int numLinearMovements;          // The number of linear movements the boss will make before going to the next state
    double waitingAtTargetCountdown; // Time to wait at the current targetPosition before choosing a new one and moving to it
    


    void KillLeftArm();
    void KillRightArm();

    float GetTotalLifePercent() const;

    int GenerateNumMovements() const { return (Randomizer::GetInstance()->RandomUnsignedInt() % 8) + 3; }
    double GenerateWaitAtTargetTime() const { return (Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0) + 1.0; }

    Point2D ChooseTargetPosition(const Point2D& startPos) const;

    void SetState(NouveauBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

    // State-specific methods
    void ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel);
    void ExecuteRapidFireState(double dT, GameModel* gameModel);
    void ExecutePrepLaserBeamAttackState(double dT, GameModel* gameModel);
    void ExecuteLaserBeamAttackState(double dT, GameModel* gameModel);

    

    AnimationMultiLerp<Vector3D> GenerateArmDeathTranslationAnimation(bool isLeftArm) const;
    AnimationMultiLerp<float> GenerateArmDeathRotationAnimation(bool isLeftArm) const;
};

/*
class GlassDomeAI : public NouveauBossAI {
};

class TopBottomSphereAI : public NouveauBossAI {
public:
private:
    bool IsStateMachineFinished() const;
};

*/

}; // namespace nouveaubossai

#endif // __NOUVEAUBOSSAISTATES_H__