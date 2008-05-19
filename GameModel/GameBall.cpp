#include "GameBall.h"
#include "../Utils/Algebra.h"

const float GameBall::STD_INIT_SPEED = 10.0f;
const Vector2D GameBall::STD_INIT_VEL = Vector2D(0, STD_INIT_SPEED);

GameBall::GameBall() : bounds(Point2D(0.0f,0.0f), 0.5f), velocity(Vector2D(0.0f, 0.0f)){
}

GameBall::~GameBall() {
}
