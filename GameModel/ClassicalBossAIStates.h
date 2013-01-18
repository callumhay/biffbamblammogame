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
#include "../BlammoEngine/Colour.h"

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

    // Shared movement data getters
    float GetBossMovementMinXBound(const GameLevel* level, float bossWidth) const;
    float GetBossMovementMaxXBound(const GameLevel* level, float bossWidth) const;

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

    bool CanHurtPaddleWithBody() const { return this->isAttackingWithArm; }

private:
    static const float BOSS_HEIGHT;
    static const float BOSS_WIDTH;
    static const float HALF_BOSS_WIDTH;

    static const float ARM_HEIGHT;
    static const float ARM_WIDTH;

    static const float ARM_LIFE_POINTS;
    static const float ARM_BALL_DAMAGE;

    static const double ARM_ATTACK_DELTA_T;

    BossCompositeBodyPart* leftArm;
    BossCompositeBodyPart* rightArm;

    BossWeakpoint* leftArmSqrWeakpt;
    BossWeakpoint* rightArmSqrWeakpt;

    Vector2D currVel;
    Vector2D desiredVel;

    bool isAttackingWithArm;

    AnimationMultiLerp<ColourRGBA> armAlphaFadeoutAnim;

    enum AIState { BasicMoveAndLaserSprayAIState, ChasePaddleAIState, 
                   AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState,
                   PrepLaserAIState, MoveAndBarrageWithLaserAIState,
                   HurtLeftArmAIState, HurtRightArmAIState, LostArmsAngryAIState };

    // AIState Variables -----------------------------------------------------------
    AIState currState;

    // BasicMoveAndLaserSprayAIState
    static const double LASER_SPRAY_RESET_TIME_IN_SECS;
    double laserSprayCountdown;
    double countdownToNextState;
    
    // ChasePaddleAIState
    double countdownToAttack;
    AIState nextAttackState;
    double temptAttackCountdown;

    // AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState
    AnimationMultiLerp<float> armShakeAnim;
    AnimationMultiLerp<float> armAttackYMovementAnim;

    // ExecutePrepLaserState
    double countdownToLaserBarrage;
    float movingDir;

    // MoveAndBarrageWithLaserAIState
    double laserShootTimer;
    std::vector<float> laserAngles;
    
    // HurtLeftArmAIState, HurtRightArmAIState
    AnimationMultiLerp<ColourRGBA> hurtColourAnim;
    AnimationMultiLerp<Vector3D> leftArmHurtMoveAnim;
    AnimationMultiLerp<Vector3D> rightArmHurtMoveAnim;

    // -----------------------------------------------------------------------------

    void SetState(AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void UpdateMovement(double dT, GameModel* gameModel);
    void ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel);
    void ExecuteChasePaddleState(double dT, GameModel* gameModel);
    void ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking);
    void ExecutePrepLaserState(double dT, GameModel* gameModel);
    void ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel);
    void ExecuteHurtArmState(double dT, bool isLeftArm);

    // Basic boss attribute getters
    float GetMaxSpeed() const;
    Vector2D GetAcceleration() const;

    // Helper functions for generating various pieces of data across states
    double GenerateBasicMoveTime() const { return 10.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    double GenerateChaseTime() const { return 8.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    double GenerateTemptAttackTime() const { return 1.0; }
    double GetLaserChargeTime() const { return 1.5; }
    double GetTimeBetweenLaserBarrageShots() const { return 0.1 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.1; }
    
    float GetBasicMovementHeight(const GameLevel* level) const;
    float GetFollowAndAttackHeight() const;
    float GetMaxArmAttackYMovement() const;
    float GetPrepLaserHeight(const GameLevel* level) const;

    ArmsBodyHeadAI::AIState DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const;

    AnimationMultiLerp<Vector3D> GenerateArmDeathTranslationAnimation(bool isLeftArm) const;
    AnimationMultiLerp<float> GenerateArmDeathRotationAnimation(bool isLeftArm) const;

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