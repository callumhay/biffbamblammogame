#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../BlammoEngine/BlammoEngine.h"

class GameBall {

public:
	enum BallSpeed { ZeroSpeed = 0, SlowSpeed = 12, NormalSpeed = 17, FastSpeed = 24 };
	enum BallSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };
	enum BallType { NormalBall = 0x00000000, UberBall = 0x00000001, InvisiBall = 0x00000010, GhostBall = 0x00000100 };
	
private:
	Collision::Circle2D bounds;	// The bounds of the ball, constantly updated to world space
	Vector2D currDir;						// The current direction of movement of the ball
	BallSpeed currSpeed;				// The current speed of the ball
	int currType;								// The current type of this ball

	BallSize currSize;					// The current size of this ball
	float currScaleFactor;			// The scale difference between the ball's current size and its default size

	static const float DEFAULT_BALL_RADIUS;			// Default radius of the ball
	static const float MAX_ROATATION_SPEED;			// Speed of rotation in degrees/sec
	static const float SECONDS_TO_CHANGE_SIZE;	// Number of seconds for the ball to grow/shrink
	static const float RADIUS_DIFF_PER_SIZE;		// The difference in radius per size change of the ball

	Vector3D rotationInDegs;	// Used for random rotation of the ball, gives the view the option to use it

	void SetDimensions(float newScaleFactor);
	void SetDimensions(GameBall::BallSize size);
	void SetBallSize(GameBall::BallSize size);

public:
	// Minimum angle the ball can be reflected at
	static const float MIN_BALL_ANGLE_IN_DEGS;
	static const float MIN_BALL_ANGLE_IN_RADS;

	static const Vector2D STD_INIT_VEL_DIR;
	// The largest possible random angle of release (for the ball) from a still paddle, in degrees
	static const int STILL_RAND_RELEASE_DEG  = 20;
	static const int MOVING_RAND_RELEASE_DEG = 10;

	GameBall();
	GameBall(const GameBall& gameBall);
	~GameBall();

	void ResetBallAttributes();

	Vector3D GetRotation() const {
		return this->rotationInDegs;
	}

	Collision::Circle2D GetBounds() const {
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

	// Ball size modifying / querying functions
	BallSize GetBallSize() const {
		return this->currSize;
	}
	float GetBallScaleFactor() const {
		return this->currScaleFactor;
	}

	/**
	 * Increases the paddle size if it can.
	 * Returns: true if there was an increase in size, false otherwise.
	 */
	bool IncreaseBallSize() {
		if (this->currSize == BiggestSize) { 
			return false; 
		}
		
		this->SetBallSize(static_cast<BallSize>(this->currSize + 1));
		return true;
	}

	/**
	 * Decreases the paddle size if it can.
	 * Returns: true if there was an decrease in size, false otherwise.
	 */
	bool DecreaseBallSize() {
		if (this->currSize == SmallestSize) { 
			return false; 
		}

		this->SetBallSize(static_cast<BallSize>(this->currSize - 1));
		return true;
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

	void Tick(double seconds);
};

#endif