/**
 * GothicRomanticBossAIStates.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GOTHICROMANTICBOSSAISTATES_H__
#define __GOTHICROMANTICBOSSAISTATES_H__

#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"

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

    GothicRomanticBoss* boss;

    // State-related variables
    enum AIState { BasicMoveAndShootState, SpinLaserAttackAIState, 
                   SummonItemsAIState, OrbProjectileAttackAIState,
                   HurtTopAIState, HurtBottomAIState, GlitchAIState,
                   MoveToCenterOfLevelAIState, DestroyConfinesAIState,
                   FinalDeathThroesAIState};

    AIState currState;

    // Movement positions while confined
    enum ConfinedMovePos { TopLeftCorner = 0, TopRightCorner = 1, BottomLeftCorner = 2, BottomRightCorner = 3, Center = 4 };
    static const ConfinedMovePos CORNER_POSITIONS[];

    Point2D GetConfinedBossCenterMovePosition(float levelWidth, float levelHeight, const ConfinedMovePos& pos) const;
    static ConfinedMovePos GetRandomConfinedMovePosition(const ConfinedMovePos& currPos) {
        return static_cast<ConfinedMovePos>((currPos + (1 + Randomizer::GetInstance()->RandomUnsignedInt() % 4)) % 5);
    }
    static ConfinedMovePos GetRandomConfinedCornerMovePosition(const ConfinedMovePos& currPos);
    static AIState GetAIStateForConfinedMovePos(const ConfinedMovePos& pos, float probabilityOfOrb);

    float GetSlowestConfinedMoveSpeed() const { return LevelPiece::PIECE_WIDTH; }
    double GetLaserSpinTime() const { return 5.0 + Randomizer::GetInstance()->RandomNumZeroToOne() * 4.0; }

    float GetAccelerationMagnitude() const;
    virtual void SetState(GothicRomanticBossAI::AIState newState) = 0;

    void ShootLaserFromLegPoint(int legIdx, GameModel* gameModel) const;
    void ShootLaserFromBody(GameModel* gameModel) const;

private:
    DISALLOW_COPY_AND_ASSIGN(GothicRomanticBossAI);
};


class FireBallAI : public GothicRomanticBossAI {
public:
    FireBallAI(GothicRomanticBoss* boss);
    ~FireBallAI();
    
    void CollisionOccurred(GameModel*, GameBall&, BossBodyPart*) {};
    void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    void CollisionOccurred(GameModel*, PlayerPaddle&, BossBodyPart*) {};

private:
    static const int NUM_OF_TOP_POINT_HITS = 5; // Number of times the weakpoint has to be hit before we move to the next state
    static const float TOP_POINT_LIFE_POINTS;

    BossWeakpoint* topPointWeakpt;

    // State variables
    ConfinedMovePos nextMovePos;
    AIState nextAtkAIState;

    int attacksSinceLastSummon;
    double shootCountdown;
    AnimationLerp<float> laserSpinRotAnim;

    double GenerateShootCountdownAmtForMoving()   const { return 0.2 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.25; }
    double GenerateShootCountdownAmtForSpinning() const { return 0.075 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.2; }
    float GenerateSummonProbability() const;
    float GenerateOrbAttackProbability() const { return NumberFuncs::LerpOverFloat<float>(0.0f, 1.0f, 0.33f, 0.66f, this->GetTotalLifePercent()); }

    void SetupNextAttackStateAndMove();

    // Inherited from GothicRomanticBossAI/BossAIState
    void SetState(GothicRomanticBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);
    float GetTotalLifePercent() const;

    // State methods
    void ExecuteBasicMoveAndShootState(double dT, GameModel* gameModel);
    void ExecuteSpinLaserAttackState(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(FireBallAI);
};

/*
class IceBallAI {
};

class FreeMovingAttackAI {
public:
    //bool IsStateMachineFinished() const { return ; }
private:

};
*/

}; // namespace gothicromanticbossai

#endif // __GOTHICROMANTICBOSSAISTATES_H__