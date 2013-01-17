/**
 * BossAIState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BOSSAISTATE_H__
#define __BOSSAISTATE_H__

#include "../BlammoEngine/BasicIncludes.h"

class GameModel;
class BossBodyPart;
class GameBall;
class Projectile;
class PlayerPaddle;

class BossAIState {
public:
    BossAIState() {};
    virtual ~BossAIState() {};

    virtual void Tick(double dT, GameModel* gameModel) = 0;

	virtual void CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart)         = 0;
	virtual void CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) = 0;
    virtual void CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart)   = 0;

    virtual bool CanHurtPaddleWithBody() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(BossAIState);
};

#endif // __BOSSAISTATE_H__