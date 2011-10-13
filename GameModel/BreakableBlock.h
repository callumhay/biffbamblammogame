/**
 * BreakableBlock.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __BREAKABLEBLOCK_H__
#define __BREAKABLEBLOCK_H__

#include "LevelPiece.h"
#include "Beam.h"

class BreakableBlock : public LevelPiece {
public:
	BreakableBlock(char type, unsigned int wLoc, unsigned int hLoc);
	virtual ~BreakableBlock();

	virtual LevelPieceType GetType() const { 
		return LevelPiece::Breakable;
	}

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
    bool CanChangeSelfOrOtherPiecesWhenHitByBall() const {
        return true;
    }

	// Whether or not the uber ball can just blast right through this block.
	// Returns: true if it can, false otherwise.
	virtual bool BallBlastsThrough(const GameBall& b) const {
        // The ball may blast through ONLY IF the piece is green, the ball is uber and NOT on fire / icy
        return (this->pieceType == GreenBreakable) && 
               ((b.GetBallType() & GameBall::UberBall) == GameBall::UberBall) &&
               ((b.GetBallType() & GameBall::IceBall) != GameBall::IceBall) && 
               ((b.GetBallType() & GameBall::FireBall) != GameBall::FireBall);
	}

	// Whether or not the ghost ball can just pass through this block.
	// Returns: true if it can, false otherwise.
	virtual bool GhostballPassesThrough() const {
		return true;
	}

	bool ProjectilePassesThrough(Projectile* projectile) const;
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
	LevelPiece* CollisionOccurred(GameModel* gameModel, Projectile* projectile);
	LevelPiece* TickBeamCollision(double dT, const BeamSegment* beamSegment, GameModel* gameModel);
	LevelPiece* TickPaddleShieldCollision(double dT, const PlayerPaddle& paddle, GameModel* gameModel);

	bool StatusTick(double dT, GameModel* gameModel, int32_t& removedStatuses);

protected:
	static const int PIECE_STARTING_LIFE_POINTS     = 100;	// Starting life points given to a breakable block
	static const int POINTS_ON_RED_TO_ORANGE_HIT    = 300;
    static const int POINTS_ON_ORANGE_TO_YELLOW_HIT = 200;
    static const int POINTS_ON_YELLOW_TO_GREEN_HIT  = 150;
	static const int POINTS_ON_BLOCK_DESTROYED	    = 100;   // Points obtained when you destory a breakable block

	float currLifePoints;	// Current life points of this block

	enum BreakablePieceType { GreenBreakable = 'G', YellowBreakable = 'Y', OrangeBreakable = 'O', RedBreakable = 'R' }; 
	BreakablePieceType pieceType;

	static BreakablePieceType GetDecrementedPieceType(BreakablePieceType breakableType);
	static Colour GetColourOfBreakableType(BreakablePieceType breakableType);

	// Decrements this piecetype to the next lesser breakable piece type.
	void DecrementPieceType() {
		this->pieceType = BreakableBlock::GetDecrementedPieceType(this->pieceType);
		this->colour    = BreakableBlock::GetColourOfBreakableType(this->pieceType);
	}

	LevelPiece* DiminishPiece(GameModel* gameModel, const LevelPiece::DestructionMethod& method);
	LevelPiece* EatAwayAtPiece(double dT, int dmgPerSec, GameModel* gameModel, const LevelPiece::DestructionMethod& method);

private:
	double fireGlobTimeCounter;	// Used to keep track of the amount of time this block has been on fire and
															// when it stands the chance of dropping a fire glob
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


#endif