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
    GothicRomanticBoss* boss;

    // State-related variables
    enum AIState { BasicMoveAndShootState, SpinLaserAttackState, 
                   SummonItemsAIState, OrbProjectileAttackAIState,
                   HurtTopAIState, HurtBottomAIState, GlitchAIState,
                   MoveToCenterOfLevelAIState, DestroyConfinesAIState,
                   FinalDeathThroesAIState};

    AIState currState;

    float GetAccelerationMagnitude() const;
    virtual void SetState(GothicRomanticBossAI::AIState newState) = 0;

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

    // Inherited from GothicRomanticBossAI/BossAIState
    void SetState(GothicRomanticBossAI::AIState newState);
    void UpdateState(double dT, GameModel* gameModel);

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