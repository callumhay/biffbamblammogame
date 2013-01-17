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

    static BossWeakpoint* BuildWeakpoint(BossBodyPart* part, float lifePoints, float dmgOnBallHit);

    float GetCurrentLifePercentage() const;

    // Inherited functions from BossBodyPart
    void Tick(double dT);
	void CollisionOccurred(GameModel* gameModel, GameBall& ball);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    //void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
    void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    bool GetIsDestroyed() const;
    ColourRGBA GetColour() const;
    
private:
    AnimationMultiLerp<Colour> colourAnim; 
    const float totalLifePoints;
    float currLifePoints;
    float dmgOnBallHit;

    
    void Diminish(float damageAmt, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(BossWeakpoint);
};

inline float BossWeakpoint::GetCurrentLifePercentage() const {
    assert(this->currLifePoints <= this->totalLifePoints);
    return std::max<float>(0.0f, this->currLifePoints / this->totalLifePoints);
}

inline bool BossWeakpoint::GetIsDestroyed() const {
    return (this->currLifePoints <= 0);
}

inline ColourRGBA BossWeakpoint::GetColour() const {
    return ColourRGBA(this->colourAnim.GetInterpolantValue(), 1.0f);
}

#endif // __BOSSWEAKPOINT_H__