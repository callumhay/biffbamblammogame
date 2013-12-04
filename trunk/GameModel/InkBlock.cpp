/**
 * InkBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "InkBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"

InkBlock::InkBlock(unsigned int wLoc, unsigned int hLoc) :
LevelPiece(wLoc, hLoc), currLifePoints(InkBlock::PIECE_STARTING_LIFE_POINTS) {
}

InkBlock::~InkBlock() {
}

void InkBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                            const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                            const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                            const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(leftNeighbor);
	UNUSED_PARAMETER(bottomNeighbor);
	UNUSED_PARAMETER(rightNeighbor);
	UNUSED_PARAMETER(topNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(bottomLeftNeighbor);

	// Make the bounds a bit smaller than a typical level piece and always make all of them

	// Clear all the currently existing boundary lines first
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];

	static const float FRACTION_HALF_PIECE_WIDTH  = 0.7f * LevelPiece::HALF_PIECE_WIDTH;
	static const float FRACTION_HALF_PIECE_HEIGHT = 0.7f * LevelPiece::HALF_PIECE_HEIGHT;

	// Left boundary of the piece
    boundingLines[0] = Collision::LineSeg2D(this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT), 
        this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT));
	boundingNorms[0] = Vector2D(-1, 0);

	// Bottom boundary of the piece
    boundingLines[1] = Collision::LineSeg2D(this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT),
        this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT));
	boundingNorms[1] = Vector2D(0, -1);

	// Right boundary of the piece
    boundingLines[2] = Collision::LineSeg2D(this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT),
        this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT));
	boundingNorms[2] = Vector2D(1, 0);

	// Top boundary of the piece
    boundingLines[3] = Collision::LineSeg2D(this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT),
        this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT));
	boundingNorms[3] = Vector2D(0, 1);

	this->SetBounds(BoundingLines(MAX_NUM_LINES, boundingLines, boundingNorms), 
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

// Determine whether the given projectile will pass through this block...
bool InkBlock::ProjectilePassesThrough(const Projectile* projectile) const {
	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
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
int InkBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return InkBlock::POINTS_ON_BLOCK_DESTROYED;
    }
    return 0;
}

/**
 * The ink block is destroyed and replaced by an empty space.
 * Returns: A new empty space block.
 */
LevelPiece* InkBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// EVENT: Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);

	// Check to see if the ink block is frozen...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
        assert(success);
	}

	// Tell the level that this piece has changed to empty...
	GameLevel* level = gameModel->GetCurrentLevel();
	LevelPiece* emptyPiece = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPiece, method);

	// Obliterate all that is left of this block...
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;

    // Add to the boost meter
    gameModel->AddPercentageToBoostMeter(0.1);

	return emptyPiece;
}

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* resultingPiece = this;
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (isFireBall) {
			// Unfreeze a frozen ink block if it gets hit by a fireball
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);

                // EVENT: Frozen block cancelled-out by fire
                GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
			}
            else {
                resultingPiece = this->Destroy(gameModel, LevelPiece::RegularDestruction);
            }
		}
        else {
            resultingPiece = this->Destroy(gameModel, 
                this->HasStatus(LevelPiece::IceCubeStatus) ? LevelPiece::IceShatterDestruction : LevelPiece::RegularDestruction);
        }
	}

    ball.SetLastPieceCollidedWith(resultingPiece);
	return resultingPiece;
}

LevelPiece* InkBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				this->DoIceCubeReflectRefractLaserBullets(projectile, gameModel);
			}
			else {
                resultingPiece = this->Destroy(gameModel, LevelPiece::LaserProjectileDestruction);
			}
			break;

        case Projectile::BossLightningBoltBulletProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::BasicProjectileDestruction);
            break;

		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on an ink block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			this->LightPieceOnFire(gameModel, false);
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

LevelPiece* InkBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);
	
	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}

	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
        newPiece = this->Destroy(gameModel, LevelPiece::LaserBeamDestruction);
	}

	return newPiece;
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* InkBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	this->currLifePoints -= static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond()));

	LevelPiece* newPiece = this;
	if (currLifePoints <= 0) {
		// The piece is dead... destroy it
		this->currLifePoints = 0;
        newPiece = this->Destroy(gameModel, LevelPiece::PaddleShieldDestruction);
	}

	return newPiece;
}