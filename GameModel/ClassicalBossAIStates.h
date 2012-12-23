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

#include "BossAIState.h"

class ClassicalBoss;

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
    enum AIState { ChanceAIState, FollowPaddleAIState, AttackLeftArmAIState,
                   AttackRightArmAIState, AttackBothArmsAIState, FarLeftPrepLaserAIState,
                   FarRightPrepLaserAIState, MoveLeftLaserAIState, MoveRightLaserAIState,
                   HurtAIState, LostArmsAngryAIState };

    AIState currState;
    void UpdateState(double dT);

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