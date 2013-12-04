/**
 * TurretBlock.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "TurretBlock.h"
#include "GameEventManager.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "Beam.h"

TurretBlock::TurretBlock(unsigned int wLoc, unsigned int hLoc, float life) :
LevelPiece(wLoc, hLoc), currLifePoints(life), startingLifePoints(life) {
}

TurretBlock::~TurretBlock() {
}

bool TurretBlock::ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const {
    if (((b.GetBallType() & GameBall::IceBall) == GameBall::IceBall)) {
        return false;
    }

    return b.GetCollisionDamage() < this->currLifePoints ;
}

bool TurretBlock::ProjectileIsDestroyedOnCollision(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            if (projectile->IsLastThingCollidedWith(this)) {
                break;
            }

            // Mines are destroyed by collisions with turrets.
            return true;

        default:
            break;
    }

    return !this->ProjectilePassesThrough(projectile);
}

LevelPiece* TurretBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {

	// Check to see if the block is frozen...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        // EVENT: Ice was shattered
        GameEventManager::Instance()->ActionBlockIceShattered(*this);
        // Kill it!
        this->currLifePoints = 0;
	}

    if ((method == LevelPiece::MineDestruction || method == LevelPiece::RocketDestruction) && !this->IsDead()) {
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

    // Add to the boost meter
    gameModel->AddPercentageToBoostMeter(0.25);

	return emptyPiece;
}

LevelPiece* TurretBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {

	LevelPiece* resultingPiece = this;
	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		// Unfreeze a frozen turret if it gets hit by a fireball
        if (this->HasStatus(LevelPiece::IceCubeStatus)) {
            if (isFireBall) {
			    bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
			    assert(success);
                
                // EVENT: Frozen block cancelled-out by fire
                GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
            }
            else {
                // If the turret is frozen and the ball is neither ice nor fire, then the turret shatters...
                resultingPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
            }
		}
        else {
            // TODO: Set turrets on fire? Have them say "ouch" when they are...?
            // If I change this then I need to change the ProducesBounceEffectsWithBallWhenHit method as well!

            resultingPiece = this->DiminishPiece(ball.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);
        }
	}

    ball.SetLastPieceCollidedWith(resultingPiece);
    return resultingPiece;
}

LevelPiece* TurretBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
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
LevelPiece* TurretBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
	assert(gameModel != NULL);
	
	LevelPiece* newPiece = this->DiminishPiece(
        static_cast<float>(dT * static_cast<double>(paddle.GetShieldDamagePerSecond())),
        gameModel, LevelPiece::PaddleShieldDestruction);
	return newPiece;
}
