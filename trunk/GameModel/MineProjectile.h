/**
 * MineProjectile.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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

    virtual float GetProximityRadius() const { return MINE_DEFAULT_PROXIMITY_RADIUS * this->GetVisualScaleFactor(); }

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
        return this->GetIsAttachedToSafetyNet()  ||
               this->GetIsAttachedToLevelPiece() ||
               this->GetIsAttachedToPaddle() ||
               this->GetIsAttachedToBoss();
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

inline void MineProjectile::Land(const Point2D& landingPt) {
    this->SetVelocity(Vector2D(0, 0), 0.0f);
    this->acceleration      = 0.0f;
    this->currRotationSpd   = 0.0f;
    this->landedTimeCounter = 0.0;
    this->SetPosition(landingPt);
    this->isArmed = true;
    this->isFalling = false;
    this->StopAndResetProximityExplosionCountdown();
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