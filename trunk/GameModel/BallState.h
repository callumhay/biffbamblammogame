#ifndef __BALLSTATE_H__
#define __BALLSTATE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Colour.h"

class GameBall;
class Vector2D;
class CannonBlock;

// Abstract Superclass for the game ball's various states
class BallState {
public:
	enum BallStateType { NormalBallState, InCannonBallState };

	BallState(GameBall* gameBall) : gameBall(gameBall) { assert(gameBall != NULL); }
	virtual ~BallState() {}


	virtual BallState* Clone(GameBall* newBall) const = 0;
	virtual void Tick(double seconds, const Vector2D& worldSpaceGravityDir) = 0;
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