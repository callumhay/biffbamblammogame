/**
 * ClassicalBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __CLASSICALBOSSAISTATES_H__
#define __CLASSICALBOSSAISTATES_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"

#include "BossAIState.h"

class ClassicalBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;
class PlayerPaddle;
class GameLevel;

namespace classicalbossai {

/**
 * Abstract superclass for all of the ClassicalBosses AI state machine classes.
 */
class ClassicalBossAI : public BossAIState {
public:
    ClassicalBossAI(ClassicalBoss* boss);
    virtual ~ClassicalBossAI();

protected:
    ClassicalBoss* boss;

    // Attack routines
    void ExecuteLaserSpray(GameModel* gameModel);

private:
    DISALLOW_COPY_AND_ASSIGN(ClassicalBossAI);
};


class ArmsBodyHeadAI : public ClassicalBossAI {
public:
    ArmsBodyHeadAI(ClassicalBoss* boss);
    ~ArmsBodyHeadAI();

    void Tick(double dT, GameModel* gameModel);

	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

private:
    static const float ARM_LIFE_POINTS;
    static const float ARM_BALL_DAMAGE;

    BossCompositeBodyPart* leftArm;
    BossCompositeBodyPart* rightArm;

    BossWeakpoint* leftArmSqrWeakpt;
    BossWeakpoint* rightArmSqrWeakpt;

    Vector2D currVel;
    Vector2D desiredVel;

    enum AIState { ChanceAIState, BasicMoveAndLaserSprayAIState, ChasePaddleAIState, 
                   AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState,
                   PrepLaserAIState, MoveAndBarrageWithLaserAIState,
                   HurtAIState, LostArmsAngryAIState };

    // AIState Variables -----------------------------------------------------------
    AIState currState;

    // BasicMoveAndLaserSprayAIState
    static const double LASER_SPRAY_RESET_TIME_IN_SECS;
    double laserSprayCountdown;
    double countdownToNextState;
    // ChasePaddleAIState
    double countdownToAttack;
    // AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState
    AnimationMultiLerp<float> armShakeAnim;
    AnimationMultiLerp<float> armAttackYMovementAnim;
    Matrix4x4 leftArmStartWorldT;
    Matrix4x4 rightArmStartWorldT;
    // -----------------------------------------------------------------------------

    void SetState(AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void UpdateMovement(double dT, GameModel* gameModel);
    void ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel);
    void ExecuteFollowPaddleState(double dT, const PlayerPaddle* paddle);
    void ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking);

    // Basic boss attribute getters
    float GetMaxSpeed() const;
    Vector2D GetAcceleration() const;

    // Helper functions for generating various pieces of data across states
    double GenerateBasicMoveTime() const { return 10.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    double GenerateFollowTime() const { return 5.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    float GetBasicMovementHeight(const GameLevel* level) const;
    float GetFollowAndAttackHeight() const;
    float GetMaxArmAttackYMovement() const;
    ArmsBodyHeadAI::AIState DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const;

    DISALLOW_COPY_AND_ASSIGN(ArmsBodyHeadAI);
};

/*
class BodyHeadAI : public BossAIState {
public:
    BodyHeadAI();
    ~BodyHeadAI();

    void Tick(double dT, GameModel* gameModel);
private:
    DISALLOW_COPY_AND_ASSIGN(BodyHeadAI);
};

class HeadAI : public BossAIState {
public:
    HeadAI();
    ~HeadAI();

    void Tick(double dT, GameModel* gameModel);
private:
    DISALLOW_COPY_AND_ASSIGN(HeadAI);
};

class DyingAI : public BossAIState {
public:
    DyingAI();
    ~DyingAI();

    void Tick(double dT, GameModel* gameModel);
private:
    DISALLOW_COPY_AND_ASSIGN(DyingAI);
};
*/

};

#endif // __CLASSICALBOSSAISTATES_H__