/**
 * DecoBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

#include "BossAIState.h"
#include "GameItem.h"

class DecoBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;

namespace decobossai {

/**
 * Abstract superclass for all of the DecoBoss AI state machine classes.
 */
class DecoBossAIState : public BossAIState {
public:
    DecoBossAIState(DecoBoss* boss);
    virtual ~DecoBossAIState();

    float GetItemLoadOffset() const { return this->itemLoadingAnim.GetInterpolantValue(); }
    GameItem::ItemType GetNextItemDropType() const { return this->nextDropItemType; }

    // Inherited functions
    Boss* GetBoss() const;
    bool CanHurtPaddleWithBody() const { return (this->currState == FiringArmsAtPaddleAIState); }
    bool IsStateMachineFinished() const { return (this->currState == FinalDeathThroesAIState); }
    Collision::AABB2D GenerateDyingAABB() const;

protected:
    static const float DEFAULT_ACCELERATION;
    static const float DEFAULT_MAX_X_SPEED;
    static const float DEFAULT_MAX_Y_SPEED;
    static const float DEFAULT_ARM_ROTATION_SPEED_DEGS_PER_SEC;
    static const float SHOOT_AT_PADDLE_RANDOM_ROT_ANGLE_IN_DEGS;
    static const float DEFAULT_LEVEL_ROTATION_SPEED_DEGS_PER_SEC;
    static const float DEFAULT_LEVEL_ROTATION_AMT_IN_DEGS;

    static const double TOTAL_ELECTRIFIED_TIME_IN_SECS;
    static const double TOTAL_RETALIATION_TIME_IN_SECS;
    static const double TOTAL_ELECTRIFIED_AND_RETALIATION_TIME_IN_SECS;

    DecoBoss* boss;
    AnimationMultiLerp<Vector3D> angryMoveAnim;

    enum AIState {
        StationaryAttackAIState,            // Boss stays still and attacks player with lightning bursts and static orbs
        MovingAttackAIState,                // Boss moves around and attacks player with lightning bursts and static orbs
        MovingAttackAndItemDropAIState,     // Similar to MovingAttackAIState, but the boss also occasionally drops nasty items
        MoveToFarLeftSideAIState,           // Boss moves to the far left side of the level, prep for LeftToRightItemDropAIState
        MoveToFarRightSideAIState,          // Boss moves to the far right side of the level, prep for RightToLeftItemDropAIState
        MoveToCenterForLevelRotAIState,     // Boss moves to the center of the level, prep for shooting its arms to rotate the level
        MoveToPaddleArmAttackPosAIState,    // Boss moves to a good location for attacking the paddle with its arms, prep for FiringArmsAtPaddleAIState
        LeftToRightItemDropAIState,         // Boss moves only horizontally left to right, dropping nasty items rapidly as it moves
        RightToLeftItemDropAIState,         // Boss moves only horizontally right to left, dropping nasty items rapidly as it moves
        FiringArmsAtPaddleAIState,
        FinishedFiringArmsAtPaddleAIState,
        FiringArmsAtLevelAIState,
        RotatingLevelAIState,               // Boss is rotating the entire level with its arm(s)
        FinishRotatingLevelAIState,         // Boss is done rotating the level, it retracts its arms
        ElectrifiedAIState,                 // Boss is damaged by the Tesla block's lightning arc
        ElectrificationRetaliationAIState,  // Boss retaliates to being damaged by short-circuiting the Tesla blocks
        AngryAIState,                       // Boss is angry (after being hurt and retaliating)... Rawr.
        FinalDeathThroesAIState             // Boss is dying/exploding, this is the last state before the end of the battle
    };
    AIState currState;

    //static void DoFirstTimeInit();

    Point2D GeneratePaddleArmAttackPosition(GameModel* gameModel) const;
    Point2D GenerateShotOrigin() const;
    Vector2D GenerateRandomShotDirTowardsPaddle(const Point2D& shotOrigin, GameModel* gameModel) const;
    void ShootLightningBoltAtPaddle(GameModel* gameModel);
    //void ShootStaticOrbAtPaddle(GameModel* gameModel);

    virtual float GetMaxXSpeed() const { return DEFAULT_MAX_X_SPEED; }
    virtual float GetMaxYSpeed() const { return DEFAULT_MAX_Y_SPEED; }
    virtual float GetLevelRotationSpeed() const { return DEFAULT_LEVEL_ROTATION_SPEED_DEGS_PER_SEC; }

    AnimationMultiLerp<Vector3D> GenerateArmDeathTranslationAnimation(bool isLeft, double timeInSecs) const;
    AnimationMultiLerp<float> GenerateArmDeathRotationAnimation(bool isLeft, double timeInSecs) const;

