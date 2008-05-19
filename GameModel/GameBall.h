#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../Utils/Point.h"
#include "../Utils/Vector.h"
#include "../Utils/Shape2D.h"

class GameBall {

private:
	Circle2D bounds;				// The bounds of the ball, constantly updated to world space
	Vector2D velocity;			// The velocity of the ball in game units / second

public:
	// The standard initial velocity of the ball, when the player releases it
	// from the paddle at the start
	static const float STD_INIT_SPEED;
	static const Vector2D STD_INIT_VEL;
	// The largest possible random angle of release (for the ball) from a still paddle, in degrees
	static const int RAND_DEG_ANG = 30;

	GameBall();
	~GameBall();

	Circle2D GetBounds() const {
		return this->bounds;
	}

	// Set the center position of the ball
	void SetCenterPosition(const Point2D& p) {
		this->bounds.SetCenter(p);
	}

	Vector2D GetVelocity() const {
		return this->velocity;
	}

	// Set the velocity of the ball; (0, 1) is up and (1, 0) is right
	void SetVelocity(const Vector2D& v) {
		this->velocity = v;
	}

	void Tick(double seconds) {
		Vector2D dDist = (static_cast<float>(seconds) * this->velocity);
		this->bounds.SetCenter(this->bounds.Center() + dDist);
	}

};

#endif