/**
* LaserTurretBlock.cpp
*
* (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
* Callum Hay, 2011
*
* You may not use this work for commercial purposes.
* If you alter, transform, or build upon this work, you may distribute the 
* resulting work only under the same or similar licence to this one.
*/

#include "LaserTurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"
#include "LaserTurretProjectile.h"

const float LaserTurretBlock::BALL_DAMAGE_AMOUNT                  = static_cast<float>(LaserTurretBlock::PIECE_STARTING_LIFE_POINTS) / 6.0f;
const float LaserTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC  = 200.0f;
const float LaserTurretBlock::ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD = 400.0f;
const float LaserTurretBlock::BARREL_RECOIL_TRANSLATION_AMT       = -0.25f;

const float LaserTurretBlock::FIRE_RATE_IN_BULLETS_PER_SEC = 1.75f;
const float LaserTurretBlock::BARREL_RELOAD_TIME           = 1.0f / (2.0f * LaserTurretBlock::FIRE_RATE_IN_BULLETS_PER_SEC);
const float LaserTurretBlock::BARREL_RECOIL_TIME           = LaserTurretBlock::BARREL_RELOAD_TIME / 4.0f;

const double LaserTurretBlock::LOST_AND_FOUND_MIN_SEEK_TIME = 80.0f  / LaserTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
const double LaserTurretBlock::LOST_AND_FOUND_MAX_SEEK_TIME = 110.0f / LaserTurretBlock::MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;

const int LaserTurretBlock::LOST_AND_FOUND_MIN_NUM_LOOK_TIMES = 3;
const int LaserTurretBlock::LOST_AND_FOUND_MAX_NUM_LOOK_TIMES = 7;

const float LaserTurretBlock::ONE_MORE_BALL_HIT_LIFE_PERCENT = 
    LaserTurretBlock::BALL_DAMAGE_AMOUNT / static_cast<float>(LaserTurretBlock::PIECE_STARTING_LIFE_POINTS);

// Distance along the x-axis that the base of a barrel lies (from the origin of the block)
// in when the barrel is in its 'base' position with no turret rotation
const float LaserTurretBlock::BARREL_OFFSET_ALONG_X = 0.32f;
// Absolute distance along the y-axis that the base of a barrel is from the origin of the block in its
// base position with no turret rotation
const float LaserTurretBlock::BARREL_OFFSET_ALONG_Y = 0.13f;

const float LaserTurretBlock::BARREL_OFFSET_ALONG_Z = 0.38f;

// The maximum extent along the y-axis (in the base position - no rotation, of the block), from the origin of the block
const float LaserTurretBlock::BARREL_OFFSET_EXTENT_ALONG_Y = 0.28f;

LaserTurretBlock::LaserTurretBlock(unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(LaserTurretBlock::PIECE_STARTING_LIFE_POINTS),
currTurretState(SeekingTurretState), currRotationFromXInDegs(Randomizer::GetInstance()->RandomUnsignedInt() % 360), currRotationSpd(0.0f),
lostAndFoundTimeCounter(0.0), numSearchTimesCounter(0), numTimesToSearch(0) {

    // Add a bit of variety - different barrel configurations
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->SetBarrelState(OneForwardTwoBack, NULL);
    }
    else {
        this->SetBarrelState(TwoForwardOneBack, NULL);
    }
    if (Randomizer::GetInstance()->RandomUnsignedInt() % 2 == 0) {
        this->currRotationAccel = ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }
    else {
        this->currRotationAccel = -ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
    }

    this->colour = ColourRGBA(0.65f, 0.65f, 0.65f, 1.0f);
}

LaserTurretBlock::~LaserTurretBlock() {
}

