/**
 * PaddleIceBlasterProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEICEBLASTERPROJECTILE_H__
#define __PADDLEICEBLASTERPROJECTILE_H__

#include "PaddleBlasterProjectile.h"

class PlayerPaddle;
class CannonBlock;

class PaddleIceBlasterProjectile : public PaddleBlasterProjectile {
public:
    static const float DEFAULT_SIZE;
    static const float DEFAULT_VELOCITY_MAG;

    explicit PaddleIceBlasterProjectile(const PlayerPaddle& firingPaddle);
    PaddleIceBlasterProjectile(const PaddleIceBlasterProjectile& copy);
    ~PaddleIceBlasterProjectile();

    ProjectileType GetType() const {
        return Projectile::PaddleIceBlastProjectile;
    }

    float GetSizeMultiplier() const { return this->GetWidth() / DEFAULT_SIZE; }
    float GetDamage() const { return this->GetSizeMultiplier() * 30.0f; }

private:
    // Disallow assignment
    void operator=(const PaddleIceBlasterProjectile& copy);
};

#endif // __PADDLEICEBLASTERPROJECTILE_H__