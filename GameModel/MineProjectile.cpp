/**
 * MineProjectile.cpp
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

#include "MineProjectile.h"
#include "CannonBlock.h"
#include "GameEventManager.h"
#include "LevelPiece.h"
#include "GameModel.h"
#include "Boss.h"
#include "BossBodyPart.h"

const float MineProjectile::HEIGHT_DEFAULT = 0.8f;
const float MineProjectile::WIDTH_DEFAULT  = 0.8f;

const Vector2D MineProjectile::MINE_DEFAULT_VELOCITYDIR = Vector2D(0, 1);
const Vector2D MineProjectile::MINE_DEFAULT_RIGHTDIR    = Vector2D(1, 0);

const float MineProjectile::MINE_DEFAULT_ACCEL = 80.0f;

const float MineProjectile::MINE_DEFAULT_EXPLOSION_RADIUS = 1.00f * LevelPiece::PIECE_WIDTH;
const float MineProjectile::MINE_DEFAULT_PROXIMITY_RADIUS = 1.33f * LevelPiece::PIECE_WIDTH;

const double MineProjectile::MINE_MIN_COUNTDOWN_TIME = 1.75;
const double MineProjectile::MINE_MAX_COUNTDOWN_TIME = 3.75;

const double MineProjectile::MAX_TIME_WHEN_LANDED_UNTIL_ARMED = 30.0;

const float MineProjectile::MAX_VELOCITY = 12.0f;

MineProjectile::MineProjectile(const Point2D& spawnLoc, const Vector2D& velDir, float width, float height) :
Projectile(spawnLoc, width, height), cannonBlock(NULL), acceleration(MineProjectile::MINE_DEFAULT_ACCEL),
isArmed(false), isFalling(false), proximityAlerted(false),
proximityAlertCountdown(std::numeric_limits<float>::max()),
attachedToPiece(NULL), attachedToNet(NULL), attachedToPaddle(NULL), attachedToBoss(NULL), landedTimeCounter(0.0) {

    if (velDir.IsZero()) {
        assert(false);
        this->velocityDir = MineProjectile::MINE_DEFAULT_VELOCITYDIR;
        this->rightVec    = MineProjectile::MINE_DEFAULT_RIGHTDIR;
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

MineProjectile::MineProjectile(const MineProjectile& copy) : Projectile(copy),
cannonBlock(copy.cannonBlock), currRotationSpd(copy.currRotationSpd),
currRotation(copy.currRotation), acceleration(copy.acceleration), isArmed(copy.isArmed), isFalling(copy.isFalling),
proximityAlerted(copy.proximityAlerted), proximityAlertCountdown(copy.proximityAlertCountdown),
attachedToNet(copy.attachedToNet), attachedToPiece(copy.attachedToPiece), attachedToPaddle(copy.attachedToPaddle),
attachedToBoss(copy.attachedToBoss), landedTimeCounter(copy.landedTimeCounter) {

    assert(cannonBlock == NULL);
    assert(attachedToPiece == NULL);
    assert(attachedToNet == NULL);
    assert(attachedToPaddle == NULL);
    assert(attachedToBoss == NULL);
}

MineProjectile::~MineProjectile() {
    this->DetachFromAnyAttachedObject();
}

void MineProjectile::Land(const Point2D& landingPt) {
    this->SetVelocity(Vector2D(0, 0), 0.0f);
    this->acceleration      = 0.0f;
    this->currRotationSpd   = 0.0f;
    this->landedTimeCounter = 0.0;
    this->SetPosition(landingPt);
    this->isArmed = true;
    this->isFalling = false;
    this->StopAndResetProximityExplosionCountdown();

    // EVENT: Mine landed on something
    GameEventManager::Instance()->ActionMineLanded(*this);
}

void MineProjectile::Tick(double seconds, const GameModel& model) {

	if (this->cannonBlock == NULL) {

        // Make sure that there's a velocity or magnitude before doing all the calculations for those things...
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
        else if (this->isArmed && !this->proximityAlerted) { 
            // The mine has landed on something, don't let it sit there forever, have a timer for it to be armed after a particular time period... 
            this->landedTimeCounter += seconds;
            if (this->landedTimeCounter >= MineProjectile::MAX_TIME_WHEN_LANDED_UNTIL_ARMED) {
                this->BeginProximityExplosionCountdown();
            }
        }
	}
}

BoundingLines MineProjectile::BuildBoundingLines() const {
	// If the mine is inside a cannon block it has no bounding lines...
	if (this->IsLoadedInCannonBlock()) {
		return BoundingLines();
	}

	const Vector2D& UP_DIR    = MineProjectile::MINE_DEFAULT_VELOCITYDIR;
	const Vector2D& RIGHT_DIR = MineProjectile::MINE_DEFAULT_RIGHTDIR;

	Point2D topRight    = this->GetPosition() + this->GetHalfHeight()*UP_DIR + this->GetHalfWidth()*RIGHT_DIR;
	Point2D bottomRight = topRight - this->GetHeight()*UP_DIR;
	Point2D topLeft     = topRight - this->GetWidth()*RIGHT_DIR;
	Point2D bottomLeft  = topLeft - this->GetHeight()*UP_DIR;

    // Bounds are the outside square around the mine and an X from corner-to-corner
    // through the middle of it (for more robust collisions)

#define BUILD_BASIC_BOUNDS(b) \
    b[0] = Collision::LineSeg2D(topLeft, bottomLeft); \
    b[1] = Collision::LineSeg2D(topRight, bottomRight); \
    b[2] = Collision::LineSeg2D(topRight, topLeft); \
    b[3] = Collision::LineSeg2D(bottomRight, bottomLeft)

    const Vector2D& velDir = this->GetVelocityDirection();
    if (velDir.IsZero()) {
        static const int NUM_BOUNDS = 4;
        Collision::LineSeg2D bounds[NUM_BOUNDS];
        Vector2D norms[NUM_BOUNDS];
        
        BUILD_BASIC_BOUNDS(bounds);
        
        return BoundingLines(NUM_BOUNDS, bounds, norms);
    }
    else {
        Point2D midFront = this->GetPosition() + this->GetHalfHeight() * velDir;
        Point2D midBack  = midFront - this->GetHeight()*velDir;

        static const int NUM_BOUNDS = 5;
        Collision::LineSeg2D bounds[NUM_BOUNDS];
        Vector2D norms[NUM_BOUNDS];

        BUILD_BASIC_BOUNDS(bounds);
        bounds[4] = Collision::LineSeg2D(midFront, midBack);
        
        return BoundingLines(NUM_BOUNDS, bounds, norms);
    }
}

void MineProjectile::LoadIntoCannonBlock(CannonBlock* cannonBlock) {
	assert(cannonBlock != NULL);

	// When loaded into a cannon block the rocket stops moving and centers on the block...
	this->velocityMag  = 0.0f;
    this->acceleration = 0.0f;
	this->position    = cannonBlock->GetCenter();
	this->cannonBlock = cannonBlock;
    this->isArmed   = false;
    this->isFalling = false;
    this->landedTimeCounter = 0.0;

	// EVENT: The mine is officially loaded into the cannon block
	GameEventManager::Instance()->ActionProjectileEnteredCannon(*this, *cannonBlock);
}

/**
 * Update the level based on changes to this projectile that can result in level changes.
 * Return: true if this projectile has been destroyed/removed from the game, false otherwise.
 */