// Determine whether the given projectile will pass through this block...
bool LaserTurretBlock::ProjectilePassesThrough(Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::LaserTurretBulletProjectile:
            // Lasers shot from this turret should not be destroyed by this turret!
            if (projectile->IsLastThingCollidedWith(this)) {
                return true;
            }
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        
            // When frozen, projectiles can pass through
            if (this->HasStatus(LevelPiece::IceCubeStatus)) {
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
int LaserTurretBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return LaserTurretBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

void LaserTurretBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                    const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                    const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                    const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    // Set the bounding lines for a rectangular block
    std::vector<Collision::LineSeg2D> boundingLines;
    std::vector<Vector2D>  boundingNorms;

    // Left boundry of the piece
    if (leftNeighbor != NULL) {
        if (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::LaserTurret &&
            leftNeighbor->GetType() != LevelPiece::Breakable && leftNeighbor->GetType() != LevelPiece::RocketTurret) {
                Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n1(-1, 0);
                boundingLines.push_back(l1);
                boundingNorms.push_back(n1);
        }
    }

    // Bottom boundry of the piece
    if (bottomNeighbor != NULL) {
        if (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::LaserTurret &&
            bottomNeighbor->GetType() != LevelPiece::Breakable && bottomNeighbor->GetType() != LevelPiece::RocketTurret) {
                Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n2(0, -1);
                boundingLines.push_back(l2);
                boundingNorms.push_back(n2);
        }
    }

    // Right boundry of the piece
    if (rightNeighbor != NULL) {
        if (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::LaserTurret &&
            rightNeighbor->GetType() != LevelPiece::Breakable && rightNeighbor->GetType() != LevelPiece::RocketTurret) {
                Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n3(1, 0);
                boundingLines.push_back(l3);
                boundingNorms.push_back(n3);
        }
    }

    // Top boundry of the piece
    if (topNeighbor != NULL) {
        if (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::LaserTurret &&
            topNeighbor->GetType() != LevelPiece::Breakable && topNeighbor->GetType() != LevelPiece::RocketTurret) {
                Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                    this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
                Vector2D n4(0, 1);
                boundingLines.push_back(l4);
                boundingNorms.push_back(n4);
        }
    }

    this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* LaserTurretBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// Check to see if the block is frozen...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
	}

    if (method == LevelPiece::RocketDestruction && !this->IsDead()) {
        return this;
    }
    
    // EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

	return emptyPiece;
}

LevelPiece* LaserTurretBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {

	LevelPiece* resultingPiece = this;
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (isFireBall) {
			// Unfreeze a frozen block if it gets hit by a fireball
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}
            else {
                resultingPiece = this->DiminishPiece(LaserTurretBlock::BALL_DAMAGE_AMOUNT,
                    gameModel, LevelPiece::RegularDestruction);
            }
		}
        else {
            resultingPiece = this->DiminishPiece(LaserTurretBlock::BALL_DAMAGE_AMOUNT,
                gameModel, LevelPiece::RegularDestruction);
        }
	}

    ball.SetLastPieceCollidedWith(resultingPiece);
    return resultingPiece;
}

LevelPiece* LaserTurretBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	assert(gameModel != NULL);
	assert(projectile != NULL);

    LevelPiece* newPiece = this;
	switch (projectile->GetType()) {

	    case Projectile::LaserTurretBulletProjectile:
            // Deal with lasers that have been fired from the barrels of this turret - they
            // shouldn't collide with this block!
            if (projectile->IsLastThingCollidedWith(this)) {
                return this;
            }
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

        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile: {

			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				// EVENT: Ice was shattered
				GameEventManager::Instance()->ActionBlockIceShattered(*this);
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
			}

            LevelPiece* tempPiece = this->DiminishPiece(projectile->GetDamage(), gameModel, LevelPiece::RocketDestruction);

            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			newPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);

            assert(tempPiece == newPiece);
            UNUSED_VARIABLE(tempPiece);
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

LevelPiece* LaserTurretBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond())),
        gameModel, LevelPiece::LaserBeamDestruction);
	return newPiece;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* LaserTurretBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond())),
        gameModel, LevelPiece::PaddleShieldDestruction);
	return newPiece;
}

