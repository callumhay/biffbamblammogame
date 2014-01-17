/**
 * TeslaBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "TeslaBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "PaddleLaserBeam.h"

const float TeslaBlock::LIGHTNING_ARC_RADIUS = LevelPiece::PIECE_HEIGHT * 0.25f;

// Life points that must be diminished in order to toggle the state of the Tesla block (on/off
// (if it's changeable)
const int TeslaBlock::TOGGLE_ON_OFF_LIFE_POINTS = 150;	

TeslaBlock::TeslaBlock(bool isActive, bool isChangable, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), electricityIsActive(isActive), isChangable(isChangable), 
lifePointsUntilNextToggle(TOGGLE_ON_OFF_LIFE_POINTS), timeOfLastToggling(0) {

    this->SetRandomRotationAxis();
    this->SetRandomRotationAmount();
}

TeslaBlock::~TeslaBlock() {
}

LevelPiece* TeslaBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
    UNUSED_PARAMETER(method);
    this->ToggleElectricity(*gameModel, *gameModel->GetCurrentLevel());
    return this;
}

void TeslaBlock::Triggered(GameModel* gameModel) {
    this->ToggleElectricity(*gameModel, *gameModel->GetCurrentLevel(), true);
}

void TeslaBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// If the Tesla block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
		return;
	}

	static const float HALF_TESLA_HEIGHT_BOUND = LevelPiece::HALF_PIECE_HEIGHT;
	static const float HALF_TESLA_WIDTH_BOUND  = LevelPiece::HALF_PIECE_WIDTH;

	// Set the bounding lines for a rectangular block
    static const int MAX_NUM_LINES = 4;
    Collision::LineSeg2D boundingLines[MAX_NUM_LINES];
    Vector2D  boundingNorms[MAX_NUM_LINES];

    bool shouldGenBounds = false;
    int lineCount = 0;

	// Left boundary of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (leftNeighbor->GetType() != LevelPiece::Solid &&
         leftNeighbor->GetType() != LevelPiece::Breakable && 
         leftNeighbor->GetType() != LevelPiece::Bomb &&
         leftNeighbor->GetType() != LevelPiece::Tesla &&
         leftNeighbor->GetType() != LevelPiece::AlwaysDrop && 
         leftNeighbor->GetType() != LevelPiece::Regen));

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND), 
            this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(-1, 0);
        lineCount++;
	}

	// Bottom boundary of the piece
	shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        (bottomNeighbor->GetType() != LevelPiece::Solid && 
         bottomNeighbor->GetType() != LevelPiece::Breakable &&
         bottomNeighbor->GetType() != LevelPiece::Bomb &&
         bottomNeighbor->GetType() != LevelPiece::Tesla &&
         bottomNeighbor->GetType() != LevelPiece::AlwaysDrop && 
         bottomNeighbor->GetType() != LevelPiece::Regen));

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
            this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
        boundingNorms[lineCount] = Vector2D(0, -1);
        lineCount++;
	}

	// Right boundary of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (rightNeighbor->GetType() != LevelPiece::Solid && 
         rightNeighbor->GetType() != LevelPiece::Breakable &&
         rightNeighbor->GetType() != LevelPiece::Bomb && 
         rightNeighbor->GetType() != LevelPiece::Tesla &&
         rightNeighbor->GetType() != LevelPiece::AlwaysDrop && 
         rightNeighbor->GetType() != LevelPiece::Regen));

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
            this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(1, 0);
        lineCount++;
	}

	// Top boundary of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        (topNeighbor->GetType() != LevelPiece::Solid && 
         topNeighbor->GetType() != LevelPiece::Breakable &&
         topNeighbor->GetType() != LevelPiece::Bomb && 
         topNeighbor->GetType() != LevelPiece::Tesla &&
         topNeighbor->GetType() != LevelPiece::AlwaysDrop && 
         topNeighbor->GetType() != LevelPiece::Regen));

    if (shouldGenBounds) {
        boundingLines[lineCount] = Collision::LineSeg2D(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND),
            this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
	    boundingNorms[lineCount] = Vector2D(0, 1);
        lineCount++;
	}

	this->SetBounds(BoundingLines(lineCount, boundingLines, boundingNorms), 
        leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

LevelPiece* TeslaBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	if (ball.IsLastPieceCollidedWith(this)) {
		return this;
	}

	// Toggle the electricity to the tesla block
	GameLevel* currLevel = gameModel->GetCurrentLevel();
	this->ToggleElectricity(*gameModel, *currLevel);

	// Tell the ball what the last piece it collided with was...
	ball.SetLastPieceCollidedWith(this);

	return this;
}

/**
 * Called when the tesla block is hit by a projectile. Tends to cause the projectile to
 * extinguish - all projectiles also tend to activate/deactivate the tesla block.
 */
