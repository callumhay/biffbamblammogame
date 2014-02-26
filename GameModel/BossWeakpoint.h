/**
 * BossWeakpoint.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __BOSSWEAKPOINT_H__
#define __BOSSWEAKPOINT_H__

#include "BossBodyPart.h"

class BossWeakpoint : public BossBodyPart {
public:
    static const double DEFAULT_INVULNERABLE_TIME_IN_SECS;

    BossWeakpoint(float lifePoints, float dmgOnBallHit, const BoundingLines& bounds);
    ~BossWeakpoint();

    static BossWeakpoint* BuildWeakpoint(BossBodyPart* part, float lifePoints, float dmgOnBallHit);

    float GetCurrentLifePercentage() const;
    bool IsCurrentlyInvulnerable() const;
    void SetInvulnerableTime(double timeInSecs);
    double GetInvulnerableTime() const;
    void SetAsInvulnerable(double timeInSecs);

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
    double totalInvulnerableTime;

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

inline void BossWeakpoint::SetInvulnerableTime(double timeInSecs) {
    assert(timeInSecs >= 0.0);
    this->totalInvulnerableTime = timeInSecs;
}

inline double BossWeakpoint::GetInvulnerableTime() const {
    return this->totalInvulnerableTime;
}

inline void BossWeakpoint::SetAsInvulnerable(double timeInSecs) {
    assert(timeInSecs >= 0.0);
    this->invulnerableTimer = timeInSecs;
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