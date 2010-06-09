#ifndef __BALLDEATHSTATE_H__
#define __BALLDEATHSTATE_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"

#include "GameState.h"

class GameModel;
class GameBall;

/**
 * State for when the last ball goes out of bounds and is about to be destroyed -
 * this state allows for some time to show a death animation or some such thing
 * before the game state changes back to either the ball being on the paddle or
 * game over.
 */
class BallDeathState : public GameState {	
public:
	BallDeathState(GameBall* lastBallToBeAlive, GameModel* gm);
	~BallDeathState();

	void Tick(double seconds);
	void BallReleaseKeyPressed() {}
	void MovePaddleKeyPressed(float dist) {}

private:
	// Constant Ball spiraling death values
	static const double SPIRAL_ANIMATION_TIME_TOTAL;
	static const double EXPLOSION_ANIMATION_TIME_TOTAL;
	static const double BALL_FADE_TIME_TOTAL;

	// Ball starts off by spiralling to its death, then it explodes and is officially dead afterwards
	enum DeathAnimationState { SpiralingToDeath, Exploding, Dead };
	DeathAnimationState currAnimationState;

	// Values related to the last ball - which is inevidbly going to die
	GameBall* lastBallToBeAlive;
	Point2D initialBallPosition;
	Vector2D initialBallVelocityDir;
	float initialBallVelocityMag;

	// Varying Ball spiraling death values
	float spiralRadius;
	AnimationLerp<float> spiralAnimation;
	double timeElapsed;

	void ExecuteSpiralingToDeathState(double dT);
};

#endif // __BALLDEATHSTATE_H__