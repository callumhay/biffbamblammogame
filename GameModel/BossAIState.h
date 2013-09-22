/**
 * BossAIState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOSSAISTATE_H__
#define __BOSSAISTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Vector.h"

class GameModel;
class BossBodyPart;
class GameBall;
class Projectile;
class PlayerPaddle;
class Boss;
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