LevelPiece* TeslaBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// TODO: Tell the game model to recalculate it's tesla lightning arcs...

	GameLevel* currLevel = gameModel->GetCurrentLevel();
	switch (projectile->GetType()) {
		
        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;
		
		case Projectile::CollateralBlockProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile: {
				// The rocket should not destroy this block, however it certainly
				// is allowed to destroy blocks around it!

                assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			    LevelPiece* resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);

                UNUSED_VARIABLE(resultingPiece);
				assert(resultingPiece == this);
				this->ToggleElectricity(*gameModel, *currLevel);
			}
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
        case Projectile::PaddleFlameBlastProjectile:
        case Projectile::PaddleIceBlastProjectile:
            // Extinguish, projectile has no effect on this block
			break;

		default:
			assert(false);
			break;
	}

	return this;
}

LevelPiece* TeslaBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
	assert(gameModel != NULL);

	// If this is not changable then we don't care
	if (!this->GetIsChangable()) {
		return this;
	}

	this->lifePointsUntilNextToggle -= static_cast<float>(dT * static_cast<double>(beamSegment->GetDamagePerSecond()));
	if (this->lifePointsUntilNextToggle <= 0) {
		// Reset the hit points for the next toggle
		this->lifePointsUntilNextToggle = TeslaBlock::TOGGLE_ON_OFF_LIFE_POINTS;

		// We now need to toggle the tesla block
		GameLevel* currLevel = gameModel->GetCurrentLevel();
		this->ToggleElectricity(*gameModel, *currLevel);
	}

	return this;
}

void TeslaBlock::ToggleElectricity(GameModel& gameModel, GameLevel& level, bool ignoreIsChangable) {
	if (!this->GetIsChangable() && !ignoreIsChangable) {
		return;
	}

    // Make sure we've waited long enough since the last toggling
    unsigned long currTime = BlammoTime::GetSystemTimeInMillisecs(); 
    if (labs(static_cast<long>(currTime) - static_cast<long>(this->timeOfLastToggling)) < MIN_TIME_BETWEEN_TOGGLINGS_IN_MS) {
        return;
    }

	// Get the list of active connected Tesla blocks
	std::list<TeslaBlock*> activeNeighbourTeslaBlocks = this->GetActiveConnectedTeslaBlocks();

	// Toggle the electricity...
	this->electricityIsActive = !this->electricityIsActive;

	// Based on whether the electricity is now active or not, signal the proper events and add/remove
	// lightning arcs from the current game level
	if (this->electricityIsActive) {

        // Set a new rotation axis
        this->SetRandomRotationAxis();
        
        // Tell the level to add the electricity between it an its active neighbours/pair-blocks (if there are any)
		for (std::list<TeslaBlock*>::const_iterator iter = activeNeighbourTeslaBlocks.begin(); iter != activeNeighbourTeslaBlocks.end(); ++iter) {
			const TeslaBlock* activeNeighbour = *iter;
			assert(activeNeighbour != NULL);
			level.AddTeslaLightningBarrier(&gameModel, this, activeNeighbour);
		}
	}
	else {
		for (std::list<TeslaBlock*>::const_iterator iter = activeNeighbourTeslaBlocks.begin(); iter != activeNeighbourTeslaBlocks.end(); ++iter) {
			const TeslaBlock* activeNeighbour = *iter;
			assert(activeNeighbour != NULL);
			level.RemoveTeslaLightningBarrier(this, activeNeighbour);
		}
	}

    this->timeOfLastToggling = currTime;
}