/**
 * ClassicalBossAIStates.h
 * 
 * Copyright (c) 2014, Callum Hay
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

#ifndef __CLASSICALBOSSAISTATES_H__
#define __CLASSICALBOSSAISTATES_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

#include "../GameSound/SoundCommon.h"

#include "BossAIState.h"
#include "GameLevel.h"

class ClassicalBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;
class PlayerPaddle;

namespace classicalbossai {

/**
 * Abstract superclass for all of the ClassicalBoss AI state machine classes.
 */
class ClassicalBossAI : public BossAIState {
public:
    ClassicalBossAI(ClassicalBoss* boss);
    virtual ~ClassicalBossAI();

    // Inherited functions
    Boss* GetBoss() const;
    virtual bool IsStateMachineFinished() const { return false; }
    Collision::AABB2D GenerateDyingAABB() const;

protected:
    static const float BOSS_HEIGHT;
    static const float BOSS_WIDTH;
    static const float HALF_BOSS_WIDTH;

    ClassicalBoss* boss;

    // State-related variables
    enum AIState { BasicMoveAndLaserSprayAIState, ChasePaddleAIState, 
                   AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState,
                   PrepLaserAIState, MoveAndBarrageWithLaserAIState,
                   HurtLeftArmAIState, HurtRightArmAIState, LostArmsAngryAIState,
                   LostColumnAIState, LostAllColumnsAIState, LostBodyAngryAIState,
                   MoveToCenterOfLevelAIState, EyeRisesFromPedimentAIState,
                   SpinningPedimentAIState, HurtEyeAIState, FinalDeathThroesAIState};

    AIState currState;

    AnimationMultiLerp<ColourRGBA> angryColourAnim;
    AnimationMultiLerp<Vector3D> angryMoveAnim;

    SoundID laserChargeSoundID;

    // Attack routines
    void ExecuteLaserSpray(GameModel* gameModel);
    void SignalLaserFireEffects();

    // Shared movement data getters
    double GenerateBasicMoveTime() const { return 10.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    float GetBossMovementMinXBound(const GameLevel* level, float bossWidth) const;
    float GetBossMovementMaxXBound(const GameLevel* level, float bossWidth) const;
    float GetBasicMovementHeight(const GameLevel* level) const;
    float GetPrepLaserHeight(const GameLevel* level) const;
    float GetEyeRiseHeight() const;

    // Basic boss attribute getters
    float GetMaxSpeed() const;
    float GetAccelerationMagnitude() const;
    float GetSpeedModifier(float initialDivisor, float currHealthPercent) const {
        return 1.0f / (initialDivisor * std::max<float>(1.0f / initialDivisor, currHealthPercent));
    }

    // Update functions
    virtual void SetState(ClassicalBossAI::AIState newState) = 0;
    
private:
    DISALLOW_COPY_AND_ASSIGN(ClassicalBossAI);
};


class ArmsBodyHeadAI : public ClassicalBossAI {
public:
    ArmsBodyHeadAI(ClassicalBoss* boss);
    ~ArmsBodyHeadAI();

	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

    bool CanHurtPaddleWithBody() const { return this->isAttackingWithArm; }

private:
    static const float ARM_HEIGHT;
    static const float ARM_WIDTH;

    static const float ARM_LIFE_POINTS;
    static const float ARM_BALL_DAMAGE;

    static const double ARM_ATTACK_DELTA_T;
    static const double ARM_FADE_TIME;

    BossCompositeBodyPart* leftArm;
    BossCompositeBodyPart* rightArm;

    BossWeakpoint* leftArmSqrWeakpt;
    BossWeakpoint* rightArmSqrWeakpt;

    bool isAttackingWithArm;

    // AIState Variables Specific to this state ---------------------------------------------------

    // BasicMoveAndLaserSprayAIState
    static const double LASER_SPRAY_RESET_TIME_IN_SECS;
    double laserSprayCountdown;
    double countdownToNextState;
    
    // ChasePaddleAIState
    double countdownToAttack;
    AIState nextAttackState;
    double temptAttackCountdown;
    double lastRecourceLaserCountdown;

    // AttackLeftArmAIState, AttackRightArmAIState, AttackBothArmsAIState
    AnimationMultiLerp<float> armShakeAnim;
    AnimationMultiLerp<float> armAttackYMovementAnim;

    // ExecutePrepLaserState
    double countdownToLaserBarrage;
    float movingDir;

    // MoveAndBarrageWithLaserAIState
    double laserShootTimer;

    // HurtLeftArmAIState, HurtRightArmAIState
    AnimationMultiLerp<Vector3D> leftArmHurtMoveAnim;
    AnimationMultiLerp<Vector3D> rightArmHurtMoveAnim;


    // -----------------------------------------------------------------------------

    void SetState(ClassicalBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel);
    void ExecuteChasePaddleState(double dT, GameModel* gameModel);
    void ExecuteArmAttackState(double dT, bool isLeftArmAttacking, bool isRightArmAttacking, GameModel* gameModel);
    void ExecutePrepLaserState(double dT, GameModel* gameModel);
    void ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel);
    void ExecuteHurtArmState(double dT, bool isLeftArm, GameModel* gameModel);
    void ExecuteArmsLostAngryState(double dT);

    // Helper functions for generating various pieces of data across states
    double GenerateChaseTime() const { return 4.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 3.5; }
    double GenerateTemptAttackTime() const { return 1.0; }
    double GetLaserChargeTime() const { return 1.25; }
    double GetTimeBetweenLaserBarrageShots() const { return 0.08 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.1; }
    
    float GetFollowAndAttackHeight() const;
    float GetMaxArmAttackYMovement() const;

    float GetTotalLifePercent() const;

    ArmsBodyHeadAI::AIState DetermineNextArmAttackState(const Vector2D& bossToPaddleVec) const;

