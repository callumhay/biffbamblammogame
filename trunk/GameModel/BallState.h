/**
 * BallState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BALLSTATE_H__
#define __BALLSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"

class GameBall;
class Vector2D;
class CannonBlock;
class GameModel;

// Abstract Superclass for the game ball's various states
class BallState {
public:
	enum BallStateType { NormalBallState, InCannonBallState };

	BallState(GameBall* gameBall) : gameBall(gameBall) { assert(gameBall != NULL); }
	virtual ~BallState() {}


	virtual BallState* Clone(GameBall* newBall) const = 0;
	virtual void Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel) = 0;
	virtual ColourRGBA GetBallColour() const = 0;
	virtual CannonBlock* GetCannonBlock() const;

	virtual BallState::BallStateType GetBallStateType() const = 0;

protected:
	GameBall* gameBall;

};

inline CannonBlock* BallState::GetCannonBlock() const {
	return NULL;
}

#endif // __BALLSTATE_H__