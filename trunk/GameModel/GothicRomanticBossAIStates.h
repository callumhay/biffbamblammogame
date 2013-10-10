/**
 * GothicRomanticBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GOTHICROMANTICBOSSAISTATES_H__
#define __GOTHICROMANTICBOSSAISTATES_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

#include "../GameSound/SoundCommon.h"

#include "BossAIState.h"
#include "GameLevel.h"

class GothicRomanticBoss;
class BossBodyPart;
class BossWeakpoint;
class BossCompositeBodyPart;
class PlayerPaddle;

namespace gothicromanticbossai {

/**
 * Abstract superclass for all of the GothicRomanticBoss AI state machine classes.
 */
class GothicRomanticBossAI : public BossAIState {
public:
    GothicRomanticBossAI(GothicRomanticBoss* boss);
    virtual ~GothicRomanticBossAI();

    // Inherited functions
    Boss* GetBoss() const;
    bool CanHurtPaddleWithBody() const { return false; }
    virtual bool IsStateMachineFinished() const { return false; }
    Collision::AABB2D GenerateDyingAABB() const;
    
protected:
    static const float MOVE_X_BORDER;
    static const float MOVE_Y_BORDER;
    static const float DEFAULT_LASER_SPIN_DEGREES_PER_SEC;
    static const float DEFAULT_ROCKET_SPIN_DEGREES_PER_SEC;
    static const float DEFAULT_DROP_Y_POS;

    static const double GLITCH_TIME_IN_SECS;

    GothicRomanticBoss* boss;
    int attacksSinceLastSummon;
    AnimationMultiLerp<Vector3D> angryMoveAnim;

    // State-related variables
    enum AIState { BasicMoveAndShootState, SpinLaserAttackAIState, 
                   SummonItemsAIState, RocketToCannonAttackAIState,
                   RocketToPaddleAttackAIState, RocketToCannonAndPaddleAttackAIState,
                   HurtTopAIState, HurtBottomAIState, HurtBodyAIState, VeryHurtAndAngryAIState, 
                   DestroyRegenBlocksAIState, GlitchAIState,
                   MoveToCenterAIState, DestroyConfinesAIState,
                   FinalDeathThroesAIState};

    AIState currState;

    // Movement positions while confined
    enum ConfinedMovePos { TopLeftCorner = 0, TopRightCorner = 1, BottomLeftCorner = 2, BottomRightCorner = 3, Center = 4 };
    static const ConfinedMovePos CORNER_POSITIONS[];

    Point2D GetConfinedBossCenterMovePosition(const ConfinedMovePos& pos) const;
    static ConfinedMovePos GetRandomConfinedMovePosition(const ConfinedMovePos& currPos) {
        return static_cast<ConfinedMovePos>((currPos + (1 + Randomizer::GetInstance()->RandomUnsignedInt() % 4)) % 5);
    }
    static ConfinedMovePos GetRandomConfinedCornerMovePosition(const ConfinedMovePos& currPos);
    static AIState GetAIStateForConfinedMovePos(const ConfinedMovePos& pos, 
        float probabilityOfTargetCannonRocket, float probabilityOfTargetPaddleRocket);

    float GetSlowestConfinedMoveSpeed() const { return 1.5f * LevelPiece::PIECE_WIDTH; }
    double GetLaserSpinTime() const { return 5.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 3.5; }

    float GenerateSummonProbability() const;
    float GenerateRocketCannonAttackProbability(float multiplier = 1.0f) const {
        return NumberFuncs::LerpOverFloat<float>(0.0f, 1.0f, multiplier*0.3f, multiplier*0.6f, this->GetTotalLifePercent());
    }
    float GenerateRocketPaddleAttackProbability(float multiplier = 1.0f) const {
        return NumberFuncs::LerpOverFloat<float>(0.0f, 1.0f, multiplier*0.15f, multiplier*0.3f, this->GetTotalLifePercent());
    }

    float GetAccelerationMagnitude() const;
    virtual void SetState(GothicRomanticBossAI::AIState newState) = 0;

    void ShootOrbFromLegPoint(int legIdx, GameModel* gameModel) const;
    void ShootOrbFromBody(GameModel* gameModel) const;
    void ShootRocket(const Point2D& rocketTarget, GameModel* gameModel) const;
    void ShootRocketAtPaddle(GameModel* gameModel) const;

    static void GetItemDropPositions(int numItemsToDrop, const GameLevel& level, std::vector<Point2D>& positions);

    static AnimationMultiLerp<float> GenerateRocketSpinAnimation(int numRockets);
    static AnimationMultiLerp<Vector2D> GenerateGlitchShakeAnimation();
    static void GenerateRocketTargetCannonPositions(int numRockets, std::vector<Point2D>& positions);
    static void GenerateTargetRegenBlockPositions(std::vector<Point2D>& positions);

private:
    DISALLOW_COPY_AND_ASSIGN(GothicRomanticBossAI);
};