bool MineProjectile::ModifyLevelUpdate(double dT, GameModel& model) {

    if (this->proximityAlertCountdown <= 0.0) {
        // This mine has already exploded, this will only happen if the mine was told to
        // explicitly be removed without exploding via "MineProjectile::DestroyWithoutExplosion()"
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

        // Check whether the mine is inside a cannon block and whether we should fire it...
        if (this->cannonBlock != NULL) {
            // Start by doing a test to see if the cannon will actually fire...
            bool willCannonFire = this->cannonBlock->TestRotateAndFire(dT, false, this->velocityDir);
            if (willCannonFire) {
                // Set the remaining kinematics parameters of the projectile so that it is fired
                this->position = this->cannonBlock->GetCenter() + CannonBlock::HALF_CANNON_BARREL_LENGTH * this->velocityDir;
                this->velocityMag = this->GetMaxVelocityMagnitude();
                this->acceleration = 0.0;

                // NOTE: We must do this event before calling RotateAndFire since the cannon can be destroyed in the process
                // EVENT: Mine is being fired from the cannon
                GameEventManager::Instance()->ActionProjectileFiredFromCannon(*this, *this->cannonBlock);
            }

            // 'Tick' the cannon to spin the rocket around inside it... eventually the function will say
            // it has fired the rocket
            // WARNING: This function can destroy the cannon!
            std::pair<LevelPiece*, bool> cannonFiredPair = this->cannonBlock->RotateAndFire(dT, &model, false);

            if (willCannonFire) {
                assert(cannonFiredPair.second);
                this->cannonBlock = NULL;
                this->SetLastThingCollidedWith(cannonFiredPair.first);
            }
        }

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
                bool isBottomSafetyNet = model.IsBottomSafetyNet(this->attachedToNet);
                model.DestroySafetyNet(this->attachedToNet);
                this->attachedToNet = NULL;
                
                // EVENT: The mine just destroyed the safety net
                GameEventManager::Instance()->ActionBallSafetyNetDestroyed(*this, isBottomSafetyNet);
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
        const GameModel::ProjectileMap& projectileMap = model.GetActiveProjectiles();
        for (GameModel::ProjectileMapConstIter mapIter = projectileMap.begin(); mapIter != projectileMap.end(); ++mapIter) {
            
            const GameModel::ProjectileList& currProjectiles = mapIter->second;
            for (GameModel::ProjectileListConstIter iter = currProjectiles.begin(); iter != currProjectiles.end(); ++iter) {

                const Projectile* projectile = *iter;
                
                // Ignore other mines
                if (projectile->GetType() == Projectile::PaddleMineBulletProjectile ||
                    projectile->GetType() == Projectile::MineTurretBulletProjectile) {
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
    }

    return false;
}

void MineProjectile::SafetyNetCollisionOccurred(SafetyNet* safetyNet) {
    assert(safetyNet != NULL);

    // If the mine is already attached to the safety net then ignore this
    if (this->IsAttachedToSomething()) {
        return;
    }
    assert(this->attachedToNet != safetyNet);

    this->Land(safetyNet->GetBounds().ClosestPoint(this->GetPosition()) + Vector2D(0, SafetyNet::SAFETY_NET_HEIGHT / 2.0f));
    Projectile::SafetyNetCollisionOccurred(safetyNet);
    safetyNet->AttachProjectile(this);
    this->attachedToNet = safetyNet;
}

void MineProjectile::LevelPieceCollisionOccurred(LevelPiece* block) {
    assert(block != NULL);

    // If the mine is already attached to a piece then ignore this
    if (this->IsAttachedToSomething()) {
        return;
    }
    assert(this->attachedToPiece == NULL);

    // We don't 'land' the mine and set an attached block if the mine is being loaded into a cannon
    // or if it just can't collide with the block
    if (block->GetType() == LevelPiece::Cannon || block->GetType() == LevelPiece::FragileCannon || block->IsNoBoundsPieceType() ||
        (block->GetType() == LevelPiece::NoEntry && !block->HasStatus(LevelPiece::IceCubeStatus))) {

        return;
    }

    // We don't land the mine on blocks that mines destroy (turrets)
    if (block->GetType() == LevelPiece::LaserTurret ||
        block->GetType() == LevelPiece::RocketTurret ||
        block->GetType() == LevelPiece::MineTurret) {

        return;
    }

    this->Land(block->GetBounds().ClosestPoint(this->GetPosition()));
    this->SetLastThingCollidedWith(block);
    block->AttachProjectile(this);
    this->attachedToPiece = block;
}

void MineProjectile::PaddleCollisionOccurred(PlayerPaddle* paddle) {
    assert(paddle != NULL);

    if (this->IsAttachedToSomething()) {
        return;
    }
    assert(this->attachedToPaddle == NULL);

    // If the paddle has its shield activated then we don't land the mine
    if (!paddle->HasPaddleType(PlayerPaddle::ShieldPaddle)) {
        this->Land(paddle->GetBounds().ClosestPoint(this->GetPosition()));
        paddle->AttachProjectile(this);
        this->attachedToPaddle = paddle;
    }
    else {
        // We need to make sure the mine isn't falling anymore since it will be reflected off the shield
        this->isFalling = false;
    }

    this->SetLastThingCollidedWith(paddle);
}

void MineProjectile::BossCollisionOccurred(Boss* boss, BossBodyPart* bossPart) {
    assert(boss != NULL);

    if (this->IsAttachedToSomething() || bossPart->GetIsDestroyed()) {
        return;
    }
    assert(this->attachedToBoss == NULL);

    this->Land(bossPart->GetWorldBounds().ClosestPoint(this->GetPosition()));
    this->SetLastThingCollidedWith(bossPart);
    boss->AttachProjectile(this, bossPart);
    this->attachedToBoss = boss;
}

void MineProjectile::BeginProximityExplosionCountdown() {
    this->proximityAlerted = true;
    this->proximityAlertCountdown = MineProjectile::MINE_MIN_COUNTDOWN_TIME + Randomizer::GetInstance()->RandomNumZeroToOne() *
        (MineProjectile::MINE_MAX_COUNTDOWN_TIME - MineProjectile::MINE_MIN_COUNTDOWN_TIME);
}

void MineProjectile::StopAndResetProximityExplosionCountdown() {
    this->proximityAlerted = false;
    this->proximityAlertCountdown = std::numeric_limits<float>::max();
}

void MineProjectile::DetachFromPaddle() {
    if (this->attachedToPaddle != NULL) {
        this->attachedToPaddle->DetachProjectile(this);
        this->attachedToPaddle = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPiece == NULL);
        assert(this->attachedToBoss == NULL);
    }
}

void MineProjectile::DetachFromAnyAttachedObject() {
    if (this->attachedToNet != NULL) {
        this->attachedToNet->DetachProjectile(this);
        this->attachedToNet = NULL;
        assert(this->attachedToPiece == NULL);
    }
    if (this->attachedToPiece != NULL) {
        this->attachedToPiece->DetachProjectile(this);
        this->attachedToPiece = NULL;
        assert(this->attachedToNet == NULL);
    }
    if (this->attachedToPaddle != NULL) {
        this->attachedToPaddle->DetachProjectile(this);
        this->attachedToPaddle = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPiece == NULL);
    }
    if (this->attachedToBoss != NULL) {
        this->attachedToBoss->DetachProjectile(this);
        this->attachedToBoss = NULL;
        assert(this->attachedToNet == NULL);
        assert(this->attachedToPiece == NULL);
        assert(this->attachedToPaddle == NULL);
    }
}