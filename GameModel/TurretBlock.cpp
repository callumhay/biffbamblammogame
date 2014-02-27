/**
 * TurretBlock.cpp
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
                
                // EVENT: Frozen block canceled-out by fire
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

LevelPiece* TurretBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    if (paddle.IsLastPieceCollidedWith(this)) {
        return this;
    }

    LevelPiece* resultingPiece = this;

    if (this->HasStatus(LevelPiece::IceCubeStatus)) {
        resultingPiece = this->Destroy(gameModel, LevelPiece::IceShatterDestruction);
    }
    else {
        resultingPiece = this->DiminishPiece(paddle.GetCollisionDamage(), gameModel, LevelPiece::RegularDestruction);
        if (resultingPiece->GetType() != LevelPiece::Empty) {
            paddle.SetLastPieceCollidedWith(resultingPiece);
        }
    }

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
