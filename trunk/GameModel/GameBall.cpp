#include "GameBall.h"
#include "../BlammoEngine/BlammoEngine.h"

// Minimum angle a ball can be to the normal when it comes off something
const float GameBall::MIN_BALL_ANGLE_IN_DEGS = 15.0f;
const float GameBall::MIN_BALL_ANGLE_IN_RADS = Trig::degreesToRadians(MIN_BALL_ANGLE_IN_DEGS);
// TODO: have a max angle as well?? ... YES PROBABLY!

const float GameBall::MAX_ROATATION_SPEED	= 70;

// Typical initial velocity for the ball when released from the player paddle
const Vector2D GameBall::STD_INIT_VEL_DIR = Vector2D(0, GameBall::NormalSpeed);

GameBall::GameBall() : bounds(Point2D(0.0f,0.0f), 0.5f), currDir(Vector2D(0.0f, 0.0f)), currSpeed(GameBall::ZeroSpeed),
currType(GameBall::NormalBall), rotationInDegs(0.0f, 0.0f, 0.0f) {
}

GameBall::GameBall(const GameBall& gameBall) : bounds(gameBall.bounds), currDir(gameBall.currDir), currSpeed(gameBall.currSpeed), 
currType(gameBall.currType), rotationInDegs(gameBall.rotationInDegs) {
}

GameBall::~GameBall() {
}
