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

#include "BossAIState.h"

class ClassicalBoss;
class BossBodyPart;
class BossCompositeBodyPart;
class PlayerPaddle;
class GameLevel;

namespace classicalbossai {

/**
 * Abstract superclass for all of the ClassicalBosses AI state machine classes.
 */
class ClassicalBossAI : public BossAIState {
public:
    ClassicalBossAI(ClassicalBoss* boss) : BossAIState(), boss(boss) { assert(boss != NULL); };
    virtual ~ClassicalBossAI() { this->boss = NULL; };

protected:
    ClassicalBoss* boss;

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
    BossCompositeBodyPart* leftArm;
    BossCompositeBodyPart* rightArm;

    BossBodyPart* leftArmSqrWeakpt;
    BossBodyPart* rightArmSqrWeakpt;

    Vector2D currVel;
    Vector2D desiredVel;

    enum AIState { ChanceAIState, BasicMovementAIState, FollowPaddleAIState, AttackLeftArmAIState,
                   AttackRightArmAIState, AttackBothArmsAIState, FarLeftPrepLaserAIState,
                   FarRightPrepLaserAIState, MoveLeftLaserAIState, MoveRightLaserAIState,
                   HurtAIState, LostArmsAngryAIState };

    // AIState Variables -----------------------------------------------------------
    AIState currState;

    // BasicMovementAIState

    // FollowPaddleAIState
    double countdownToAttack;
    // AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState
    AnimationMultiLerp<float> armShakeAnim;
    AnimationMultiLerp<float> armAttackYMovementAnim;
    Point3D leftArmStartWorldPos;
    Point3D rightArmStartWorldPos;
    // -----------------------------------------------------------------------------

    void SetState(AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void UpdateMovement(double dT, GameModel* gameModel);
    void ExecuteBasicMovementState(double dT, const GameLevel* level);
    void ExecuteFollowPaddleState(double dT, const PlayerPaddle* paddle);
    void ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking);

    float GetMaxSpeed() const;
    Vector2D GetAcceleration() const;

    double GenerateFollowTime() const { return 4.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    float GetBasicMovementHeight(const GameLevel* level) const;
    float GetFollowAndAttackHeight() const;
    float GetMaxArmAttackYMovement() const;

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