/**
 * MineTurretBlock.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "MineTurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"
#include "MineTurretProjectile.h"
#include "PaddleMineProjectile.h"

const float MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X = 0.52f;
const float MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y = 0.0f;
const float MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z = 0.51f;

const float MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC  = 150.0f;
const float MineTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD = 300.0f;
const float MineTurretBlock::BARREL_RECOIL_TRANSLATION_AMT       = -0.22f;

const float MineTurretBlock::MINE_DISPLACEMENT_PRE_LOAD =
MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 0.6f  * MineTurretProjectile::HEIGHT_DEFAULT;
const float MineTurretBlock::MINE_DISPLACEMENT_ON_LOAD  =
MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 0.2f * MineTurretProjectile::HEIGHT_DEFAULT;

const float MineTurretBlock::FIRE_RATE_IN_MINES_PER_SEC = 0.33f;
const float MineTurretBlock::BARREL_RELOAD_TIME         = 0.7f  * (1.0f / MineTurretBlock::FIRE_RATE_IN_MINES_PER_SEC);
const float MineTurretBlock::BARREL_RECOIL_TIME         = 0.02f * (1.0f / MineTurretBlock::FIRE_RATE_IN_MINES_PER_SEC);
const float MineTurretBlock::MINE_LOAD_TIME             = 0.28f * (1.0f / MineTurretBlock::FIRE_RATE_IN_MINES_PER_SEC);

const double MineTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME = 90.0f  / MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
const double MineTurretBlock::LOST_AND_FOUND_MAX_SEEK_TIME = 120.0f / MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;

const int MineTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES = 3;
const int MineTurretBlock::LOST_AND_FOUND_MAX_NUM_LOOK_TIMES = 6;

#define PIECE_STARTING_LIFE_POINTS (5*GameModelConstants::GetInstance()->DEFAULT_DAMAGE_ON_BALL_HIT)

MineTurretBlock::MineTurretBlock(unsigned int wLoc, unsigned int hLoc) :
TurretBlock(wLoc, hLoc, PIECE_STARTING_LIFE_POINTS), 
currTurretState(SeekingTurretState), currRotationFromXInDegs(Randomizer::GetInstance()->RandomUnsignedInt() % 360), currRotationSpd(0.0f),
lostAndFoundTimeCounter(0.0), numSearchTimesCounter(0), numTimesToSearch(0),
currBarrelAxisRotation(Randomizer::GetInstance()->RandomUnsignedInt() % 360) {

    this->SetBarrelState(BarrelForwardMineLoaded, NULL);

    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->currRotationAccel = ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }
    else {
        this->currRotationAccel = -ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }

    this->colour = ColourRGBA(0.65f, 0.65f, 0.65f, 1.0f);
}

MineTurretBlock::~MineTurretBlock() {
}

// Determine whether the given projectile will pass through this block...
bool MineTurretBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        
            // When frozen, projectiles can pass through
            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                return true;
            }
            break;

        case Projectile::MineTurretBulletProjectile:
            if (projectile->IsLastThingCollidedWith(this)) {
                return true;
            }
            break;

        case Projectile::CollateralBlockProjectile:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * Get the number of points when this piece changes to the given piece.
 */
int MineTurretBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return MineTurretBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* MineTurretBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);

    LevelPiece* newPiece = this;
	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
	    case Projectile::LaserTurretBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {	
				// Laser bullets dimish the piece, but don't necessarily obliterated/destroy it
                newPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

        case Projectile::BossLightningBoltBulletProjectile:
            newPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::BasicProjectileDestruction);
            break;

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;


        case Projectile::BossRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile: {

			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}

            LevelPiece* tempPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::RocketDestruction);

            assert(dynamic_cast<const RocketProjectile*>(projectile) != NULL);
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<const RocketProjectile*>(projectile), tempPiece);

            assert(tempPiece == newPiece);
            break;
        }

        case Projectile::MineTurretBulletProjectile:
            // Deal with mines that have been fired from the barrel of this turret - they
            // shouldn't collide with this block!
            if (projectile->IsLastThingCollidedWith(this)) {
                break;
            }
        case Projectile::PaddleMineBulletProjectile: {
            
            // A mine will blow up on contact
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}

            LevelPiece* tempPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::MineDestruction);

            assert(dynamic_cast<const MineProjectile*>(projectile) != NULL);
            newPiece = gameModel->GetCurrentLevel()->MineExplosion(gameModel, static_cast<const MineProjectile*>(projectile), tempPiece);
            
            assert(tempPiece == newPiece);
            break;
        }

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a mine turret block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			this->LightPieceOnFire(gameModel, false);
			break;

        case Projectile::PaddleFlameBlastProjectile:
            this->LightPieceOnFire(gameModel, false);
            newPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::FireDestruction);
            break;

        case Projectile::PaddleIceBlastProjectile:
            this->FreezePieceInIce(gameModel);
            break;

		default:
			assert(false);
			break;
	}

	return newPiece;
}

