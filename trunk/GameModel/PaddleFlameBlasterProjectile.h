/**
 * PaddleFlameBlasterProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEFLAMEBLASTERPROJECTILE_H__
#define __PADDLEFLAMEBLASTERPROJECTILE_H__

#include "Projectile.h"

class PlayerPaddle;
class CannonBlock;

class PaddleFlameBlasterProjectile : public Projectile {
public:
    static const float DIST_FROM_TOP_OF_PADDLE_TO_FLAME;
    static const float DEFAULT_SIZE;
    static const float DEFAULT_VELOCITY_MAG;

    PaddleFlameBlasterProjectile(const PlayerPaddle& firingPaddle);
    PaddleFlameBlasterProjectile(const PaddleFlameBlasterProjectile& copy);
    ~PaddleFlameBlasterProjectile();

    bool IsRocket() const { return false; }
    bool IsMine() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

    void Tick(double seconds, const GameModel& model);
    BoundingLines BuildBoundingLines() const;

    ProjectileType GetType() const {
        return Projectile::PaddleFlameBlastProjectile;
    }

    float GetSizeMultiplier() const { return this->GetWidth() / DEFAULT_SIZE; }
    float GetDamage() const { return this->GetSizeMultiplier() * 30.0f; }

    void LoadIntoCannonBlock(CannonBlock* cannonBlock);
    bool IsLoadedInCannonBlock() const { return this->cannonBlock != NULL; }

private:
    // When the blast is loaded into a cannon block this will not be NULL
    CannonBlock* cannonBlock;

    // Disallow assignment
    void operator=(const PaddleFlameBlasterProjectile& copy);
};

#endif // __PADDLEFLAMEBLASTERPROJECTILE_H__