    double GenerateShootCountdownWhileStationary() const { 
        return 0.15 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.15 + 
            0.2 * Randomizer::GetInstance()->RandomNumZeroToOne() * this->GetTotalLifePercent(); 
    }
    int GenerateNumShotsWhileStationary() const {
        return 10 + Randomizer::GetInstance()->RandomUnsignedInt() % 6 + 
            Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(3.0f / this->GetTotalLifePercent());
    }
    double GenerateShootCountdownWhileMoving() const {
        return 0.12 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.13 + 
            0.22 * Randomizer::GetInstance()->RandomNumZeroToOne() * this->GetTotalLifePercent(); 
    }
    int GenerateNumShotsWhileMoving() const {
        return 20 + Randomizer::GetInstance()->RandomUnsignedInt() % 8 + 
            Randomizer::GetInstance()->RandomUnsignedInt() % static_cast<int>(3.25f / this->GetTotalLifePercent());
    }
    double GenerateItemDropCountdown() const {
        return 3.0 + 3.0 * Randomizer::GetInstance()->RandomNumZeroToOne() + 1.0 * this->GetTotalLifePercent();
    }
    double GenerateTimeToFollowPaddleBeforeShootingArms() const {
        return 1.5 + Randomizer::GetInstance()->RandomNumZeroToOne() * 2.0;
    }
    double GenerateRotateShakeCountdown() const {
        return 0.4 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.75;
    }
    float CalculateSideToSideDropStateVelocity() const;

    virtual GameItem::ItemType GenerateRandomItemDropType(GameModel* gameModel) const = 0;
    virtual int GenerateNumItemsToDropInSideToSideState() const = 0;

    bool RemoteControlRocketCheckAndNecessaryStateChange(GameModel* gameModel);

    virtual bool IsFinalAIStage() const { return false; }
    virtual void SetState(DecoBossAIState::AIState newState) = 0;
    virtual void GoToNextRandomAttackState(GameModel* gameModel) = 0;
    virtual DecoBossAIState* BuildNextAIState() const = 0;

    virtual bool ShutsOffLeftTeslaBlockOnRetaliation() const { return true; }
    virtual bool ShutsOffRightTeslaBlockOnRetaliation() const { return true; }

    void UpdateBossUpDownSideToSideMotion();
    bool UpdateShootAtPaddleWhileMoving(double dT, GameModel* gameModel);
    bool UpdateArmAnimation(double dT, AnimationMultiLerp<float>& armSeg1Anim, AnimationMultiLerp<float>& armSeg2Anim, 
        AnimationMultiLerp<float>& armSeg3Anim, AnimationMultiLerp<float>& armSeg4Anim);
    bool RotateArmsToDefaultPosition(double dT);
    
    void DropLoadedItem(GameModel* gameModel);

    // State Variables and functions
    double shootCountdown;
    int numShotsUntilNextState;
    double dropItemCountdown;
    GameItem::ItemType nextDropItemType;
    AnimationMultiLerp<Vector3D> electrifiedHurtAnim;
    AnimationLerp<float> itemLoadingAnim;
    AnimationMultiLerp<float> leftArmShakeAnim;
    AnimationMultiLerp<float> rightArmShakeAnim;
    int sideToSideNumDropCountdown;
    
    AnimationMultiLerp<float> armSeg1RotateExtendAnim, armSeg2RotateExtendAnim, armSeg3RotateExtendAnim, armSeg4RotateExtendAnim;
    AnimationMultiLerp<float> armSeg1RetractAnim,  armSeg2RetractAnim, armSeg3RetractAnim, armSeg4RetractAnim;

    AnimationMultiLerp<float> armSeg1AttackExtendAnim,  armSeg2AttackExtendAnim, armSeg3AttackExtendAnim, armSeg4AttackExtendAnim;

    float currLeftArmRotInDegs;
    float currRightArmRotInDegs;
    float currLevelRotationAmtInDegs;
    int rotationDir;
    double rotateShakeCountdown;

    void InitStationaryAttackState();
    void InitMovingAttackState();
    void InitMovingAttackAndItemDropState();
    void InitMoveToFarLeftSideState();
    void InitMoveToFarRightSideState();
    void InitLeftToRightItemDropState();
    void InitRightToLeftItemDropState();
    void InitMoveToCenterForLevelRotState();
    void InitMoveToPaddleArmAttackPosState();
    void InitFiringArmsAtPaddleState();
    void InitFinishedFiringArmsAtPaddleState();
    void InitFiringArmsAtLevelState();
    void InitRotatingLevelState();
    void InitFinishRotatingLevelState();
    void InitElectrifiedState();
    void InitElectrificationRetaliationState();
    void InitAngryState();

    void ExecuteStationaryAttackState(double dT, GameModel* gameModel);
    void ExecuteMovingAttackState(double dT, GameModel* gameModel);
    void ExecuteMovingAttackAndItemDropState(double dT, GameModel* gameModel);
    void ExecuteMoveToFarLeftSideState(double dT, GameModel* gameModel);
    void ExecuteMoveToFarRightSideState(double dT, GameModel* gameModel);
    void ExecuteSideToSideItemDropState(double dT, GameModel* gameModel);
    void ExecuteMoveToCenterForLevelRotState(double dT, GameModel* gameModel);
    void ExecuteMoveToPaddleArmAttackPosState(double dT, GameModel* gameModel);
    void ExecuteFiringArmsAtPaddleState(double dT, GameModel* gameModel);
    void ExecuteFinishedFiringArmsAtPaddleState(double dT, GameModel* gameModel);
    void ExecuteFiringArmsAtLevelState(double dT, GameModel* gameModel);
    void ExecuteRotatingLevelState(double dT, GameModel* gameModel);
    void ExecuteFinishRotatingLevelState(double dT, GameModel* gameModel);
    void ExecuteElectrifiedState(double dT, GameModel* gameModel);
    void ExecuteElectrifiedRetaliationState(double dT, GameModel* gameModel);
    void ExecuteAngryState(double dT);
    void ExecuteFinalDeathThroesState();