void MineTurretBlock::AITick(double dT, GameModel* gameModel) {

    if (gameModel->GetCurrentStateType() == GameState::BallInPlayStateType &&
        !this->HasStatus(LevelPiece::IceCubeStatus)) {
        
        switch (this->currTurretState) {
            case IdleTurretState:
                // The turret should never be idle when the ball is in play, set it to start seeking out the paddle
                this->SetTurretState(SeekingTurretState);
                this->UpdateBarrelState(dT, this->ExecutePaddleSeekingAI(dT, gameModel), gameModel);
                break;

            case SeekingTurretState:
                this->UpdateBarrelState(dT, this->ExecutePaddleSeekingAI(dT, gameModel), gameModel);
                break;

            case TargetFoundTurretState:
                this->UpdateBarrelState(dT, this->ExecuteContinueTrackingAI(dT, gameModel), gameModel);
                break;

            case TargetLostTurretState:
                this->UpdateBarrelState(dT, this->ExecuteLostAndFoundAI(dT, gameModel), gameModel);
                break;

            default:
                assert(false);
                break;
        }
    }
    else  {
        // When the ball isn't in play we set it to the idle state
        if (this->currTurretState != IdleTurretState) {
            this->SetTurretState(IdleTurretState);
        }
    }
}

// Executes the AI for the turret attempting to find the paddle
// Returns: true if the turret may start to fire on the paddle, false if not
bool MineTurretBlock::ExecutePaddleSeekingAI(double dT, const GameModel* model) {
    //assert(this->currTurretState == SeekingTurretState);

    this->UpdateSpeed();
    float rotationAmt = dT * currRotationSpd;

    this->currRotationFromXInDegs += rotationAmt;
    this->currBarrelAxisRotation += rotationAmt;

    bool canSeePaddle, canFireAtPaddle;
    this->CanSeeAndFireAtPaddle(model, canSeePaddle, canFireAtPaddle);
    
    if (canSeePaddle) {
        this->SetTurretState(TargetFoundTurretState);
        return canFireAtPaddle;
    }

    return false;
}

// Executes the AI for continuing to track the player's paddle in order to shoot
// at it as much as possible.
// Returns: true if the turret still has a bead on the paddle, false if the turret
// has lost sight of the paddle and should no longer fire.
bool MineTurretBlock::ExecuteContinueTrackingAI(double dT, const GameModel* model) {
    assert(this->currTurretState == TargetFoundTurretState);

    const Point2D& paddlePos = model->GetPlayerPaddle()->GetCenterPosition();
    
    Vector2D fireDir;
    this->GetFiringDirection(fireDir);
    
    Vector2D blockToPaddleDir = paddlePos - this->GetCenter();
    Vector3D perpendicularDir = Vector3D::cross(fireDir, blockToPaddleDir);
    if (fabs(perpendicularDir[2]) < EPSILON) {
        // We don't need to adjust if the two vectors are the same
        // Check to see whether the paddle is still in view and can still be shot at
        return this->ContinueTrackingStateUpdateFromView(model);
    }

    blockToPaddleDir.Normalize();
    float requiredTackingRotation = fabs(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(fireDir, blockToPaddleDir)))));

    // Rotate towards the paddle...
    this->currRotationAccel = NumberFuncs::SignOf(perpendicularDir[2]) * MineTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    this->UpdateSpeed();
    float rotationAmt =  dT * currRotationSpd;
    if (fabs(rotationAmt) > requiredTackingRotation) {
        rotationAmt = NumberFuncs::SignOf(rotationAmt) * requiredTackingRotation; 
    }
    this->currRotationFromXInDegs += rotationAmt;

    // Check to see whether the paddle is still in view and can still be shot at
    return this->ContinueTrackingStateUpdateFromView(model);
}

// Function for centralizing code used by the ExecuteContinueTrackingAI method for
// changing the state of the turret AI based on whether or not the turret still sees the paddle.
// Returns: true if the paddle can be fired at, false if the paddle can't be fired at.
bool MineTurretBlock::ContinueTrackingStateUpdateFromView(const GameModel* model) {
    bool canSeePaddle, canFireAtPaddle;
    this->CanSeeAndFireAtPaddle(model, canSeePaddle, canFireAtPaddle);
    if (canSeePaddle) {
        return canFireAtPaddle;
    }

    this->SetTurretState(TargetLostTurretState);
    return false;
}

