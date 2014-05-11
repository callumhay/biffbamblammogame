/**
 * PaddleBlasterProjectile.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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
