#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../BlammoEngine/BlammoEngine.h"

class GameBall {

public:
	enum BallSpeed { ZeroSpeed = 0, SlowSpeed = 10, NormalSpeed = 16, FastSpeed = 22 };
	enum BallType { NormalBall = 0x00000000, UberBall = 0x00000001, InvisiBall = 0x00000010 };

private:
	Circle2D bounds;			// The bounds of the ball, constantly updated to world space
	Vector2D currDir;			// The current direction of movement of the ball
	BallSpeed currSpeed;	// The current speed of the ball
	int currType;					// The current type of this ball
	
	static const float MAX_ROATATION_SPEED;	// Speed of rotation in degrees/sec
	Vector3D rotationInDegs;	// Used for random rotation of the ball, gives the view the option to use it

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

	Vector3D GetRotation() const {
		return this->rotationInDegs;
	}

	Circle2D GetBounds() const {
		return this->bounds;
	}

	// Set the center position of the ball
	void SetCenterPosition(const Point2D& p) {
		this->bounds.SetCenter(p);
	}

	Vector2D GetDirection() const {
		return this->currDir;
	}
	// Obtain the current velocity of ball
	Vector2D GetVelocity() const {
		return static_cast<float>(this->currSpeed) * this->currDir;
	}
	// Obtain the current speed of the ball
	BallSpeed GetSpeed() const {
		return this->currSpeed;
	}
	// Set the current speed of this ball
	void SetSpeed(const BallSpeed speed) {
		this->currSpeed = speed;
	}

	int GetBallType() const {
		return this->currType;
	}
	void AddBallType(const BallType type) {
		this->currType = this->currType | type;
	}
	void RemoveBallType(const BallType type) {
		this->currType = this->currType & ~type;
	}

	// Set the velocity of the ball; (0, 1) is up and (1, 0) is right
	void SetVelocity(const BallSpeed &magnitude, const Vector2D& dir) {
		this->currDir = Vector2D(dir);
		this->currSpeed = magnitude;
	}

	// Increases the Speed of the ball
	void IncreaseSpeed() {
		switch (this->currSpeed) {
			case SlowSpeed :
				this->currSpeed = NormalSpeed;
				break;
			case NormalSpeed :
				this->currSpeed = FastSpeed;
				break;
			default:
				break;
		}
	}
	// Decreases the speed of the ball
	void DecreaseSpeed() {
		switch (this->currSpeed) {
			case FastSpeed :
				this->currSpeed = NormalSpeed;
				break;
			case NormalSpeed :
				this->currSpeed = SlowSpeed;
				break;
			default:
				break;
		}	
	}

	void Tick(double seconds) {
		// Update the position of the ball based on its velocity
		Vector2D dDist = (static_cast<float>(seconds) * static_cast<float>(this->currSpeed) *this->currDir);
		this->bounds.SetCenter(this->bounds.Center() + dDist);

		// Update the rotation of the ball
		float dRotSpd = GameBall::MAX_ROATATION_SPEED * static_cast<float>(seconds);
		this->rotationInDegs = this->rotationInDegs + Vector3D(dRotSpd, dRotSpd, dRotSpd);
	}

};

#endif