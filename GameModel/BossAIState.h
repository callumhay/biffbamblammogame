/**
 * BossAIState.h
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

#ifndef __BOSSAISTATE_H__
#define __BOSSAISTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Vector.h"

class GameSound;
class GameModel;
class BossBodyPart;
class GameBall;
class Projectile;
class PlayerPaddle;
class Boss;
class RocketProjectile;
class MineProjectile;
class TeslaBlock;

class BossAIState {
public:
    BossAIState();
    virtual ~BossAIState();

    void Tick(double dT, GameModel* gameModel);

    virtual Boss* GetBoss() const = 0;

	virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart)         = 0;
	virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) = 0;
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart)   = 0;
    
    virtual void RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket);
    virtual void MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine);
    virtual void TeslaLightningArcHitOccurred(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2);
    // TODO: Beams!!!

    virtual bool CanHurtPaddleWithBody() const = 0;
    virtual bool IsStateMachineFinished() const = 0;
    virtual Collision::AABB2D GenerateDyingAABB() const = 0;

    virtual void UpdateMovement(double dT);
    virtual void UpdateState(double dT, GameModel* gameModel) = 0;
    
    virtual float GetTotalLifePercent() const = 0;

protected:
    static const float DEFAULT_TARGET_ERROR;
    Vector2D currVel;
    Vector2D desiredVel;

    Vector2D GetAcceleration() const;
    virtual float GetAccelerationMagnitude() const = 0;

    // Special movement functionality to get the boss to move to an exact location
    void SetMoveToTargetPosition(const Point2D& startPos, const Point2D& targetPos, float targetError = DEFAULT_TARGET_ERROR);
    bool MoveToTargetPosition(float maxMoveSpd);

    // Misc. Helper Functions    
    static void GetFurthestDistFromBossPositions(const Point2D& currBossPos, const std::vector<Point2D>& positions, 
        int numPositionsToChoose, std::vector<Point2D>& result);

private:
    Point2D startPosition;   // The position the boss started in before its current movement
    Point2D targetPosition;  // The position the boss is moving towards
    float targetError;

    DISALLOW_COPY_AND_ASSIGN(BossAIState);
};

inline void BossAIState::Tick(double dT, GameModel* gameModel) {
    // Update the state of the AI
    this->UpdateState(dT, gameModel);
    // Update the bosses' movement
    this->UpdateMovement(dT);
}

inline void BossAIState::RocketExplosionOccurred(GameModel*, const RocketProjectile*) {
}

inline void BossAIState::MineExplosionOccurred(GameModel*, const MineProjectile*) {
}

inline void BossAIState::TeslaLightningArcHitOccurred(GameModel*, const TeslaBlock*, const TeslaBlock*) {
}

inline Vector2D BossAIState::GetAcceleration() const {
    Vector2D accel = this->desiredVel - this->currVel;
    if (accel.IsZero()) {
        return Vector2D(0,0);
    }
    accel.Normalize();
    return this->GetAccelerationMagnitude() * accel;
}

inline void BossAIState::SetMoveToTargetPosition(const Point2D& startPos, const Point2D& targetPos, float targetError) {
    this->startPosition  = startPos;
    this->targetPosition = targetPos;
    this->targetError = targetError;
}

#endif // __BOSSAISTATE_H__