/**
 * BreakableBlock.h
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

#ifndef __BREAKABLEBLOCK_H__
#define __BREAKABLEBLOCK_H__

#include "LevelPiece.h"
#include "Beam.h"

class BreakableBlock : public LevelPiece {
public:
    enum BreakablePieceType { GreenBreakable = 'G', YellowBreakable = 'Y', OrangeBreakable = 'O', RedBreakable = 'R' }; 

	BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc);
	virtual ~BreakableBlock();

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Breakable;
	}

    bool ProducesBounceEffectsWithBallWhenHit(const GameBall& b) const;

	// Is this piece one without any boundries (i.e., no collision surface/line)?
	// Return: true if non-collider, false otherwise.
	bool IsNoBoundsPieceType() const {
		return false;
	}
	bool BallBouncesOffWhenHit() const {
		return true;
	}


	// Check to see if a given character is a type of breakable block
	// Returns: true if c is a breakable block type, false otherwise.
	static bool IsValidBreakablePieceType(char c) {
		return (c == GreenBreakable || c == YellowBreakable || c == OrangeBreakable || c == RedBreakable);
	}
	
	// All breakable blocks must be destroyed to end a level...
	bool MustBeDestoryedToEndLevel() const {
		return true;
	}
	bool CanBeDestroyedByBall() const {
		return true;
	}
    bool CanChangeSelfOrOtherPiecesWhenHit() const {
        return true;
    }

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	bool BallBlastsThrough(const GameBall& b) const {
        // The ball may blast through ONLY IF the piece is (green OR frozen) AND
        // the ball is uber AND NOT on fire AND NOT icy
        return (this->pieceType == GreenBreakable || this->HasStatus(LevelPiece::IceCubeStatus)) && 
               ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) &&
               ((b.GetBallType() & GameBall::IceBall) != GameBall::IceBall) && 
               ((b.GetBallType() & GameBall::FireBall) != GameBall::FireBall);
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	bool GhostballPassesThrough() const {
		return true;
	}

	bool ProjectilePassesThrough(const Projectile* projectile) const;
    int GetPointsOnChange(const LevelPiece& changeToPiece) const;

	// Breakable blocks do not reflect or refract light.
	// Returns: false
	bool IsLightReflectorRefractor() const {
		// When frozen in ice a block can reflect/refract lasers and the like
		if (this->HasStatus(LevelPiece::IceCubeStatus)) {
			return true;
		}
		return false;
	}

	// Collision related stuffs
    void UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor);

	LevelPiece* Destroy(GameModel* gameModel, const LevelPiece::DestructionMethod& method);	
	LevelPiece* CollisionOccurred(GameModel* gameModel, GameBall& ball);
    LevelPiece* CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle);
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

    const BreakablePieceType& GetBreakablePieceType() const { return this->pieceType; }

protected:
	static const int PIECE_STARTING_LIFE_POINTS = 100;	// Starting life points given to a breakable block
	static const int POINTS_ON_BLOCK_DESTROYED	= 100;  // Points obtained when you destory a breakable block
	
    static const double ALLOWABLE_TIME_BETWEEN_BALL_COLLISIONS_IN_MS;

	float currLifePoints;	// Current life points of this block
	BreakablePieceType pieceType;

    long timeOfLastBallCollision;

	static BreakablePieceType GetDecrementedPieceType(BreakablePieceType breakableType);
	static Colour GetColourOfBreakableType(BreakablePieceType breakableType);

	// Decrements this piecetype to the next lesser breakable piece type.
	void DecrementPieceType() {
		this->pieceType = BreakableBlock::GetDecrementedPieceType(this->pieceType);
		this->colour    = ColourRGBA(BreakableBlock::GetColourOfBreakableType(this->pieceType), 1.0f);
	}

	LevelPiece* DiminishPiece(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	LevelPiece* EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel, const LevelPiece::DestructionMethod& method);

private:
    static const int MAX_FIREGLOB_COUNTDOWNS_WITHOUT_DROP = 2;

	double fireGlobDropCountdown;	           // Used to keep track of when to drop the next fire glob (when the block is on fire)
    int numFireGlobCountdownsWithoutDrop;      // Tracks the number of times that the fire glob countdown has happened

	DISALLOW_COPY_AND_ASSIGN(BreakableBlock);
};

/**
 * Called as this piece is being hit by the given beam over the given amount of time in seconds.
 */
inline LevelPiece* BreakableBlock::TickBeamCollision(double dT, const BeamSegment* beamSegment,
                                                     GameModel* gameModel) {
	assert(beamSegment != NULL);
	assert(gameModel != NULL);

	// If the piece is frozen in ice we don't hurt it, instead it will refract the laser beams...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		return this;
	}
    return this->EatAwayAtPiece(dT, beamSegment->GetDamagePerSecond(),
        gameModel, LevelPiece::LaserBeamDestruction);
}

/**
 * Tick the collision with the paddle shield - the shield will eat away at the block until it's destroyed.
 * Returns: The block that this block is/has become.
 */
inline LevelPiece* BreakableBlock::TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle,
                                                             GameModel* gameModel) {
	assert(gameModel != NULL);
    return this->EatAwayAtPiece(dT, paddle.GetShieldDamagePerSecond(),
        gameModel, LevelPiece::PaddleShieldDestruction);
}

inline void BreakableBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                         const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                         const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                         const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

    LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
        topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

#endif