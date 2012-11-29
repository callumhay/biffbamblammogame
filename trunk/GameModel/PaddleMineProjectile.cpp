/**
 * PaddleMineProjectile.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PaddleMineProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"
#include "LevelPiece.h"
#include "GameModel.h"

const float PaddleMineProjectile::PADDLEMINE_HEIGHT_DEFAULT = 0.8f;
const float PaddleMineProjectile::PADDLEMINE_WIDTH_DEFAULT  = 0.8f;

const Vector2D PaddleMineProjectile::MINE_DEFAULT_VELOCITYDIR = Vector2D(0, 1);
const Vector2D PaddleMineProjectile::MINE_DEFAULT_RIGHTDIR    = Vector2D(1, 0);

const float PaddleMineProjectile::MINE_DEFAULT_ACCEL = 80.0f;

const float PaddleMineProjectile::MINE_DEFAULT_EXPLOSION_RADIUS = 0.9f * LevelPiece::PIECE_HEIGHT;
const float PaddleMineProjectile::MINE_DEFAULT_PROXIMITY_RADIUS = 1.00f * LevelPiece::PIECE_WIDTH;

const double PaddleMineProjectile::MINE_MIN_COUNTDOWN_TIME = 1.75;
const double PaddleMineProjectile::MINE_MAX_COUNTDOWN_TIME = 3.75;

const float PaddleMineProjectile::MAX_VELOCITY = 15.0f;

PaddleMineProjectile::PaddleMineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL), acceleration(MINE_DEFAULT_ACCEL), isArmed(false), isFalling(false),
proximityAlerted(false), proximityAlertCountdown(std::numeric_limits<float>::max()),
attachedToPiece(NULL), attachedToNet(NULL), attachedToPaddle(NULL) {

    if (velDir.IsZero()) {
        assert(false);
        this->velocityDir = MINE_DEFAULT_VELOCITYDIR;
        this->rightVec    = MINE_DEFAULT_RIGHTDIR;
    }
    else {
        this->velocityDir = velDir;
        this->rightVec    = Vector2D::Rotate(-90, velDir);
        this->velocityDir.Normalize();
        this->rightVec.Normalize();
    }

	this->velocityMag      = 0.0f;
	this->currRotation     = 0.0f;
	this->currRotationSpd  = 0.0f;
}

PaddleMineProjectile::PaddleMineProjectile(const PaddleMineProjectile& copy) : Projectile(copy),
cannonBlock(copy.cannonBlock), currRotationSpd(copy.currRotationSpd),
currRotation(copy.currRotation), acceleration(copy.acceleration), isArmed(copy.isArmed), isFalling(copy.isFalling),
proximityAlerted(copy.proximityAlerted), proximityAlertCountdown(copy.proximityAlertCountdown),
attachedToNet(copy.attachedToNet), attachedToPiece(copy.attachedToPiece), attachedToPaddle(copy.attachedToPaddle) {

    assert(cannonBlock == NULL);
    assert(attachedToPiece == NULL);
    assert(attachedToNet == NULL);
    assert(attachedToPaddle == NULL);
}

PaddleMineProjectile::~PaddleMineProjectile() {
    this->DetachFromAnyAttachedObject();
}

void PaddleMineProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock != NULL) {
		// 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
		// it has fired the rocket
		bool cannonHasFired = this->cannonBlock->RotateAndEventuallyFire(seconds);
		if (cannonHasFired) {
			// Set the velocity in the direction the cannon has fired in
            this->velocityMag = 0.25f * this->GetMaxVelocityMagnitude();
			this->velocityDir = this->cannonBlock->GetCurrentCannonDirection();
            this->acceleration = MINE_DEFAULT_ACCEL;
			this->position = this->position + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;

			// EVENT: Mine has officially been fired from the cannon
			GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);

			this->cannonBlock = NULL;
		}
	}
	else {
        // Make sure that there's a velocity or magnitude before doing all the calcuations
        // for those things...

        if (this->GetVelocityMagnitude() != 0.0f || this->GetAccelerationMagnitude() != 0.0f) {
            this->AugmentDirectionOnPaddleMagnet(seconds, model, 60.0f);

	        // Update the rocket's velocity and position
            float dA = seconds * this->GetAccelerationMagnitude();
            this->velocityMag = std::min<float>(this->GetMaxVelocityMagnitude(), this->velocityMag + dA);
	        float dV = seconds * this->velocityMag;
	        this->SetPosition(this->GetPosition() + dV * this->velocityDir);

            // Update the mine's rotation
            dA = seconds * this->GetRotationAccelerationMagnitude();
            this->currRotationSpd = std::min<float>(this->GetMaxRotationVelocityMagnitude(), this->currRotationSpd + dA);
		    dV = static_cast<float>(seconds * this->currRotationSpd);
		    this->currRotation += dV;
            this->currRotation = fmod(this->currRotation, 360.0f);
        }
	}
}

BoundingLines PaddleMineProjectile::BuildBoundingLines() const {
	// If the mine is inside a cannon block it has no bounding lines...
	if (this->IsLoadedInCannonBlock()) {
		return BoundingLines();
	}

	const Vector2D& UP_DIR    = PaddleMineProjectile::MINE_DEFAULT_VELOCITYDIR;
	const Vector2D& RIGHT_DIR = PaddleMineProjectile::MINE_DEFAULT_RIGHTDIR;

	Point2D topRight    = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
	Point2D topLeft     = topRight - this->GetWidth()*RIGHT_DIR;
	Point2D bottomLeft  = topLeft - this->GetHeight()*UP_DIR;

	std::vector<Collision::LineSeg2D> sideBounds;
	sideBounds.reserve(4);
	sideBounds.push_back(Collision::LineSeg2D(topLeft, bottomLeft));
	sideBounds.push_back(Collision::LineSeg2D(topRight, bottomRight));
	sideBounds.push_back(Collision::LineSeg2D(topRight, topLeft));
	sideBounds.push_back(Collision::LineSeg2D(bottomRight, bottomLeft));
	std::vector<Vector2D> normBounds;
	normBounds.resize(4);

	return BoundingLines(sideBounds, normBounds);
}

void PaddleMineProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag  = 0.0f;
    this->acceleration = 0.0f;
	this->position    = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;
    this->isArmed   = false;
    this->isFalling = false;

	// EVENT: The mine is officially loaded into the cannon block
	GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}

/**
 * Update the level based on changes to this projectile that can result in level changes.
 * Return: true if this projectile has been destroyed/removed from the game, false otherwise.
 */