    // Inherited functions
    void CollisionOccurred(GameModel*, GameBall&, BossBodyPart*) {};
    void CollisionOccurred(GameModel*, Projectile*, BossBodyPart*) {};
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {};
    void TeslaLightningArcHitOccurred(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2);
    void UpdateState(double dT, GameModel* gameModel);

private:
    DISALLOW_COPY_AND_ASSIGN(DecoBossAIState);
};

class Stage1AI : public DecoBossAIState {
public:
    Stage1AI(DecoBoss* boss);
    ~Stage1AI();

private:
    AnimationMultiLerp<Vector3D> GenerateSideBodyPartDeathTranslationAnimation(bool isLeft, double timeInSecs) const;
    AnimationMultiLerp<float> GenerateSideBodyPartDeathRotationAnimation(bool isLeft, double timeInSecs) const;

    // Inherited Functions
    bool ShutsOffLeftTeslaBlockOnRetaliation() const { return true; }
    bool ShutsOffRightTeslaBlockOnRetaliation() const { return false; }
    GameItem::ItemType GenerateRandomItemDropType(GameModel* gameModel) const;
    int GenerateNumItemsToDropInSideToSideState() const { return 4; } // Must not be random!
    DecoBossAIState* BuildNextAIState() const;
    float GetTotalLifePercent() const { return 1.0f; }
    void SetState(DecoBossAIState::AIState newState);
    void GoToNextRandomAttackState(GameModel* gameModel);
    float GetAccelerationMagnitude() const { return DecoBossAIState::DEFAULT_ACCELERATION; }
};


class Stage2AI : public DecoBossAIState {
public:
    Stage2AI(DecoBoss* boss);
    ~Stage2AI();

private:
    static const float SPEED_COEFF;

    BossCompositeBodyPart* stayAliveArm;
    BossCompositeBodyPart* becomeDeadArm;

    float GetMaxXSpeed() const { return SPEED_COEFF * DecoBossAIState::DEFAULT_MAX_X_SPEED; }
    float GetMaxYSpeed() const { return SPEED_COEFF * DecoBossAIState::DEFAULT_MAX_Y_SPEED; }

    // Inherited Functions
    bool ShutsOffLeftTeslaBlockOnRetaliation() const { return true; }
    bool ShutsOffRightTeslaBlockOnRetaliation() const { return true; }
    GameItem::ItemType GenerateRandomItemDropType(GameModel* gameModel) const;
    int GenerateNumItemsToDropInSideToSideState() const { return 6; } // Must not be random!
    DecoBossAIState* BuildNextAIState() const;
    float GetTotalLifePercent() const { return 0.666666f; }
    void SetState(DecoBossAIState::AIState newState);
    void GoToNextRandomAttackState(GameModel* gameModel);
    float GetAccelerationMagnitude() const { return 1.2f * DecoBossAIState::DEFAULT_ACCELERATION; }
};

class Stage3AI : public DecoBossAIState {
public:
    Stage3AI(DecoBoss* boss, BossCompositeBodyPart* remainingArm, 
        float currLevelRotationAmtInDegs, float remainingArmRot);
    ~Stage3AI();

private:
    static const float SPEED_COEFF;
    
    BossCompositeBodyPart* remainingArm;
    
    float GetMaxXSpeed() const { return SPEED_COEFF * DecoBossAIState::DEFAULT_MAX_X_SPEED; }
    float GetMaxYSpeed() const { return SPEED_COEFF * DecoBossAIState::DEFAULT_MAX_Y_SPEED; }
    float GetLevelRotationSpeed() const { return 1.3f * DecoBossAIState::DEFAULT_LEVEL_ROTATION_SPEED_DEGS_PER_SEC; }

    // Inherited Functions
    GameItem::ItemType GenerateRandomItemDropType(GameModel* gameModel) const;
    int GenerateNumItemsToDropInSideToSideState() const { return 8; } // Must not be random!
    DecoBossAIState* BuildNextAIState() const { assert(false); return NULL; }
    bool IsFinalAIStage() const { return true; }
    float GetTotalLifePercent() const { return 0.33333333f; }
    void SetState(DecoBossAIState::AIState newState);
    void GoToNextRandomAttackState(GameModel* gameModel);
    float GetAccelerationMagnitude() const { return 1.4f * DecoBossAIState::DEFAULT_ACCELERATION; }

};

}; // namespace decobossai