class ConfinedAI : public GothicRomanticBossAI {
public:
    ConfinedAI(GothicRomanticBoss* boss, int numItemsPerSummoning);
    virtual ~ConfinedAI() {}

    void CollisionOccurred(GameModel*, GameBall&, BossBodyPart*) {};
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {};

protected:
    // State variables
    ConfinedMovePos nextMovePos;
    AIState nextAtkAIState;
    double shootCountdown;
    AnimationLerp<float> laserSpinRotAnim;
    double summonItemsDelayCountdown;
    int numRocketsToFire;
    AnimationMultiLerp<float> rocketSpinRotAnim;
    std::vector<Point2D> rocketTargetPositions;
    
    int numItemsPerSummoning;
    int summonsSinceLastSpecialItemDrop;

    SoundID itemSummoningSoundID;

    virtual float GenerateSpecialItemSummonProbability() const;
    virtual bool AllowLevelFlipItem() const { return true; }
    double GenerateShootCountdownAmtForMoving()   const { return 0.5 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5; }
    double GenerateShootCountdownAmtForSpinning() const { return 0.3 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.3; }
    double GenerateTimeBetweenRockets() const { return this->rocketSpinRotAnim.GetTimeValues().back() / static_cast<double>(this->numRocketsToFire+1); }

    virtual void SetupNextAttackStateAndMove(GameModel* gameModel);

    // State methods
    void ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel);
    void ExecuteSpinLaserAttackState(double dT, GameModel* gameModel);
    void ExecuteRocketToCannonAttackState(double dT, GameModel* gameModel);
    void ExecuteSummonItemsState(double dT, GameModel* gameModel);

    // New pure-virtual methods
    virtual float GetRocketCannonAtkProbabilityMultiplier() const = 0;
    virtual float GetRocketPaddleAtkProbabilityMultiplier() const = 0;
    virtual GameItem::ItemType GetSpecialItemDropType() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(ConfinedAI);
};

class FireBallAI : public ConfinedAI {
public:
    static const int NUM_ITEMS_PER_SUMMONING = 4;

    FireBallAI(GothicRomanticBoss* boss);
    ~FireBallAI();
    
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);

