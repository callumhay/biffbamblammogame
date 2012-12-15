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

class BossBodyPart;

class Boss {
public:
    Boss();
    virtual ~Boss();

    virtual void Tick(double dT);

	virtual bool DoCollision(const GameBall& ball, double dT, Vector2D& n,
        Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
	virtual bool DoCollision(const Collision::Ray2D& ray, float& rayT);
	virtual bool DoCollision(const BoundingLines& boundingLines, const Vector2D& velDir);
	virtual bool DoCollision(const Collision::Circle2D& c, const Vector2D& velDir);


    virtual bool GetIsDead() const = 0;

    





protected:
    std::vector<BossBodyPart*> bodyParts;

private:
    DISALLOW_COPY_AND_ASSIGN(Boss);
};

#endif // __BOSS_H__