void LaserTurretBlock::AITick(double dT, GameModel* gameModel) {

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

void LaserTurretBlock::GetBarrel1ExtentPosInLocalSpace(Point3D& pos) const {
    Vector2D offsetDir(0.78f, BARREL_OFFSET_ALONG_Y);
    offsetDir.Rotate(this->currRotationFromXInDegs);
    pos = Point3D(offsetDir[0], offsetDir[1], BARREL_OFFSET_ALONG_Z);
}

void LaserTurretBlock::GetBarrel2ExtentPosInLocalSpace(Point3D& pos) const {
    Vector2D offsetDir(0.78f, -BARREL_OFFSET_ALONG_Y);
    offsetDir.Rotate(this->currRotationFromXInDegs);
    pos = Point3D(offsetDir[0], offsetDir[1], BARREL_OFFSET_ALONG_Z);
}

// Executes the AI for the turret attempting to find the paddle
// Returns: true if the turret may start to fire on the paddle, false if not
bool LaserTurretBlock::ExecutePaddleSeekingAI(double dT, const GameModel* model) {
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
bool LaserTurretBlock::ExecuteContinueTrackingAI(double dT, const GameModel* model) {
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
    this->currRotationAccel = NumberFuncs::SignOf(perpendicularDir[2]) * ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
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
bool LaserTurretBlock::ContinueTrackingStateUpdateFromView(const GameModel* model) {
    bool canSeePaddle, canFireAtPaddle;
    this->CanSeeAndFireAtPaddle(model, canSeePaddle, canFireAtPaddle);
    if (canSeePaddle) {
        return canFireAtPaddle;
    }

    this->SetTurretState(TargetLostTurretState);
    return false;
}

bool LaserTurretBlock::ExecuteLostAndFoundAI(double dT, const GameModel* model) {
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
            this->currRotationAccel = -NumberFuncs::SignOf(this->currRotationAccel) * ROTATION_ACCEL_IN_DEGS_PER_SEC_SQRD;
            this->lostAndFoundTimeCounter = 0.0;
            if (this->numSearchTimesCounter == 1) {
                this->nextLostAndFoundSeekTime = 2.0 * this->nextLostAndFoundSeekTime;
            }

        }
        return false;
    }

    return this->ExecutePaddleSeekingAI(dT, model);
}

void LaserTurretBlock::SetTurretState(const TurretAIState& state) {
    TurretAIState newState = state;
    if (this->currTurretState == TargetLostTurretState && newState == TargetLostTurretState) {
        newState = SeekingTurretState;
    }

    if (newState == TargetLostTurretState) {
        this->lostAndFoundTimeCounter = 0.0;
        this->nextLostAndFoundSeekTime = LOST_AND_FOUND_MIN_SEEK_TIME +
        Randomizer::GetInstance()->RandomNumZeroToOne() * (LOST_AND_FOUND_MAX_SEEK_TIME - LOST_AND_FOUND_MIN_SEEK_TIME);

        this->numSearchTimesCounter = 0;
        this->numTimesToSearch = LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + Randomizer::GetInstance()->RandomUnsignedInt() %
            (LOST_AND_FOUND_MAX_NUM_LOOK_TIMES - LOST_AND_FOUND_MIN_NUM_LOOK_TIMES + 1);
    }

    LaserTurretBlock::TurretAIState prevState = this->currTurretState;
    this->currTurretState = newState;

    // EVENT: The AI state of the laser turret block has changed
    GameEventManager::Instance()->ActionLaserTurretAIStateChanged(*this, prevState, this->currTurretState);
}

void LaserTurretBlock::SetBarrelState(const BarrelAnimationState& state, GameModel* model) {

    switch (state) {

        case OneForwardTwoBack:
            // The first barrel has no recoil, the second has full recoil
            this->barrel1RecoilAnim.SetInterpolantValue(0.0f);
            this->barrel1RecoilAnim.ClearLerp();
            this->barrel2RecoilAnim.SetInterpolantValue(LaserTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrel2RecoilAnim.ClearLerp();
            
            break;

        case OneFiringTwoReloading: {
            assert(model != NULL);

            // Fire a laser from the first barrel...
            Vector2D velocityDir(BARREL_OFFSET_ALONG_X, BARREL_OFFSET_ALONG_Y);
            velocityDir.Rotate(this->currRotationFromXInDegs);
            Point2D laserOrigin = this->GetCenter() + velocityDir;
            this->GetFiringDirection(velocityDir);

            LaserTurretProjectile* turretProjectile = new LaserTurretProjectile(laserOrigin, velocityDir);
            turretProjectile->SetLastThingCollidedWith(this);
            model->AddProjectile(turretProjectile);

            // EVENT: Laser fired by this turret
            GameEventManager::Instance()->ActionLaserFiredByTurret(*this);

            // The first barrel is preparing its recoil and laser discharge, while the second
            // is gathering its charge and recovering from its previous recoil
            this->barrel1RecoilAnim.SetLerp(LaserTurretBlock::BARREL_RECOIL_TIME, LaserTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrel2RecoilAnim.SetLerp(LaserTurretBlock::BARREL_RELOAD_TIME, 0.0f);
            break;
        }

        case TwoForwardOneBack:
            // The second barrel has no recoil, the first has full recoil
            this->barrel2RecoilAnim.SetInterpolantValue(0.0f);
            this->barrel2RecoilAnim.ClearLerp();
            this->barrel1RecoilAnim.SetInterpolantValue(LaserTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrel1RecoilAnim.ClearLerp();
            break;

        case TwoFiringOneReloading: {
            assert(model != NULL);

            // Fire a laser from the second barrel...
            Vector2D velocityDir(BARREL_OFFSET_ALONG_X, -BARREL_OFFSET_ALONG_Y);
            velocityDir.Rotate(this->currRotationFromXInDegs);
            Point2D laserOrigin = this->GetCenter() + velocityDir;
            this->GetFiringDirection(velocityDir);

            LaserTurretProjectile* turretProjectile = new LaserTurretProjectile(laserOrigin, velocityDir);
            turretProjectile->SetLastThingCollidedWith(this);
            model->AddProjectile(turretProjectile);

            // EVENT: Laser fired by this turret
            GameEventManager::Instance()->ActionLaserFiredByTurret(*this);

            // The second barrel is preparing its recoil and laser discharge, while the first
            // is gathering its charge and recovering from its previous recoil
            this->barrel2RecoilAnim.SetLerp(LaserTurretBlock::BARREL_RECOIL_TIME, LaserTurretBlock::BARREL_RECOIL_TRANSLATION_AMT);
            this->barrel1RecoilAnim.SetLerp(LaserTurretBlock::BARREL_RELOAD_TIME, 0.0f);
            break;
        }

        default:
            assert(false);
            return;
    }

    this->barrelAnimState = state;
}

void LaserTurretBlock::UpdateBarrelState(double dT, bool isAllowedToFire, GameModel* model) {

    switch (this->barrelAnimState) {

        case OneForwardTwoBack:
            // Only change state if we're allowed to continue firing
            if (isAllowedToFire) {
                this->SetBarrelState(OneFiringTwoReloading, model);
            }
            break;

        case OneFiringTwoReloading: {
            // We're in the midst of firing a laser, finish off the animation and
            // go to the next logical state for the barrels
            bool isFinished = true;
            isFinished &= this->barrel1RecoilAnim.Tick(dT);
            isFinished &= this->barrel2RecoilAnim.Tick(dT);

            if (isFinished) {
                this->SetBarrelState(TwoForwardOneBack, model);
            }

            break;
        }

        case TwoForwardOneBack:
            // Only change state if we're allowed to continue firing
            if (isAllowedToFire) {
                this->SetBarrelState(TwoFiringOneReloading, model);
            }
            break;

        case TwoFiringOneReloading: {
            // We're in the midst of firing a laser, finish off the animation and
            // go to the next logical state for the barrels
            bool isFinished = true;
            isFinished &= this->barrel1RecoilAnim.Tick(dT);
            isFinished &= this->barrel2RecoilAnim.Tick(dT);

            if (isFinished) {
                this->SetBarrelState(OneForwardTwoBack, model);
            }

            break;
        }

        default:
            assert(false);
            return;
    }
}

void LaserTurretBlock::GetFiringDirection(Vector2D& unitDir) const {
    unitDir[0] = 1;
    unitDir[1] = 0;
    unitDir.Rotate(this->currRotationFromXInDegs);
    unitDir.Normalize();
}

void LaserTurretBlock::CanSeeAndFireAtPaddle(const GameModel* model, bool& canSeePaddle, bool& canFireAtPaddle) const {
    // Check to see whether the paddle is in view or not...
    Vector2D fireDir;
    this->GetFiringDirection(fireDir);
    Collision::Ray2D rayOfFire(this->GetCenter(), fireDir);
    
    float paddleRayT = 0.0f;
    bool collidesWithPaddle = model->GetPlayerPaddle()->GetBounds().CollisionCheck(rayOfFire, paddleRayT);

    // Check to see if the ray collides with the paddle before doing any further calculations...
    if (collidesWithPaddle) {
        // Now make sure the ray isn't colliding with any blocks before the paddle...
        float levelPieceRayT = std::numeric_limits<float>::max();
        std::set<const LevelPiece*> ignorePieces;
        ignorePieces.insert(this);
        LevelPiece* collisionPiece = model->GetCurrentLevel()->GetLevelPieceFirstCollider(rayOfFire,
            ignorePieces, levelPieceRayT, 1.05f * BARREL_OFFSET_EXTENT_ALONG_Y);

        if (collisionPiece == NULL || paddleRayT < levelPieceRayT) {
            // The ray is unimpeded, fire ze lasers!
            canSeePaddle    = true;
            canFireAtPaddle = true;
            return;
        }
        else {
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

LevelPiece* LaserTurretBlock::DiminishPiece(float dmgAmount, GameModel* model, const LevelPiece::DestructionMethod& method) {
    currLifePoints -= dmgAmount;
    if (currLifePoints <= 0.0f) {
        return this->Destroy(model, method);
    }
    return this;
}

void LaserTurretBlock::UpdateSpeed() {
    this->currRotationSpd = NumberFuncs::SignOf(this->currRotationSpd) * fabs(this->currRotationSpd) + this->currRotationAccel;
    if (this->currRotationSpd > MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
    else if (this->currRotationSpd < -MAX_ROTATION_SPEED_IN_DEGS_PER_SEC) {
        this->currRotationSpd = -MAX_ROTATION_SPEED_IN_DEGS_PER_SEC;
    }
}