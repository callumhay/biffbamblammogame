#include "GameBall.h"
#include "../Utils/Algebra.h"

// Minimum angle a ball can be to the normal when it comes off something
const float GameBall::MIN_BALL_ANGLE_IN_DEGS = 15.0f;
const float GameBall::MIN_BALL_ANGLE_IN_RADS = Trig::degreesToRadians(MIN_BALL_ANGLE_IN_DEGS);
// TODO: have a max angle as well??

// Typical initial velocity for the ball when released from the player paddle
const Vector2D GameBall::STD_INIT_VEL_DIR = Vector2D(0, GameBall::NormalSpeed);

GameBall::GameBall() : bounds(Point2D(0.0f,0.0f), 0.5f), velocity(Vector2D(0.0f, 0.0f)), currSpeed(GameBall::ZeroSpeed){
}

GameBall::~GameBall() {
}
