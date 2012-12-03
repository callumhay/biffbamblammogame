/**
 * MineProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
    static const double MINE_MIN_COUNTDOWN_TIME;
    static const double MINE_MAX_COUNTDOWN_TIME;

    static const float MAX_VELOCITY;

	MineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height);
	MineProjectile(const MineProjectile& copy);
    virtual ~MineProjectile();
    
    void Land(const Point2D& landingPt);

	void Tick(double seconds, const GameModel& model);
	BoundingLines BuildBoundingLines() const;

    virtual float GetDamage() const = 0;

    bool IsRocket() const { return false; }
    bool IsRefractableOrReflectable() const { return false; }

    bool BlastsThroughSafetyNets() const { return false; }
    bool IsDestroyedBySafetyNets() const { return false;  }

    bool ModifyLevelUpdate(double dT, GameModel& model);

    void SafetyNetCollisionOccurred(SafetyNet* safetyNet);
    void LevelPieceCollisionOccurred(LevelPiece* block);
    void PaddleCollisionOccurred(PlayerPaddle* paddle);

    void DetachFromPaddle();

	void LoadIntoCannonBlock(CannonBlock* cannonBlock);
    bool IsLoadedInCannonBlock() const { return this->cannonBlock != NULL; }

    float GetVisualScaleFactor() const { return this->GetWidth() / this->GetDefaultWidth(); }
    
    void SetAsFalling();

    float GetAccelerationMagnitude() const { return this->acceleration; }
    float GetRotationAccelerationMagnitude() const { return 300.0f; }

    float GetMaxVelocityMagnitude() const { return MAX_VELOCITY; }
    float GetMaxRotationVelocityMagnitude() const { return 168.0f; }

    virtual float GetDefaultHeight() const = 0;
    virtual float GetDefaultWidth() const  = 0;

    float GetCurrentRotation() const { return this->currRotation; }

    float GetExplosionRadius() const { return MINE_DEFAULT_EXPLOSION_RADIUS * this->GetVisualScaleFactor(); }
    float GetProximityRadius() const { return MINE_DEFAULT_PROXIMITY_RADIUS * this->GetVisualScaleFactor(); }
    float GetProximityCountdownInSeconds() const { return this->proximityAlertCountdown; }

    bool GetIsArmed() const { return this->isArmed; }
    bool GetIsProximityAlerted() const { return this->proximityAlerted; }

    bool GetIsAttachedToSafetyNet() const { return this->attachedToNet != NULL; }
    bool GetIsAttachedToLevelPiece() const { return this->attachedToPiece != NULL; }
    bool GetIsAttachedToPaddle() const { return this->attachedToPaddle != NULL; }

    bool IsAttachedToSomething() const {
        return this->GetIsAttachedToSafetyNet()  ||
               this->GetIsAttachedToLevelPiece() ||
               this->GetIsAttachedToPaddle();
    }

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

    // Extra Mine kinematics
    float acceleration;

	float currRotationSpd;
	float currRotation;

    // Whether the mine has attached/latched on to something and ready to blow up yet
    bool isArmed;
    bool isFalling;
    bool proximityAlerted;
    float proximityAlertCountdown;

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

inline void MineProjectile::Land(const Point2D& landingPt) {
    this->SetVelocity(Vector2D(0, 0), 0.0f);
    this->acceleration    = 0.0f;
    this->currRotationSpd = 0.0f;
    this->SetPosition(landingPt);
    this->isArmed = true;
    this->isFalling = false;
    this->StopAndResetProximityExplosionCountdown();
}

inline void MineProjectile::SafetyNetCollisionOccurred(SafetyNet* safetyNet) {
    assert(safetyNet != NULL);

    // If the mine is already attached to the safety net then ignore this
    if (this->attachedToNet == safetyNet) {
        return;
    }

    this->Land(safetyNet->GetBounds().ClosestPoint(this->GetPosition()) + Vector2D(0, SafetyNet::SAFETY_NET_HEIGHT / 2.0f));
    Projectile::SafetyNetCollisionOccurred(safetyNet);
    safetyNet->AttachProjectile(this);
    this->attachedToNet = safetyNet;
}

inline void MineProjectile::LevelPieceCollisionOccurred(LevelPiece* block) {
    assert(block != NULL);

    // If the mine is already attached to a piece then ignore this
    if (this->attachedToPiece != NULL) {
        return;
    }

    // We don't 'land' the mine and set an attached block if the mine is being loaded into a cannon
    // or if it just can't collide with the block
    if (block->GetType() == LevelPiece::Cannon || block->IsNoBoundsPieceType()) {
        return;
    }

    this->Land(block->GetBounds().ClosestPoint(this->GetPosition()));
    this->SetLastThingCollidedWith(block);
    block->AttachProjectile(this);
    this->attachedToPiece = block;
}

inline void MineProjectile::PaddleCollisionOccurred(PlayerPaddle* paddle) {
    assert(paddle != NULL);

    if (this->attachedToPaddle != NULL) {
        return;
    }

    this->Land(paddle->GetBounds().ClosestPoint(this->GetPosition()));
    this->SetLastThingCollidedWith(paddle);
    paddle->AttachProjectile(this);
    this->attachedToPaddle = paddle;
}

#endif // __PADDLEMINEPROJECTILE_H__