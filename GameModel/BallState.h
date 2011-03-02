/**
 * BallState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
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
	virtual void Tick(double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel) = 0;
	virtual ColourRGBA GetBallColour() const = 0;
	virtual const CannonBlock* GetCannonBlock() const;

	virtual BallState::BallStateType GetBallStateType() const = 0;

protected:
	GameBall* gameBall;

};

inline const CannonBlock* BallState::GetCannonBlock() const {
	return NULL;
}

#endif // __BALLSTATE_H__