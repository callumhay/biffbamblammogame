/**
 * BossWeakpoint.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSWEAKPOINT_H__
#define __BOSSWEAKPOINT_H__

#include "BossBodyPart.h"

class BossWeakpoint : public BossBodyPart {
public:
    static const double INVULNERABLE_TIME_IN_SECS;

    BossWeakpoint(float lifePoints, float dmgOnBallHit, const BoundingLines& bounds);
    ~BossWeakpoint();

    static BossWeakpoint* BuildWeakpoint(BossBodyPart* part, float lifePoints, float dmgOnBallHit);

    float GetCurrentLifePercentage() const;
    bool IsCurrentlyInvulnerable() const;
    void ResetLife(float lifePoints);
    void ResetDamageOnBallHit(float dmgOnBallHit);

    // Inherited functions from BossBodyPart
    AbstractBossBodyPart::Type GetType() const { return AbstractBossBodyPart::WeakpointBodyPart; }
    void Tick(double dT);
	void CollisionOccurred(GameModel* gameModel, GameBall& ball);
	void CollisionOccurred(GameModel* gameModel, Projectile* projectile);
    //void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
    void TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
    void SetDestroyed(bool isDestroyed);

    void ColourAnimationFinished();
    void Diminish(float damageAmt);

private:
    float totalLifePoints;
    float currLifePoints;
    float dmgOnBallHit;

    double invulnerableTimer;

    void SetFlashingColourAnim();

    DISALLOW_COPY_AND_ASSIGN(BossWeakpoint);
};

/**
 * Return life percentage fraction in [0,1].
 */
inline float BossWeakpoint::GetCurrentLifePercentage() const {
    assert(this->currLifePoints <= this->totalLifePoints);
    return std::max<float>(0.0f, this->currLifePoints / this->totalLifePoints);
}

inline bool BossWeakpoint::IsCurrentlyInvulnerable() const {
    return this->invulnerableTimer > 0.0;
}

inline void BossWeakpoint::ResetLife(float lifePoints) {
    assert(lifePoints > 0);
    this->SetDestroyed(false);
    this->totalLifePoints = lifePoints;
    this->currLifePoints  = lifePoints;
    //this->SetFlashingColourAnim();
}

inline void BossWeakpoint::ResetDamageOnBallHit(float dmgOnBallHit) {
    this->dmgOnBallHit = dmgOnBallHit;
}

inline void BossWeakpoint::SetDestroyed(bool isDestroyed) {
    BossBodyPart::SetDestroyed(isDestroyed);
    if (isDestroyed) {
        this->currLifePoints = 0.0f;
    }
}

#endif // __BOSSWEAKPOINT_H__