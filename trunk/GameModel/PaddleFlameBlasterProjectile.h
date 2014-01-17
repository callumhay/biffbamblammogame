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

#include "PaddleBlasterProjectile.h"

class PlayerPaddle;
class CannonBlock;

class PaddleFlameBlasterProjectile : public PaddleBlasterProjectile {
public:
    static const float DEFAULT_SIZE;
    static const float DEFAULT_VELOCITY_MAG;

    explicit PaddleFlameBlasterProjectile(const PlayerPaddle& firingPaddle);
    PaddleFlameBlasterProjectile(const PaddleFlameBlasterProjectile& copy);
    ~PaddleFlameBlasterProjectile();

    ProjectileType GetType() const {
        return Projectile::PaddleFlameBlastProjectile;
    }

    float GetSizeMultiplier() const { return this->GetWidth() / DEFAULT_SIZE; }
    float GetDamage() const { return this->GetSizeMultiplier() * 30.0f; }

private:
    // Disallow assignment
    void operator=(const PaddleFlameBlasterProjectile& copy);
};

#endif // __PADDLEFLAMEBLASTERPROJECTILE_H__