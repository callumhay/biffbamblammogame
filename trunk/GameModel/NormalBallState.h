#ifndef __NORMALBALLSTATE_H__
#define __NORMALBALLSTATE_H__

#include "BallState.h"
#include "GameBall.h"

// The state that the game ball is usually in - does its normal tick where the ball
// travels around the level based on its velocity
class NormalBallState : public BallState {
public:
	NormalBallState(GameBall* ball);
	~NormalBallState();

	BallState* Clone(GameBall* newBall) const;
	void Tick(double seconds, const Vector2D& worldSpaceGravityDir);
	ColourRGBA GetBallColour() const;

	BallState::BallStateType GetBallStateType() const;
//private:

};

// Return the typical ball colour
inline ColourRGBA NormalBallState::GetBallColour() const {
	return this->gameBall->contributingGravityColour * this->gameBall->colour;
}

inline BallState::BallStateType NormalBallState::GetBallStateType() const {
	return BallState::NormalBallState;
}

#endif // __NORMALBALLSTATE_H__