/**
 * BossWeakpoint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSWEAKPOINT_H__
#define __BOSSWEAKPOINT_H__

#include "BossBodyPart.h"


class BossWeakpoint : public BossBodyPart {
public:
    BossWeakpoint(float lifePoints, float dmgOnBallHit, const BoundingLines& bounds);
    ~BossWeakpoint();

    // Inherited functions from BossBodyPart
	void CollisionOccurred(GameModel* gameModel, GameBall& ball);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    //void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
    void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    bool GetIsDestroyed() const;
    
private:
    float currLifePoints;
    float dmgOnBallHit;
    
    void Diminish(float damageAmt, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(BossWeakpoint);
};

inline bool BossWeakpoint::GetIsDestroyed() const {
    return (this->currLifePoints <= 0);
}

#endif // __BOSSWEAKPOINT_H__