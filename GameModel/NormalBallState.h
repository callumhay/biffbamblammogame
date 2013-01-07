/**
 * NormalBallState.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __NORMALBALLSTATE_H__
#define __NORMALBALLSTATE_H__

#include "BallState.h"
#include "GameBall.h"

class GameModel;

// The state that the game ball is usually in - does its normal tick where the ball
// travels around the level based on its velocity
class NormalBallState : public BallState {
public:
	NormalBallState(GameBall* ball);
	~NormalBallState();

	BallState* Clone(GameBall* newBall) const;
	void Tick(double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel);
	ColourRGBA GetBallColour() const;

	BallState::BallStateType GetBallStateType() const;

private:
    static const double OMNI_BULLET_WAIT_TIME_IN_SECS;
    static const int MAX_BULLETS_AT_A_TIME;

    double timeSinceLastOmniBullets;
	void ApplyCrazyBallVelocityChange(double dT, Vector2D& currVelocity, GameModel* gameModel);
    void AttemptFireOfOmniBullets(double dT, GameModel* gameModel);

    DISALLOW_COPY_AND_ASSIGN(NormalBallState);
};

// Return the typical ball colour
inline ColourRGBA NormalBallState::GetBallColour() const {
	return this->gameBall->contributingIceColour * this->gameBall->contributingCrazyColour * 
				 this->gameBall->contributingGravityColour * this->gameBall->colour;
}

inline BallState::BallStateType NormalBallState::GetBallStateType() const {
	return BallState::NormalBallState;
}

#endif // __NORMALBALLSTATE_H__