bool MineTurretBlock::ExecuteLostAndFoundAI(double dT, const GameModel* model) {
    assert(this->currTurretState == TargetLostTurretState);

    // Keep rotating in the direction we were already going... hopefully we'll find the
    // paddle again pretty quickly...
    
    this->lostAndFoundTimeCounter += dT;
    if (this->lostAndFoundTimeCounter > this->nextLostAndFoundSeekTime) {
        // The time for lost and found searching has expired...
        this->numSearchTimesCounter++;
        if (this->numSearchTimesCounter >= this->numTimesToSearch) {
            // Failed to find the paddle across all searches, go back to the seeking state
            this->SetTurretState(SeekingTurretState);
        }
        else {
            // Reverse the direction of the search...
            this->currRotationAccel = -NumberFuncs::SignOf(this->currRotationAccel) * MineTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
            this->lostAndFoundTimeCounter = 0.0;
            if (this->numSearchTimesCounter == 1) {
                this->nextLostAndFoundSeekTime = 2.0 * this->nextLostAndFoundSeekTime;
            }

        }
        return false;
    }

    return this->ExecutePaddleSeekingAI(dT, model);
}

void MineTurretBlock::SetTurretState(const TurretAIState& state) {
    TurretAIState newState = state;
    if (this->currTurretState == TargetLostTurretState && newState == TargetLostTurretState) {
        newState = SeekingTurretState;
    }

    if (newState == TargetLostTurretState) {
        this->lostAndFoundTimeCounter = 0.0;
        this->nextLostAndFoundSeekTime = MineTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME +
        Randomizer::GetInstance()->RandomNumZeroToOne() * (MineTurretBlock::LOST_AND_FOUND_MAX_SEEK_TIME - MineTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME);

        this->numSearchTimesCounter = 0;
        this->numTimesToSearch = MineTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + Randomizer::GetInstance()->RandomUnsignedInt() %
            (MineTurretBlock::LOST_AND_FOUND_MAX_NUM_LOOK_TIMES - MineTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + 1);
    }

    MineTurretBlock::TurretAIState prevState = this->currTurretState;
    this->currTurretState = newState;

    // EVENT: The AI state of the mine turret block has changed
    GameEventManager::Instance()->ActionMineTurretAIStateChanged(*this, prevState, this->currTurretState);
}

void MineTurretBlock::SetBarrelState(const BarrelAnimationState& state, GameModel* model) {

    switch (state) {

        case MineTurretBlock::BarrelForwardMineLoading:
            this->barrelMovementAnim.SetInterpolantValue(0.0f);
            this->barrelMovementAnim.ClearLerp();
            this->mineMovementAnim.SetLerp(0.0, MineTurretBlock::MINE_LOAD_TIME,
                MineTurretBlock::MINE_DISPLACEMENT_PRE_LOAD, MineTurretBlock::MINE_DISPLACEMENT_ON_LOAD);
            break;

        case MineTurretBlock::BarrelForwardMineLoaded:
            this->mineMovementAnim.SetInterpolantValue(MineTurretBlock::MINE_DISPLACEMENT_ON_LOAD);
            this->mineMovementAnim.ClearLerp();
            break;

        case MineTurretBlock::BarrelRecoiling: {
            assert(model != NULL);

            // Fire a mine from the barrel...
            Vector2D vec(MineTurretBlock::MINE_DISPLACEMENT_ON_LOAD, MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y);
            vec.Rotate(this->currRotationFromXInDegs);
            Point2D mineOrigin = this->GetCenter() + vec;
            this->GetFiringDirection(vec);

            MineTurretProjectile* turretProjectile = new MineTurretProjectile(mineOrigin, vec);
            turretProjectile->SetLastThingCollidedWith(this);
            model->AddProjectile(turretProjectile);

            // EVENT: The mine turret just fired a mine
            GameEventManager::Instance()->ActionMineFiredByTurret(*this);

            this->barrelMovementAnim.SetLerp(MineTurretBlock::BARREL_RECOIL_TIME, MineTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->mineMovementAnim.SetInterpolantValue(MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 1.05f*MineTurretProjectile::HEIGHT_DEFAULT);
            this->mineMovementAnim.ClearLerp();
            break;
        }

        case MineTurretBlock::BarrelBack:
            // The barrel has full recoil
            this->barrelMovementAnim.SetInterpolantValue(MineTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrelMovementAnim.ClearLerp();
            break;

        case MineTurretBlock::BarrelReloading: {
            this->barrelMovementAnim.SetLerp(MineTurretBlock::BARREL_RELOAD_TIME, 0.0f);
            break;
        }

        default:
            assert(false);
            return;
    }

    this->barrelState = state;
}

void MineTurretBlock::UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model) {
    switch (this->barrelState) {
        
        case MineTurretBlock::BarrelForwardMineLoading: {
            bool isFinished = this->mineMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelForwardMineLoaded, model);
            }
            break;
        }

        case MineTurretBlock::BarrelForwardMineLoaded:
            if (isAllowedToFire) {
                this->SetBarrelState(BarrelRecoiling, model);
            }
            break;

        case MineTurretBlock::BarrelRecoiling: {
            bool isFinished = this->barrelMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelBack, model);
            }
            break;
        }

        case MineTurretBlock::BarrelBack:
            this->SetBarrelState(BarrelReloading, model);
            break;

        case MineTurretBlock::BarrelReloading: {
            bool isFinished = this->barrelMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelForwardMineLoading, model);
            }
            break;
        }

        default:
            assert(false);
            return;
    }
}

