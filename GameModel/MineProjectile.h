/**
 * MineProjectile.h
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

#ifndef __MINEPROJECTILE_H__
#define __MINEPROJECTILE_H__

#include "Projectile.h"
#include "SafetyNet.h"
#include "LevelPiece.h"
#include "PlayerPaddle.h"

class CannonBlock;

/**
 * Projectile for a mine launched from the player paddle when the player acquires a mine launcher paddle item.
 */
class MineProjectile : public Projectile {
public:
	static const float HEIGHT_DEFAULT;
	static const float WIDTH_DEFAULT;

    static const double MINE_MIN_COUNTDOWN_TIME;
    static const double MINE_MAX_COUNTDOWN_TIME;

    static const double MAX_TIME_WHEN_LANDED_UNTIL_ARMED;

    static const float MAX_VELOCITY;

	MineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height);
	MineProjectile(const MineProjectile& copy);
    virtual ~MineProjectile();
    
    void Land(const Point2D& landingPt);

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

    virtual float GetDamage() const = 0;

    virtual float GetProximityRadius() const { return std::min<float>(MINE_DEFAULT_PROXIMITY_RADIUS * this->GetVisualScaleFactor(), 2*LevelPiece::PIECE_WIDTH); }

    bool IsRocket() const { return false; }
    bool IsMine() const { return true; }
    bool IsRefractableOrReflectable() const { return false; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return false;  }

    bool ModifyLevelUpdate(double dT, GameModel& model);

    void SafetyNetCollisionOccurred(SafetyNet* safetyNet);
    void LevelPieceCollisionOccurred(LevelPiece* block);
    void PaddleCollisionOccurred(PlayerPaddle* paddle);
    void BossCollisionOccurred(Boss* boss, BossBodyPart* bossPart);

    void DetachFromPaddle();

	void LoadIntoCannonBlock(CannonBlock* cannonBlock);
    bool IsLoadedInCannonBlock() const { return this->cannonBlock != NULL; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    void SetAsFalling();

    float GetAccelerationMagnitude() const { return this->acceleration; }
    float GetRotationAccelerationMagnitude() const { return 300.0f; }

    float GetMaxVelocityMagnitude() const { return MineProjectile::MAX_VELOCITY; }
    float GetMaxRotationVelocityMagnitude() const { return 168.0f; }

    float GetDefaultHeight() const { return MineProjectile::HEIGHT_DEFAULT; };
    float GetDefaultWidth() const  { return MineProjectile::WIDTH_DEFAULT;  };

    float GetCurrentRotation() const { return this->currRotation; }

    float GetExplosionRadius() const { return MINE_DEFAULT_EXPLOSION_RADIUS * this->GetVisualScaleFactor(); }
    float GetProximityCountdownInSeconds() const { return this->proximityAlertCountdown; }

    bool GetIsArmed() const { return this->isArmed; }
    bool GetIsProximityAlerted() const { return this->proximityAlerted; }

    bool GetIsAttachedToSafetyNet() const { return this->attachedToNet != NULL; }
    bool GetIsAttachedToLevelPiece() const { return this->attachedToPiece != NULL; }
    bool GetIsAttachedToPaddle() const { return this->attachedToPaddle != NULL; }
    bool GetIsAttachedToBoss() const { return this->attachedToBoss != NULL; }

    bool IsAttachedToSomething() const {
        return this->GetIsAttachedToSafetyNet() || this->GetIsAttachedToLevelPiece() ||
            this->GetIsAttachedToPaddle() || this->GetIsAttachedToBoss();
    }

    void DestroyWithoutExplosion();

protected:
	static const Vector2D MINE_DEFAULT_VELOCITYDIR;
	static const Vector2D MINE_DEFAULT_RIGHTDIR;

    static const float MINE_DEFAULT_ACCEL;

    static const float MINE_DEFAULT_EXPLOSION_RADIUS;
    static const float MINE_DEFAULT_PROXIMITY_RADIUS;

private:
    // When the mine is loaded into a cannon block this will not be NULL
	CannonBlock* cannonBlock;

    // When the mine is attached to other objects one of these will not be NULL
    LevelPiece* attachedToPiece;
    SafetyNet* attachedToNet;
    PlayerPaddle* attachedToPaddle;
    Boss* attachedToBoss;

    // Extra Mine kinematics
    float acceleration;

	float currRotationSpd;
	float currRotation;

    // Whether the mine has attached/latched on to something and ready to blow up yet
    bool isArmed;
    bool isFalling;
    bool proximityAlerted;
    float proximityAlertCountdown;

    // Timer that keeps track of how long a mine has been 'landed' for, used
    // to ensure that mines eventually blow up if they're never triggered by proximity
    double landedTimeCounter;

    void BeginProximityExplosionCountdown();
    void StopAndResetProximityExplosionCountdown();
    void DetachFromAnyAttachedObject();
};

inline void MineProjectile::SetAsFalling() {
    this->SetVelocity(Vector2D(0, -1), 0.0f);
    this->acceleration = 9.8f;
    this->isFalling = true;
    this->DetachFromAnyAttachedObject();
}

inline void MineProjectile::DestroyWithoutExplosion() {
    this->SetVelocity(Vector2D(0, 0), 0.0f);
    this->acceleration      = 0.0f;
    this->currRotationSpd   = 0.0f;
    this->isArmed = false;
    this->isFalling = false;
    this->proximityAlerted = false;
    this->proximityAlertCountdown = 0.0;
    this->DetachFromAnyAttachedObject();

    // The next time that ModifyLevelUpdate is called on this mine it will be removed from the game
}

#endif // __PADDLEMINEPROJECTILE_H__