    AnimationMultiLerp<Vector3D> GenerateArmDeathTranslationAnimation(bool isLeftArm) const;
    AnimationMultiLerp<float> GenerateArmDeathRotationAnimation(bool isLeftArm) const;

    DISALLOW_COPY_AND_ASSIGN(ArmsBodyHeadAI);
};


class BodyHeadAI : public ClassicalBossAI {
public:
    BodyHeadAI(ClassicalBoss* boss);
    ~BodyHeadAI();

	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

    bool CanHurtPaddleWithBody() const { return false; }

private:
    static const float COLUMN_LIFE_POINTS;
    static const float COLUMN_BALL_DAMAGE;

    static const double COLUMN_FADE_TIME;
    static const double BODY_FADE_TIME;

    std::vector<BossWeakpoint*> columnWeakpts;  // Left to right ordering
    BossBodyPart* tabTopLeft;
    BossBodyPart* tabTopRight;
    BossBodyPart* tabBottomLeft;
    BossBodyPart* tabBottomRight;
    BossBodyPart* base;
    BossBodyPart* pediment;
    BossBodyPart* eye;

    // AIState Variables Specific to this state ---------------------------------------------------

    // BasicMoveAndLaserSprayAIState
    static const double LASER_SPRAY_RESET_TIME_IN_SECS;
    double laserSprayCountdown;
    double countdownToNextState;
    int numConsecutiveTimesBasicMoveExecuted;

    // ExecutePrepLaserAIState
    double countdownToLaserBarrage;
    float movingDir;

    // ExecuteMoveAndBarrageWithLaserAIState
    double laserShootTimer;
    int numConsecutiveBarrages;

    // LostColumnAIState
    AnimationMultiLerp<Vector2D> columnHurtMoveAnim;

    // LostAllColumnsAIState
    double lostAllColumnsWaitCountdown;

    // MoveToCenterOfLevelAIState
    AnimationLerp<Vector2D> bossToCenterOfLevelAnim;
    
    // EyeRisesFromPedimentAIState
    AnimationLerp<Vector2D> eyeRiseAnim;
    SoundID transitionSoundID;
    // --------------------------------------------------------------------------------------------

    void SetState(ClassicalBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void ExecuteBasicMoveAndLaserSprayState(double dT, GameModel* gameModel);
    void ExecutePrepLaserState(double dT, GameModel* gameModel);
    void ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel);
    void ExecuteLostColumnState(double dT);
    void ExecuteLostAllColumnsState(double dT);
    void ExecuteLostBodyAngryState(double dT, GameModel* gameModel);
    void ExecuteMoveToCenterOfLevelState(double dT, GameModel* gameModel);
    void ExecuteEyeRisesFromPedimentState(double dT);

    double GetLaserChargeTime() const { return 1.0; }
    double GetTimeBetweenLaserBarrageShots() const { return 0.075 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.1; }

    float GetTotalLifePercent() const;

    AnimationMultiLerp<Vector3D> GenerateColumnDeathTranslationAnimation(float xForceDir) const;
    AnimationMultiLerp<float> GenerateColumnDeathRotationAnimation(float xForceDir) const;

    AnimationMultiLerp<Vector3D> GenerateBaseDeathTranslationAnimation() const;
    AnimationMultiLerp<float> GenerateBaseDeathRotationAnimation() const;
    AnimationMultiLerp<Vector3D> GenerateTablatureDeathTranslationAnimation(bool isLeft, bool isTop) const;
    AnimationMultiLerp<float> GenerateTablatureDeathRotationAnimation(bool isLeft) const;

    DISALLOW_COPY_AND_ASSIGN(BodyHeadAI);
};


class HeadAI : public ClassicalBossAI {
public:
    HeadAI(ClassicalBoss* boss);
    ~HeadAI();

	void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

    bool CanHurtPaddleWithBody() const { return false; }
    bool IsStateMachineFinished() const;

private:
    static const float EYE_LIFE_POINTS;
    static const float EYE_BALL_DAMAGE;

    BossWeakpoint* eye;
    BossBodyPart* pediment;

    Vector2D currPedimentVel;
    Vector2D currEyeVel;

    // MoveAndBarrageWithLaserAIState
    bool movePedimentUpAndDown;

    // MoveAndBarrageWithLaserAIState, SpinningPedimentAIState
    double laserShootTimer;
    double moveToNextStateCountdown;

    // HurtEyeAIState
    AnimationMultiLerp<Vector3D> eyeHurtMoveAnim;
    
    void SetState(ClassicalBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void UpdateMovement(double dT);

    void ExecuteMoveAndBarrageWithLaserState(double dT, GameModel* gameModel);
    void ExecuteSpinningPedimentState(double dT, GameModel* gameModel);
    void ExecuteHurtEyeState(double dT);
    void ExecuteFinalDeathThroesState();

    void PerformBasicEyeMovement(const Point2D& eyePos, const GameLevel* level);
    void PerformBasicPedimentMovement(const Point2D& pedimentPos, const GameLevel* level);
    void UpdateEyeAndPedimentHeightMovement();

    double GetTimeBetweenLaserBarrageShots() const { return 0.075 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.065; }
    double GetTimeBetweenLaserSprayShots() const { return 0.75 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5; }
    float GetPedimentBasicMoveHeight(const GameLevel* level);
    float GetEyeBasicMoveHeight(const GameLevel* level);

    float GetTotalLifePercent() const;

    double GeneratePedimentSpinningTime() const { return 10.0; }

    AnimationMultiLerp<float> GenerateSpinningPedimentRotationAnim(double animationTime);

    DISALLOW_COPY_AND_ASSIGN(HeadAI);
};

};

#endif // __CLASSICALBOSSAISTATES_H__