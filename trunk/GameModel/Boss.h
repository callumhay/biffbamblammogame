/**
 * Boss.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSS_H__
#define __BOSS_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"

#include "BoundingLines.h"
#include "GameBall.h"
#include "PlayerPaddle.h"
#include "GameWorld.h"
#include "BossBodyPart.h"
#include "BossCompositeBodyPart.h"

class BossAIState;

class Boss {
public:
    virtual ~Boss();

    static Boss* BuildStyleBoss(const GameWorld::WorldStyle& style);

	BossBodyPart* CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	BossBodyPart* CollisionCheck(const Collision::Ray2D& ray, float& rayT) const;
	BossBodyPart* CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const;
	BossBodyPart* CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const;

    void Tick(double dT, GameModel* gameModel);

    virtual bool GetIsDead() const = 0;

	virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart);
	virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart);
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart);

    // DEBUGGING...
#ifdef _DEBUG
    void DebugDraw() const;
#endif

protected:
    BossAIState* currAIState;
    BossAIState* nextAIState;

    BossCompositeBodyPart* root;

    std::vector<AbstractBossBodyPart*> bodyParts;

    Boss();
    void SetNextAIState(BossAIState* nextState);

    virtual void Init() = 0;

private:
    void UpdateAIState();
    void SetCurrentAIStateImmediately(BossAIState* newState);

    DISALLOW_COPY_AND_ASSIGN(Boss);
};

inline BossBodyPart* Boss::CollisionCheck(const GameBall& ball, double dT, Vector2D& n,
                                          Collision::LineSeg2D& collisionLine,
                                          double& timeSinceCollision) const {

    return this->root->CollisionCheck(ball, dT, n, collisionLine, timeSinceCollision);
}

inline BossBodyPart* Boss::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
    return this->root->CollisionCheck(ray, rayT);
}

inline BossBodyPart* Boss::CollisionCheck(const BoundingLines& boundingLines, const Vector2D& velDir) const {
    return this->root->CollisionCheck(boundingLines, velDir);
}

inline BossBodyPart* Boss::CollisionCheck(const Collision::Circle2D& c, const Vector2D& velDir) const {
    return this->root->CollisionCheck(c, velDir);
}

inline void Boss::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, ball);
}

inline void Boss::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, projectile);
}

inline void Boss::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, paddle);
}

#endif // __BOSS_H__