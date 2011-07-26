/**
 * InCannonBallState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __INCANNONBALLSTATE_H__
#define __INCANNONBALLSTATE_H__

#include "BallState.h"

class CannonBlock;
class GameModel;

/**
 * Ball state that occurs when the ball has been lodged inside a cannon block and
 * is rotating inside the block preparing to fire in some random direction.
 * The state transition for this is typically:
 * NormalBallState -> InCannonBallState -> NormalBallState
 */
class InCannonBallState : public BallState {
public:
	InCannonBallState(GameBall* ball, CannonBlock* cannonBlock, BallState* prevState);
	~InCannonBallState();

	BallState* Clone(GameBall* newBall) const;
	void Tick(double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel);
	ColourRGBA GetBallColour() const;
	const CannonBlock* GetCannonBlock() const;

	BallState::BallStateType GetBallStateType() const;

	BallState* GetPreviousState() const;

private:
	CannonBlock* cannonBlock;
	BallState* previousState;

};

// The ball is invisible while in the cannon
inline ColourRGBA InCannonBallState::GetBallColour() const {
	return ColourRGBA(0, 0, 0, 0);
}

inline const CannonBlock* InCannonBallState::GetCannonBlock() const {
	return this->cannonBlock;
}

inline BallState::BallStateType InCannonBallState::GetBallStateType() const {
	return BallState::InCannonBallState;
}

inline BallState* InCannonBallState::GetPreviousState() const {
	return this->previousState;
}

#endif // __INCANNONBALLSTATE_H__