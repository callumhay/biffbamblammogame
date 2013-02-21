/**
 * RocketTurretBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "RocketTurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"
#include "RocketTurretProjectile.h"
#include "PaddleMineProjectile.h"

const float RocketTurretBlock::ROCKET_HOLE_RADIUS = 0.21f;

const float RocketTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC  = 180.0f;
const float RocketTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD = 350.0f;
const float RocketTurretBlock::BARREL_RECOIL_TRANSLATION_AMT       = -0.35f;

const float RocketTurretBlock::ROCKET_DISPLACEMENT_PRE_LOAD =
RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 0.6f  * RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT;
const float RocketTurretBlock::ROCKET_DISPLACEMENT_ON_LOAD  =
RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 0.2f * RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT;

const float RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X = 0.52f;
const float RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y = 0.0f;
const float RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Z = 0.51f;

const float RocketTurretBlock::FIRE_RATE_IN_ROCKETS_PER_SEC = 0.5f;
const float RocketTurretBlock::BARREL_RELOAD_TIME           = 0.7f * (1.0f / RocketTurretBlock::FIRE_RATE_IN_ROCKETS_PER_SEC);
const float RocketTurretBlock::BARREL_RECOIL_TIME           = 0.02f * (1.0f / RocketTurretBlock::FIRE_RATE_IN_ROCKETS_PER_SEC);
const float RocketTurretBlock::ROCKET_LOAD_TIME             = 0.28f * (1.0f / RocketTurretBlock::FIRE_RATE_IN_ROCKETS_PER_SEC);

const double RocketTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME = 90.0f  / RocketTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
const double RocketTurretBlock::LOST_AND_FOUND_MAX_SEEK_TIME = 120.0f / RocketTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;

const int RocketTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES = 2;
const int RocketTurretBlock::LOST_AND_FOUND_MAX_NUM_LOOK_TIMES = 5;

#define PIECE_STARTING_LIFE_POINTS (5*GameModelConstants::GetInstance()->DEFAULT_DAMAGE_ON_BALL_HIT)

RocketTurretBlock::RocketTurretBlock(unsigned int wLoc, unsigned int hLoc) :
TurretBlock(wLoc, hLoc, PIECE_STARTING_LIFE_POINTS), 
currTurretState(SeekingTurretState), currRotationFromXInDegs(Randomizer::GetInstance()->RandomUnsignedInt() % 360), currRotationSpd(0.0f),
lostAndFoundTimeCounter(0.0), numSearchTimesCounter(0), numTimesToSearch(0) {

    this->SetBarrelState(BarrelForwardRocketLoaded, NULL);

    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->currRotationAccel = ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }
    else {
        this->currRotationAccel = -ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }

    this->colour = ColourRGBA(0.65f, 0.65f, 0.65f, 1.0f);
}

RocketTurretBlock::~RocketTurretBlock() {
}

// Determine whether the given projectile will pass through this block...
bool RocketTurretBlock::ProjectilePassesThrough(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::BossLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        
            // When frozen, projectiles can pass through
            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
                return true;
            }
            break;

        case Projectile::RocketTurretBulletProjectile:
            // Rockets shot from this turret should not be destroyed by this turret!
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
int RocketTurretBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return RocketTurretBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

LevelPiece* RocketTurretBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);

    LevelPiece* newPiece = this;
	switch (projectile->GetType()) {

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

		case Projectile::CollateralBlockProjectile:
			// Completely destroy the block...
            newPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;


        case Projectile::RocketTurretBulletProjectile:
            // Deal with rockets that have been fired from the barrel of this turret - they
            // shouldn't collide with this block!
            if (projectile->IsLastThingCollidedWith(this)) {
                break;
            }
        case Projectile::PaddleRocketBulletProjectile: {

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

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile: {

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
			// Fire glob just extinguishes on a laser turret block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
			break;

		default:
			assert(false);
			break;
	}

	return newPiece;
}

void RocketTurretBlock::AITick(double dT, GameModel* gameModel) {

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
bool RocketTurretBlock::ExecutePaddleSeekingAI(double dT, const GameModel* model) {
    //assert(this->currTurretState == SeekingTurretState);

    this->UpdateSpeed();
    float rotationAmt = dT * currRotationSpd;
    this->currRotationFromXInDegs += rotationAmt;
    
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
bool RocketTurretBlock::ExecuteContinueTrackingAI(double dT, const GameModel* model) {
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
    this->currRotationAccel = NumberFuncs::SignOf(perpendicularDir[2]) * RocketTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
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
bool RocketTurretBlock::ContinueTrackingStateUpdateFromView(const GameModel* model) {
    bool canSeePaddle, canFireAtPaddle;
    this->CanSeeAndFireAtPaddle(model, canSeePaddle, canFireAtPaddle);
    if (canSeePaddle) {
        return canFireAtPaddle;
    }

    this->SetTurretState(TargetLostTurretState);
    return false;
}

bool RocketTurretBlock::ExecuteLostAndFoundAI(double dT, const GameModel* model) {
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
            this->currRotationAccel = -NumberFuncs::SignOf(this->currRotationAccel) * RocketTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
            this->lostAndFoundTimeCounter = 0.0;
            if (this->numSearchTimesCounter == 1) {
                this->nextLostAndFoundSeekTime = 2.0 * this->nextLostAndFoundSeekTime;
            }

        }
        return false;
    }

    return this->ExecutePaddleSeekingAI(dT, model);
}

void RocketTurretBlock::SetTurretState(const TurretAIState& state) {
    TurretAIState newState = state;
    if (this->currTurretState == TargetLostTurretState && newState == TargetLostTurretState) {
        newState = SeekingTurretState;
    }

    if (newState == TargetLostTurretState) {
        this->lostAndFoundTimeCounter = 0.0;
        this->nextLostAndFoundSeekTime = RocketTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME +
        Randomizer::GetInstance()->RandomNumZeroToOne() * (RocketTurretBlock::LOST_AND_FOUND_MAX_SEEK_TIME - RocketTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME);

        this->numSearchTimesCounter = 0;
        this->numTimesToSearch = RocketTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + Randomizer::GetInstance()->RandomUnsignedInt() %
            (RocketTurretBlock::LOST_AND_FOUND_MAX_NUM_LOOK_TIMES - RocketTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + 1);
    }

    RocketTurretBlock::TurretAIState prevState = this->currTurretState;
    this->currTurretState = newState;

    // EVENT: The AI state of the rocket turret block has changed
    GameEventManager::Instance()->ActionRocketTurretAIStateChanged(*this, prevState, this->currTurretState);
}

void RocketTurretBlock::SetBarrelState(const BarrelAnimationState& state, GameModel* model) {

    switch (state) {

        case BarrelForwardRocketLoading:
            this->barrelMovementAnim.SetInterpolantValue(0.0f);
            this->barrelMovementAnim.ClearLerp();
            this->rocketMovementAnim.SetLerp(0.0, RocketTurretBlock::ROCKET_LOAD_TIME,
                RocketTurretBlock::ROCKET_DISPLACEMENT_PRE_LOAD, RocketTurretBlock::ROCKET_DISPLACEMENT_ON_LOAD);
            break;

        case BarrelForwardRocketLoaded:
            this->rocketMovementAnim.SetInterpolantValue(RocketTurretBlock::ROCKET_DISPLACEMENT_ON_LOAD);
            this->rocketMovementAnim.ClearLerp();
            break;

        case BarrelRecoiling: {
            assert(model != NULL);

            // Fire a rocket from the barrel...
            Vector2D vec(RocketTurretBlock::ROCKET_DISPLACEMENT_ON_LOAD, RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y);
            vec.Rotate(this->currRotationFromXInDegs);
            Point2D rocketOrigin = this->GetCenter() + vec;
            this->GetFiringDirection(vec);

            RocketTurretProjectile* turretProjectile = new RocketTurretProjectile(rocketOrigin, vec);
            turretProjectile->SetLastThingCollidedWith(this);
            model->AddProjectile(turretProjectile);

            // EVENT: The rocket turret just fired a rocket
            GameEventManager::Instance()->ActionRocketFiredByTurret(*this);

            this->barrelMovementAnim.SetLerp(RocketTurretBlock::BARREL_RECOIL_TIME, RocketTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->rocketMovementAnim.SetInterpolantValue(RocketTurretBlock::BARREL_OFFSET_EXTENT_ALONG_X - 1.05f*RocketTurretProjectile::TURRETROCKET_HEIGHT_DEFAULT);
            this->rocketMovementAnim.ClearLerp();
            break;
        }

        case BarrelBack:
            // The barrel has full recoil
            this->barrelMovementAnim.SetInterpolantValue(RocketTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrelMovementAnim.ClearLerp();
            break;

        case BarrelReloading: {
            this->barrelMovementAnim.SetLerp(RocketTurretBlock::BARREL_RELOAD_TIME, 0.0f);
            break;
        }

        default:
            assert(false);
            return;
    }

    this->barrelState = state;
}

void RocketTurretBlock::UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model) {
    switch (this->barrelState) {
        
        case BarrelForwardRocketLoading: {
            bool isFinished = this->rocketMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelForwardRocketLoaded, model);
            }
            break;
        }

        case BarrelForwardRocketLoaded:
            if (isAllowedToFire) {
                this->SetBarrelState(BarrelRecoiling, model);
            }
            break;

        case BarrelRecoiling: {
            bool isFinished = this->barrelMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelBack, model);
            }
            break;
        }

        case BarrelBack:
            this->SetBarrelState(BarrelReloading, model);
            break;

        case BarrelReloading: {
            bool isFinished = this->barrelMovementAnim.Tick(dT);
            if (isFinished) {
                this->SetBarrelState(BarrelForwardRocketLoading, model);
            }
            break;
        }

        default:
            assert(false);
            return;
    }
}

void RocketTurretBlock::GetBarrelInfo(Point3D& endOfBarrelPt, Vector2D& barrelDir) const {
    this->GetFiringDirection(barrelDir);
    endOfBarrelPt = Point3D(this->GetCenter() + BARREL_OFFSET_EXTENT_ALONG_X * barrelDir, BARREL_OFFSET_EXTENT_ALONG_Z);
}

void RocketTurretBlock::GetFiringDirection(Vector2D& unitDir) const {
    unitDir[0] = 1;
    unitDir[1] = 0;
    unitDir.Rotate(this->currRotationFromXInDegs);
    unitDir.Normalize();
}

void RocketTurretBlock::CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const {
    // Check to see whether the paddle is in view or not...
    Vector2D fireDir;
    this->GetFiringDirection(fireDir);
    Collision::Ray2D rayOfFire(this->GetCenter(), fireDir);
    
    float paddleRayT = 0.0f;
    bool collidesWithPaddle = model->GetPlayerPaddle()->GetBounds().CollisionCheck(rayOfFire, paddleRayT);

    // Check to see if the ray collides with the paddle before doing any further calculations...
    if (collidesWithPaddle) {

        std::set<const LevelPiece*> ignorePieces;
        ignorePieces.insert(this);
        
        static std::set<LevelPiece::LevelPieceType> ignoreTypes;
        ignoreTypes.insert(LevelPiece::NoEntry);
        ignoreTypes.insert(LevelPiece::Empty);
        ignoreTypes.insert(LevelPiece::Cannon);
        ignoreTypes.insert(LevelPiece::Portal);
        ignoreTypes.insert(LevelPiece::OneWay);

        std::list<LevelPiece*> collisionPieces;
        model->GetCurrentLevel()->GetLevelPieceColliders(rayOfFire, ignorePieces, ignoreTypes,
            collisionPieces, 0.525f * RocketTurretProjectile::TURRETROCKET_WIDTH_DEFAULT);

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

void RocketTurretBlock::UpdateSpeed() {
    this->currRotationSpd = NumberFuncs::SignOf(this->currRotationSpd) * fabs(this->currRotationSpd) + this->currRotationAccel;
    if (this->currRotationSpd > MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
    else if (this->currRotationSpd < -MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = -MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
}