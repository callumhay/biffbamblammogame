/**
 * PaddleBlasterProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEBLASTERPROJECTILE_H__
#define __PADDLEBLASTERPROJECTILE_H__

#include "Projectile.h"

class PlayerPaddle;
class CannonBlock;

class PaddleBlasterProjectile : public Projectile {
public:
    PaddleBlasterProjectile(const PaddleBlasterProjectile& copy);
    virtual ~PaddleBlasterProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

    void Tick(double seconds, const GameModel& model);
    bool ModifyLevelUpdate(double dT, GameModel& model);

    BoundingLines BuildBoundingLines() const;

    virtual float GetSizeMultiplier() const = 0;
    float GetDamage() const { return this->GetSizeMultiplier() * 30.0f; }

    void LoadIntoCannonBlock(CannonBlock* cannonBlock);
    bool IsLoadedInCannonBlock() const { return this->cannonBlock != NULL; }

    static float GetOriginDistanceFromTopOfPaddle(const PlayerPaddle& firingPaddle);

protected:
    static const float DEFAULT_DIST_FROM_TOP_OF_PADDLE_TO_PROJECTILE;

    PaddleBlasterProjectile(const PlayerPaddle& firingPaddle, float defaultSize, float defaultVelMag);

private:
    // When the blast is loaded into a cannon block this will not be NULL
    CannonBlock* cannonBlock;
    float defaultVelocityMagnitude;

    // Disallow assignment
    void operator=(const PaddleBlasterProjectile& copy);
};

#endif // __PADDLEBLASTERPROJECTILE_H__