bool PaddleMineProjectile::ModifyLevelUpdate(double dT, GameModel& model) {

    if (this->proximityAlertCountdown <= 0.0) {
        // This mine has already exploded, this should typically never happen, but just for robustness...
        assert(false);
        return true;
    }
    
    // Check to see if we are falling, if so then make sure we are falling with gravity...
    if (this->isFalling) {
        Vector3D gravityDir = model.GetGravityDir();
        this->SetVelocity(Vector2D(gravityDir[0], gravityDir[1]), this->GetVelocityMagnitude());
    }

    // The mine will only make modifications to the level (i.e., act as a mine and possibly explode)
    // when it is armed and not inside a cannon block, otherwise we just exit immediately
    if (!this->GetIsArmed() || this->IsLoadedInCannonBlock()) {
        return false;
    }

    // If the proximity alert has been triggered then we continue counting down towards mine detonation
    if (this->proximityAlerted) {
        this->proximityAlertCountdown -= dT;
        if (this->proximityAlertCountdown <= 0.0) {

            // Check to see if the paddle was caught in the explosion as well...
            Collision::Circle2D explosionBounds(this->GetPosition(), this->GetExplosionRadius());
            PlayerPaddle* paddle = model.GetPlayerPaddle();
            if (paddle->GetBounds().CollisionCheck(explosionBounds)) {
                paddle->HitByProjectile(&model, *this); // This function will call MineExplosion
            }
            else {
                // The mine will now officially Kaboom!
                model.GetCurrentLevel()->MineExplosion(&model, this);
            }

            // If this mine was attached to a safety net then the safety net should be destroyed...
            if (this->GetIsAttachedToSafetyNet()) {
                model.DestroySafetyNet();
                
                // EVENT: The mine just destroyed the safety net
                GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*this);
            }

            return true;
        }
    }
    else {
        // The mine is armed and explodes based on other active entity's proximity to it...
        // Check to see if any moving object is within the proximity of the mine...
        Collision::Circle2D proximityBound(this->GetPosition(), this->GetProximityRadius());

        // Check the ball(s)
        const std::list<GameBall*>& balls = model.GetGameBalls();
        for (std::list<GameBall*>::const_iterator iter = balls.begin(); iter != balls.end(); ++iter) {
            const GameBall* currBall = *iter;
            if (Collision::IsCollision(proximityBound, currBall->GetBounds())) {
                this->BeginProximityExplosionCountdown();
                return false;
            }
        }

        // Check the paddle
        const PlayerPaddle* paddle = model.GetPlayerPaddle();
        if (paddle->GetBounds().CollisionCheck(proximityBound)) {
            this->BeginProximityExplosionCountdown();
            return false;
        }

        // Check other projectiles
        const std::list<Projectile*>& projectiles = model.GetActiveProjectiles();
        for (std::list<Projectile*>::const_iterator iter = projectiles.begin(); iter != projectiles.end(); ++iter) {
            const Projectile* projectile = *iter;
            
            // Ignore other mines
            if (projectile->GetType() == Projectile::PaddleMineBulletProjectile) {
                continue;
            }

            // Ignore light beams / lasers
            if (projectile->IsRefractableOrReflectable()) {
                continue;
            }

            // Check for a collision with the projectile...
            if (projectile->BuildBoundingLines().CollisionCheck(proximityBound)) {
                this->BeginProximityExplosionCountdown();
            }
        }
    }

    return false;
}

void PaddleMineProjectile::BeginProximityExplosionCountdown() {
    this->proximityAlerted = true;
    this->proximityAlertCountdown = MINE_MIN_COUNTDOWN_TIME + Randomizer::GetInstance()->RandomNumZeroToOne() *
        (MINE_MAX_COUNTDOWN_TIME - MINE_MIN_COUNTDOWN_TIME);
}

void PaddleMineProjectile::StopAndResetProximityExplosionCountdown() {
    this->proximityAlerted = false;
    this->proximityAlertCountdown = std::numeric_limits<float>::max();
}

void PaddleMineProjectile::DetachFromPaddle() {
    if (this->attachedToPaddle != NULL) {
        this->attachedToPaddle->DetachProjectile(this);
        this->attachedToPaddle = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPiece == NULL);
    }
}

void PaddleMineProjectile::DetachFromAnyAttachedObject() {
    if (this->attachedToNet != NULL) {
        this->attachedToNet->DetachProjectile(this);
        this->attachedToNet = NULL;
        assert(this->attachedToPiece == NULL);
        assert(this->attachedToPaddle == NULL);
    }
    if (this->attachedToPiece != NULL) {
        this->attachedToPiece->DetachProjectile(this);
        this->attachedToPiece = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPaddle == NULL);
    }
    if (this->attachedToPaddle != NULL) {
        this->attachedToPaddle->DetachProjectile(this);
        this->attachedToPaddle = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPiece == NULL);
    }
}