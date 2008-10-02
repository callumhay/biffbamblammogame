#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../Utils/Point.h"
#include "../Utils/Vector.h"
#include "../Utils/Shape2D.h"

class GameBall {

public:
	enum BallSpeed {ZeroSpeed = 0, SlowSpeed = 10, NormalSpeed = 17, FastSpeed = 22};

private:
	Circle2D bounds;			// The bounds of the ball, constantly updated to world space
	Vector2D currDir;			// The current direction of movement of the ball
	BallSpeed currSpeed;	// The current speed of the ball

public:
	// Minimum angle the ball can be reflected at
	static const float MIN_BALL_ANGLE_IN_DEGS;
	static const float MIN_BALL_ANGLE_IN_RADS;

	static const Vector2D STD_INIT_VEL_DIR;
	// The largest possible random angle of release (for the ball) from a still paddle, in degrees
	static const int STILL_RAND_RELEASE_DEG  = 20;
	static const int MOVING_RAND_RELEASE_DEG = 10;

	GameBall();
	~GameBall();

	Circle2D GetBounds() const {
		return this->bounds;
	}

	// Set the center position of the ball
	void SetCenterPosition(const Point2D& p) {
		this->bounds.SetCenter(p);
	}

	// Obtain the current velocity of ball
	Vector2D GetVelocity() const {
		return static_cast<float>(this->currSpeed) * this->currDir;
	}
	// Obtain the current speed of the ball
	BallSpeed GetSpeed() const {
		return this->currSpeed;
	}

	// Set the velocity of the ball; (0, 1) is up and (1, 0) is right
	void SetVelocity(const BallSpeed &magnitude, const Vector2D& dir) {
		this->currDir = Vector2D(dir);
		this->currSpeed = magnitude;
	}
	void SetSpeed(const BallSpeed &spd) {
		this->currSpeed = spd;
	}

	void Tick(double seconds) {
		Vector2D dDist = (static_cast<float>(seconds) * static_cast<float>(this->currSpeed) *this->currDir);
		this->bounds.SetCenter(this->bounds.Center() + dDist);
	}

};

#endif