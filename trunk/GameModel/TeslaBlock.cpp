/**
 * TeslaBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TeslaBlock.h"
#include "Projectile.h"
#include "GameModel.h"
#include "PaddleLaserBeam.h"

const float TeslaBlock::LIGHTNING_ARC_RADIUS = LevelPiece::PIECE_HEIGHT * 0.25f;

// Life points that must be diminished in order to toggle the state of the tesla block (on/off
// (if it's changable)
const int TeslaBlock::TOGGLE_ON_OFF_LIFE_POINTS = 150;	

TeslaBlock::TeslaBlock(bool isActive, bool isChangable, unsigned int wLoc, unsigned int hLoc) : 
LevelPiece(wLoc, hLoc), electricityIsActive(isActive), isChangable(isChangable), 
lifePointsUntilNextToggle(TOGGLE_ON_OFF_LIFE_POINTS) {
}

TeslaBlock::~TeslaBlock() {
}

void TeslaBlock::Triggered(GameModel* gameModel) {
    this->ToggleElectricity(*gameModel, *gameModel->GetCurrentLevel());
}

/**
 * Update the collision boundries of this tesla block, tesla blocks are special in that they will
 * ALWAYS have all possible collision boundries enabled and created.
 */
void TeslaBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// If the tesla block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
		return;
	}

	static const float HALF_TESLA_HEIGHT_BOUND = LevelPiece::HALF_PIECE_HEIGHT;
	static const float HALF_TESLA_WIDTH_BOUND  = LevelPiece::HALF_PIECE_WIDTH;

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

    bool shouldGenBounds = false;

	// Left boundry of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        leftNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
	    Collision::LineSeg2D l1(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND), 
							     this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
	    Vector2D n1(-1, 0);
	    boundingLines.push_back(l1);
	    boundingNorms.push_back(n1);
	}

	// Bottom boundry of the piece
	shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        bottomNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
        Collision::LineSeg2D l2(this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
						         this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND));
        Vector2D n2(0, -1);
        boundingLines.push_back(l2);
        boundingNorms.push_back(n2);
	}

	// Right boundry of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        rightNeighbor->GetType() != LevelPiece::Solid);

    if (shouldGenBounds) {
	    Collision::LineSeg2D l3(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, -HALF_TESLA_HEIGHT_BOUND),
							     this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
	    Vector2D n3(1, 0);
	    boundingLines.push_back(l3);
	    boundingNorms.push_back(n3);
	}

	// Top boundry of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        topNeighbor->HasStatus(LevelPiece::OnFireStatus) || topNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
	    Collision::LineSeg2D l4(this->center + Vector2D(HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND),
							     this->center + Vector2D(-HALF_TESLA_WIDTH_BOUND, HALF_TESLA_HEIGHT_BOUND));
	    Vector2D n4(0, 1);
	    boundingLines.push_back(l4);
	    boundingNorms.push_back(n4);
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
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
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;
		
		case Projectile::CollateralBlockProjectile:
			this->ToggleElectricity(*gameModel, *currLevel);
			break;

		case Projectile::PaddleRocketBulletProjectile:
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
			// Fire glob will just extinguish
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

void TeslaBlock::ToggleElectricity(GameModel& gameModel, GameLevel& level) {
	if (!this->GetIsChangable()) {
		return;
	}

	// Get the list of active connected tesla blocks
	std::list<TeslaBlock*> activeNeighbourTeslaBlocks = this->GetActiveConnectedTeslaBlocks();

	// Toggle the electricity...
	this->electricityIsActive = !this->electricityIsActive;

	// Based on whether the electricity is now active or not, signal the proper events and add/remove
	// lightning arcs from the current game level
	if (this->electricityIsActive) {
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
}