private:
    static const int NUM_OF_TOP_POINT_HITS = 3; // Number of times the weakpoint has to be hit before we move to the next state
    static const float TOP_POINT_LIFE_POINTS;

    static const double TIME_BETWEEN_REGEN_BLOCK_DESTROYING_ROCKETS;
    static const double TOP_FADE_TIME;

    BossWeakpoint* topPointWeakpt;

    // State variables
    AnimationMultiLerp<float> hitOnTopTiltAnim;
    AnimationMultiLerp<float> hitOnTopMoveDownAnim;

    bool AllowLevelFlipItem() const { return false; }
    float GenerateFireballSummonProbability() const;
    int GenerateNumRocketsToFire() const { return 2 + Randomizer::GetInstance()->RandomUnsignedInt() % 2; }

    AnimationMultiLerp<float> GenerateHitOnTopTiltAnim(const Point2D& hitPos) const;
    AnimationMultiLerp<Vector3D> GenerateTopDeathTranslationAnimation(bool fallLeft) const;
    AnimationMultiLerp<float> GenerateTopDeathRotationAnimation(bool fallLeft) const;

    // Inherited from ConfinedAI/GothicRomanticBossAI/BossAIState
    void SetState(GothicRomanticBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    float GetTotalLifePercent() const;
    float GetRocketCannonAtkProbabilityMultiplier() const { return 1.0f; }
    float GetRocketPaddleAtkProbabilityMultiplier() const { return 0.0f; }
    GameItem::ItemType GetSpecialItemDropType() const { return GameItem::FireBallItem; }

    // State methods
    void ExecuteHurtTopState(double dT, GameModel* gameModel);
    void ExecuteVeryHurtAndAngryState(double dT, GameModel* gameModel);
    void ExecuteDestroyRegenBlocksState(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(FireBallAI);
};


class IceBallAI : public ConfinedAI {
public:
    static const int NUM_ITEMS_PER_SUMMONING = FireBallAI::NUM_ITEMS_PER_SUMMONING + 1;

    IceBallAI(GothicRomanticBoss* boss, ConfinedMovePos prevMovePos);
    ~IceBallAI();

    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);

private:
    static const int NUM_LASER_HITS = 3;
    static const float BOTTOM_POINT_LIFE_POINTS;
    static const double BOTTOM_FADE_TIME;

    static const double DESTROY_CONFINES_TIME_IN_SECS;
    static const float DESTROY_CONFINES_SPIN_ROT_SPD;
    static const double TIME_TO_CHARGE_CONFINE_DESTROYER_BLAST;
    static const double TIME_BEFORE_DESTROY_CONFINES_FLASH;

    BossWeakpoint* bottomPointWeakpt;

    // State variables
    AnimationMultiLerp<float> hitOnBottomTiltAnim;
    AnimationMultiLerp<float> hitOnBottomMoveUpAnim;
    AnimationMultiLerp<Vector2D> glitchShakeAnim;
    int numRocketsToFireAtPaddle;
    double glitchSummonCountdown;
    AnimationLerp<float> spinningDestroyConfinesAnim;
    double destroyConfinesCountdown;
    double chargeDestroyerBlastCountdown;

    int GenerateNumRocketsToFire() const { return 3 + Randomizer::GetInstance()->RandomUnsignedInt() % 3; }
    
    AnimationMultiLerp<float> GenerateHitOnBottomTiltAnim(const Point2D& hitPos) const;
    AnimationMultiLerp<Vector3D> GenerateBottomDeathTranslationAnimation(bool fallLeft) const;
    AnimationMultiLerp<float> GenerateBottomDeathRotationAnimation(bool fallLeft) const;

    void BlowUpLegs();

    // Inherited from ConfinedAI/GothicRomanticBossAI/BossAIState
    void SetState(GothicRomanticBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    void SetupNextAttackStateAndMove(GameModel* gameModel);
    float GetTotalLifePercent() const;
    float GenerateSpecialItemSummonProbability() const { return 1.0f; } // Always drop the special item
    float GetRocketCannonAtkProbabilityMultiplier() const { return 1.0f; }
    float GetRocketPaddleAtkProbabilityMultiplier() const { return 0.5f; }
    GameItem::ItemType GetSpecialItemDropType() const { return GameItem::IceBallItem; }

    // State methods
    void ExecuteRocketToPaddleAttackState(double dT, GameModel* gameModel);
    void ExecuteRocketToCannonAndPaddleAttackState(double dT, GameModel* gameModel);
    void ExecuteGlitchState(double dT, GameModel* gameModel);
    void ExecuteHurtBottomState(double dT, GameModel* gameModel);
    void ExecuteVeryHurtAndAngryState(double dT, GameModel* gameModel);
    void ExecuteMoveToCenterState(double dT, GameModel* gameModel);
    void ExecuteDestroyConfinesState(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(IceBallAI);
};


class FreeMovingAttackAI : public GothicRomanticBossAI {
public:
    FreeMovingAttackAI(GothicRomanticBoss* boss);
    ~FreeMovingAttackAI();

    bool IsStateMachineFinished() const;

    void CollisionOccurred(GameModel*, GameBall& ball, BossBodyPart*) {
        this->DoCollisionOccurredStuff(ball.GetCenterPosition2D());
    }
    void CollisionOccurred(GameModel*, Projectile* projectile, BossBodyPart*) {
        this->DoCollisionOccurredStuff(projectile->GetPosition());
    }
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {}

private:
    static const int NUM_BALL_HITS = 5;
    static const float BODY_LIFE_POINTS;
    static const float MAX_SPEED;

    BossWeakpoint* bodyWeakpt;

    // State variables
    AnimationMultiLerp<Vector2D> glitchShakeAnim;
    int glitchCounter;
    int numItemsPerSummoning;
    double moveToNextStateCountdown;
    double summonItemsDelayCountdown;
    double shootCountdown;
    double glitchSummonCountdown;
    int numRocketsToFire;
    std::vector<Point2D> rocketTargetPositions;
    AnimationMultiLerp<Vector3D> hurtMoveAnim;
    AnimationLerp<float> twirlAnim;

    double GenerateBasicMoveAndShootTime() const { return 8.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 5.0; }
    double GenerateTimeBetweenLaserShots() const { return 1.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.5; }
    double GenerateTimeBetweenRockets() const { return 1.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 1.0; }
    int GenerateNumRocketsToFire() const { return 4 + Randomizer::GetInstance()->RandomUnsignedInt() % 4; }
    void SetNextAttackState();
    void DoCollisionOccurredStuff(const Point2D& collisionPos);

    // Inherited from GothicRomanticBossAI/BossAIState
    void SetState(GothicRomanticBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    float GetTotalLifePercent() const;

    // State methods
    void ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel);
    void ExecuteSummonItemsState(double dT, GameModel* gameModel);
    void ExecuteRocketToCannonAndPaddleAttackState(double dT, GameModel* gameModel);
    void ExecuteGlitchState(double dT, GameModel* gameModel);
    void ExecuteHurtBodyState(double dT, GameModel* gameModel);
    void ExecuteFinalDeathThroesState();

    void PerformBasicMovement(const Point2D& pos, const GameLevel* level);
    void UpdateBasicMovement();
    void ExecuteOrbSpray(GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(FreeMovingAttackAI);
};

}; // namespace gothicromanticbossai

#endif // __GOTHICROMANTICBOSSAISTATES_H__