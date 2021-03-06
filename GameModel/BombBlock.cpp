/**
 * BombBlock.cpp
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

#include "BombBlock.h"
#include "EmptySpaceBlock.h"
#include "TriangleBlocks.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameLevel.h"
#include "Beam.h"

#include <set>

BombBlock::BombBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc),
currLifePoints(BombBlock::PIECE_STARTING_LIFE_POINTS) {
}

BombBlock::~BombBlock() {
}

// Whether or not the given projectile passes through this block...
bool BombBlock::ProjectilePassesThrough(const Projectile* projectile) const {
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
int BombBlock::GetPointsOnChange(const LevelPiece& changeToPiece) const {
    if (changeToPiece.GetType() == LevelPiece::Empty) {
        return BombBlock::BOMB_DESTRUCTION_POINTS;
    }
    return 0;
}

LevelPiece* BombBlock::Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method) {
	// Obtain the level from the model...
	GameLevel* level = gameModel->GetCurrentLevel();
	
	// If the bomb is encased in ice then we don't blow up, we just shatter
	if (!this->HasStatus(LevelPiece::IceCubeStatus)) {
		
        // Grab all the bombs that will be destroyed...
        std::list<LevelPiece*> destroyedBombStack;
        std::set<LevelPiece*> destroyedBombSet;
        std::set<LevelPiece*> destroyedNonBombSet;
        
        destroyedBombStack.push_back(this);
        while (!destroyedBombStack.empty()) {
            
            LevelPiece* currDestroyedBomb = destroyedBombStack.front();
            assert(currDestroyedBomb != NULL);

            destroyedBombStack.pop_front();
            destroyedBombSet.insert(currDestroyedBomb);
            
            // Check all the pieces that the bomb destroyed, if any are bombs that we haven't visited yet,
            // then we place them on the stack and we keep going
            
            #define CHECK_FOR_BOMB_AND_ADD_TO_STACK(hIdxOffset, wIdxOffset) { \
                LevelPiece* levelPiece = level->GetLevelPieceFromCurrentLayout(currDestroyedBomb->GetHeightIndex() + hIdxOffset, currDestroyedBomb->GetWidthIndex() + wIdxOffset); \
                if (levelPiece != NULL) { if (levelPiece->GetType() == LevelPiece::Bomb) { if (destroyedBombSet.find(levelPiece) == destroyedBombSet.end()) { destroyedBombStack.push_back(levelPiece); } } \
                else { destroyedNonBombSet.insert(levelPiece); } } }

            CHECK_FOR_BOMB_AND_ADD_TO_STACK(1, 0);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(1, -1);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(1, 1);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(0, -1);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(0, 1);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(-1, 0);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(-1, -1);
            CHECK_FOR_BOMB_AND_ADD_TO_STACK(-1, 1);

            #undef CHECK_FOR_BOMB_AND_ADD_TO_STACK
        }
        destroyedBombSet.erase(this);

        // We now have separate sets of all bombs and all the pieces that were affected by the bombs that weren't bombs... 
        // destroy them all in a nice non-infinitely-recursive way

		// Go through a set of every UNIQUE level piece that was destroyed when the bomb went off
		// and destroy each of those pieces properly
		for (std::set<LevelPiece*>::iterator iter = destroyedNonBombSet.begin(); iter != destroyedNonBombSet.end(); ++iter) {
			LevelPiece* currDestroyedPiece = *iter;
			if (currDestroyedPiece != NULL) {
				assert(currDestroyedPiece->GetType() != LevelPiece::Bomb);

				// Only allow the piece to be destroyed if the ball can destroy it as well...
				if (currDestroyedPiece->CanBeDestroyedByBall()) {
                    currDestroyedPiece->Destroy(gameModel, LevelPiece::BombDestruction);
				}
			}
		}

		// Go through a set of every UNIQUE bomb that was destroyed and destroy each properly
		for (std::set<LevelPiece*>::iterator iter = destroyedBombSet.begin(); iter != destroyedBombSet.end(); ++iter) {
			LevelPiece* currDestroyedBomb = *iter;
			assert(currDestroyedBomb->GetType() == LevelPiece::Bomb);
            assert(currDestroyedBomb != NULL);

            GameEventManager::Instance()->ActionBlockDestroyed(*currDestroyedBomb, LevelPiece::BombDestruction);
			
            level->PieceChanged(gameModel, currDestroyedBomb, 
                new EmptySpaceBlock(currDestroyedBomb->GetWidthIndex(), currDestroyedBomb->GetHeightIndex()),
                LevelPiece::BombDestruction);

			delete currDestroyedBomb;
			currDestroyedBomb = NULL;
		}

        // Charge up the boost meter a little bit
        gameModel->AddPercentageToBoostMeter(0.05);
	}
	else {
		// EVENT: Ice was shattered
		GameEventManager::Instance()->ActionBlockIceShattered(*this);
	}

	// Obliterate this bomb
	// EVENT: Bomb Block is being destroyed
	GameEventManager::Instance()->ActionBlockDestroyed(*this, method);
	LevelPiece* emptyPieceForBomb = new EmptySpaceBlock(this->wIndex, this->hIndex);
	level->PieceChanged(gameModel, this, emptyPieceForBomb, method);
	LevelPiece* tempThis = this;
	delete tempThis;
	tempThis = NULL;
    
	return emptyPieceForBomb;
}

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	LevelPiece* resultingPiece = this;

	bool isIceBall  = ((ball.GetBallType() & GameBall::IceBall) == GameBall::IceBall);
	
	// Bombs may be encased in ice, when they are they don't explode if they get destroyed...
	if (isIceBall) {
		this->FreezePieceInIce(gameModel);
	}
	else {
		bool isFireBall = ((ball.GetBallType() & GameBall::FireBall) == GameBall::FireBall);
		if (isFireBall) {
			// Unfreeze a frozen bomb block if it gets hit by a fireball
			if (this->HasStatus(LevelPiece::IceCubeStatus)) {
				bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
                UNUSED_VARIABLE(success);
				assert(success);
                // EVENT: Frozen block canceled-out by fire
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

LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle) {
    UNUSED_PARAMETER(paddle);

    LevelPiece* resultingPiece = this->Destroy(gameModel, 
        this->HasStatus(LevelPiece::IceCubeStatus) ? LevelPiece::IceShatterDestruction : LevelPiece::RegularDestruction);
    return resultingPiece;
}

/**
 * When the bomb block encounters a projectile it tends to go boom...
 * Results in an empty level piece where the bomb block once was.
 */
LevelPiece* BombBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch(projectile->GetType()) {
		
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
        case Projectile::BossShockOrbBulletProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::BasicProjectileDestruction);
            break;

		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
			// Fire glob just extinguishes on a bomb block, unless it's frozen in an ice cube;
			// in that case, unfreeze it
			this->LightPieceOnFire(gameModel, false);
			break;

        case Projectile::PaddleFlameBlastProjectile:
            this->LightPieceOnFire(gameModel, false);
            break;

        case Projectile::PaddleIceBlastProjectile:
            this->FreezePieceInIce(gameModel);
            break;

        case Projectile::PortalBlobProjectile:
            break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

/**
 * Tick the a collision with a beam - the beam will eat away at this bomb block until
 * it is destroyed.
 * Returns: The block that this block is/has become.
 */
LevelPiece* BombBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel) {
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
LevelPiece* BombBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel) {
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