void MineTurretBlock::GetBarrelInfo(Point3D& endOfBarrelPt, Vector2D& barrelDir) const {
    this->GetFiringDirection(barrelDir);
    endOfBarrelPt = Point3D(this->GetCenter() + MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X * barrelDir,
        MineTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z);
}

void MineTurretBlock::GetFiringDirection(Vector2D& unitDir) const {
    unitDir[0] = 1;
    unitDir[1] = 0;
    unitDir.Rotate(this->currRotationFromXInDegs);
    unitDir.Normalize();
}

void MineTurretBlock::CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const {

    const PlayerPaddle* paddle = model->GetPlayerPaddle();
    if (paddle->HasBeenPausedAndRemovedFromGame(model->GetPauseState())) {
        canSeePaddle = canFireAtPaddle = false;
        return;
    }
    
    // Check to see whether the paddle is in view or not...
    Vector2D fireDir;
    this->GetFiringDirection(fireDir);
    Collision::Ray2D rayOfFire(this->GetCenter(), fireDir);
    
    float paddleRayT = 0.0f;
    bool collidesWithPaddle = paddle->GetBounds().CollisionCheck(rayOfFire, paddleRayT);

    // Check to see if the ray collides with the paddle before doing any further calculations...
    if (collidesWithPaddle) {

        std::set<const LevelPiece*> ignorePieces;
        ignorePieces.insert(this);
        
        static std::set<LevelPiece::LevelPieceType> ignoreTypes;
        ignoreTypes.insert(LevelPiece::NoEntry);
        ignoreTypes.insert(LevelPiece::Empty);
        ignoreTypes.insert(LevelPiece::Cannon);
        ignoreTypes.insert(LevelPiece::FragileCannon);
        ignoreTypes.insert(LevelPiece::Portal);
        ignoreTypes.insert(LevelPiece::OneWay);
        ignoreTypes.insert(LevelPiece::Switch);
        ignoreTypes.insert(LevelPiece::Ink);

        std::list<LevelPiece*> collisionPieces;
        model->GetCurrentLevel()->GetLevelPieceColliders(rayOfFire, ignorePieces, ignoreTypes,
            collisionPieces, paddleRayT, 0.525f * MineTurretProjectile::WIDTH_DEFAULT);

        if (collisionPieces.empty()) {
            // The ray is unimpeded, fire ze lasers!
            canSeePaddle    = true;
            canFireAtPaddle = true;
            return;
        }
        else {
            float levelPieceRayT = std::numeric_limits<float>::max();         
            canFireAtPaddle = false;

            // Looks like the ray was impeded - try to find out whether or not the turret can
            // even remotely view the paddle at all, this is difficult since it might be able to see the paddle
            // through an open space between blocks via some ray in its fov... approximate this
            LevelPiece* collisionPiece = model->GetCurrentLevel()->GetLevelPieceFirstCollider(rayOfFire, ignorePieces, levelPieceRayT, 0);
            if (collisionPiece == NULL || paddleRayT < levelPieceRayT) {
                canSeePaddle = true;
                return;
            }
        }
    }

    canSeePaddle    = false;
    canFireAtPaddle = false;
}

void MineTurretBlock::UpdateSpeed() {
    this->currRotationSpd = NumberFuncs::SignOf(this->currRotationSpd) * fabs(this->currRotationSpd) + this->currRotationAccel;
    if (this->currRotationSpd > MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
    else if (this->currRotationSpd < -MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = -MineTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
}