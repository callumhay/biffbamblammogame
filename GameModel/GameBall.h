/**
 * GameBall.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */


#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Collision.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "Onomatoplex.h"
#include "BallState.h"
#include "GameModelConstants.h"

class LevelPiece;
class CannonBlock;

class GameBall {
	// State Machine Design Pattern Friendships
	friend class NormalBallState;
	friend class InCannonBallState;

public:
	enum BallSpeed { ZeroSpeed = 0, SlowestSpeed = 7, SlowSpeed = 12, NormalSpeed = 17, FastSpeed = 22, FastestSpeed = 27 };
	enum BallSize  { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };
	enum BallType  { NormalBall = 0x00000000, UberBall = 0x00000001,  InvisiBall = 0x00000002, GhostBall = 0x00000004, 
		               GraviBall = 0x00000008,  CrazyBall = 0x00000010, FireBall = 0x00000020 };
	
	// Default radius of the ball
	static const float DEFAULT_BALL_RADIUS;

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

	static void SetBallCamera(GameBall* ballCamBall) { 
		GameBall::currBallCamBall = ballCamBall; 
	}
	static bool GetIsBallCameraOn() { return (GameBall::currBallCamBall != NULL); }
	static const GameBall* GetBallCameraBall() { return GameBall::currBallCamBall; }

	// Ball colour set/get functions
	ColourRGBA GetColour() const {
		return this->currState->GetBallColour();
	}
	void SetColour(const ColourRGBA& c) {
		this->colour = c;
	}
	void SetAlpha(float alpha) {
		this->colour[3] = alpha;
	}
	void AnimateFade(bool fadeOut, double duration);

	Vector3D GetRotation() const {
		return this->rotationInDegs;
	}

	const Collision::Circle2D& GetBounds() const {
		return this->bounds;
	}

	void BallCollided() {
		this->timeSinceLastCollision = 0.0;
	}
	double GetTimeSinceLastCollision() {
		return this->timeSinceLastCollision;
	}

	/**
	 * Whether or not this ball can collide with other balls.
	 */
	bool CanCollideWithOtherBalls() const {
		return this->ballballCollisionsDisabledTimer <= EPSILON;
	}

	/**
	 * Check to see if this ball collided with another.
	 * Returns: true if there was a collision between this ball and otherball, false otherwise.
	 */
	bool CollisionCheck(const GameBall& otherBall) const {
		// If the collisions are disabled then we only return false
		if (!this->CanCollideWithOtherBalls() || !otherBall.CanCollideWithOtherBalls()) {
			return false;
		}

		return this->CollisionCheck(otherBall.GetBounds());
	}

	bool CollisionCheck(const Collision::Circle2D& circle) const {
		Vector2D lengthVec = this->bounds.Center() - circle.Center();
		float sqLength = Vector2D::Dot(lengthVec, lengthVec);
		float radiiSum = this->bounds.Radius() + circle.Radius();
		float sqRadii = radiiSum * radiiSum;
		return sqLength < sqRadii;
	}

	/** 
	 * Set ball-ball collisions for this ball to be diabled for the given duration in seconds.
	 */
	void SetBallBallCollisionsDisabled(double durationInSecs) {
		assert(durationInSecs > EPSILON);
		this->ballballCollisionsDisabledTimer = durationInSecs;
	}
	void SetBallBallCollisionsDisabled() {
		// Set the ball collisions to be disabled forever (until they are re-enabled)
		this->ballballCollisionsDisabledTimer = DBL_MAX;
	}
	void SetBallBallCollisionsEnabled() {
		this->ballballCollisionsDisabledTimer = 0.0;
	}

	void SetBallPaddleCollisionsDisabled() {
		this->paddleCollisionsDisabled = true;
	}
	void SetBallPaddleCollisionsEnabled() {
		this->paddleCollisionsDisabled = false;
	}
	bool CanCollideWithPaddles() const {
		return !this->paddleCollisionsDisabled;
	}
	
	void SetBallBlockCollisionsDisabled() {
		this->blockCollisionsDisabled = true;
	}
	void SetBallBlockCollisionsEnabled() {
		this->blockCollisionsDisabled = false;
	}
	bool CanCollideWithBlocks() const {
		return !this->blockCollisionsDisabled;
	}



	// Set the center position of the ball
	void SetCenterPosition(const Point2D& p) {
		this->bounds.SetCenter(p);
		this->zCenterPos = 0.0f;
	}

	// Occasionally we need to see the full 3D position of the ball
	void SetCenterPosition(const Point3D& p) {
		this->bounds.SetCenter(Point2D(p[0], p[1]));
		this->zCenterPos = p[2];
	}
	Point3D GetCenterPosition() const {
		Point2D center2D = this->bounds.Center();
		return Point3D(center2D[0], center2D[1], this->zCenterPos);
	}
	const Point2D& GetCenterPosition2D() const {
		return this->bounds.Center();
	}

	const Vector2D& GetDirection() const {
		return this->currDir;
	}
	void SetDirection(const Vector2D& dir) {
		assert(fabs(dir.Magnitude() - 1.0f) < EPSILON);
		this->currDir = dir;
	}

	// Obtain the current velocity of ball
	Vector2D GetVelocity() const {
		return this->currSpeed * this->currDir;
	}
	// Obtain the current speed of the ball
	float GetSpeed() const {
		return this->currSpeed;
	}
	// Set the current speed of this ball
	void SetSpeed(float speed) {
		this->currSpeed    = speed;
		this->gravitySpeed = speed;
	}

	uint32_t GetBallType() const {
		return this->currType;
	}
	void RemoveAllBallTypes() {
		this->currType = GameBall::NormalBall;
		this->contributingGravityColour = Colour(1.0f, 1.0f, 1.0f);
		this->contributingCrazyColour = Colour(1.0f, 1.0f, 1.0f);
	}
	void AddBallType(const BallType type) {
		this->currType = this->currType | type;
		switch (type) {
			case GameBall::GraviBall:
				this->contributingGravityColour = GameModelConstants::GetInstance()->GRAVITY_BALL_COLOUR;
				break;
			case GameBall::CrazyBall:
				this->contributingCrazyColour = GameModelConstants::GetInstance()->CRAZY_BALL_COLOUR;
			default:
				break;
		}
	}
	void RemoveBallType(const BallType type) {
		this->currType = this->currType & ~type;
		switch (type) {
			case GameBall::GraviBall:
				this->contributingGravityColour = Colour(1.0f, 1.0f, 1.0f);
				break;
			case GameBall::CrazyBall:
				this->contributingCrazyColour = Colour(1.0f, 1.0f, 1.0f);
			default:
				break;
		}
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
		this->currDir = dir;
		// If the ball is travelling directly sideways we should nudge it a bit up,
		// just to make sure it doesn't go sideways forever
		if (this->currDir == Vector2D(1, 0) || this->currDir == Vector2D(-1, 0)) {
			this->currDir = Vector2D(this->currDir[0], 0.01);
			this->currDir.Normalize();
		}

		this->SetSpeed(magnitude);
	}
	void SetVelocity(float magnitude, const Vector2D& dir) {
		this->currDir = dir;
		this->SetSpeed(magnitude);
	}

	void SetGravityVelocity(float magnitude, const Vector2D& dir) {
		this->currDir = dir;
		this->gravitySpeed = magnitude;
	}

	// Increases the Speed of the ball
	void IncreaseSpeed() {
		this->SetSpeed(std::min<float>(GameBall::FastestSpeed, this->currSpeed + 5.0f));
	}

	// Decreases the speed of the ball
	void DecreaseSpeed() {
		this->SetSpeed(std::max<float>(GameBall::SlowestSpeed, this->currSpeed - 5.0f));
	}

	Onomatoplex::Extremeness GetOnomatoplexExtremeness() const;

	void Tick(double seconds, const Vector2D& worldSpaceGravityDir);
	void Animate(double seconds);

	// Set and get for the last level piece that this ball collided with
	void SetLastPieceCollidedWith(const LevelPiece* p) { 
		this->lastThingCollidedWith = p; 
	}
	void SetLastThingCollidedWith(const void* p) {
		this->lastThingCollidedWith = p;
	}
	bool IsLastPieceCollidedWith(const LevelPiece* p) const { 
		return this->lastThingCollidedWith == p; 
	}
	bool IsLastThingCollidedWith(const void* p) const {
		return (this->lastThingCollidedWith == p);
	}

	void LoadIntoCannonBlock(CannonBlock* cannonBlock);
	bool IsLoadedInCannonBlock() const;
	const CannonBlock* GetCannonBlock() const;

private:
	BallState* currState;

	Collision::Circle2D bounds;	// The bounds of the ball, constantly updated to world space
	float zCenterPos;						// VERY occasionally (some animations), the ball needs a z-coordinate

	Vector2D currDir;				// The current direction of movement of the ball
	float currSpeed;				// The current speed of the ball
	float gravitySpeed;			// The current gravity speed of the ball
	uint32_t currType;			// The current type of this ball

	BallSize currSize;					// The current size of this ball
	float currScaleFactor;			// The scale difference between the ball's current size and its default size

	double ballballCollisionsDisabledTimer;	// If > 0 then collisions among this ball and other balls are disabled
	bool blockCollisionsDisabled;
	bool paddleCollisionsDisabled;

	double timeSinceLastCollision;

	ColourRGBA colour;													// The colour multiply of the paddle, including its visibility/alpha
	Colour contributingGravityColour;
	Colour contributingCrazyColour;
	AnimationLerp<ColourRGBA> colourAnimation;	// Animations associated with the colour

	static GameBall* currBallCamBall;	// The current ball that has the ball camera active on it, if none then NULL

	const void* lastThingCollidedWith;	// This is just used to check equality with POINTERS DO NOT CAST THIS!!!

	static const float MAX_ROATATION_SPEED;			// Speed of rotation in degrees/sec
	static const float SECONDS_TO_CHANGE_SIZE;	// Number of seconds for the ball to grow/shrink
	static const float RADIUS_DIFF_PER_SIZE;		// The difference in radius per size change of the ball
	static const float GRAVITY_ACCELERATION;		// Acceleration of gravity pulling the ball downwards

	Vector3D rotationInDegs;	// Used for random rotation of the ball, gives the view the option to use it

	void SetDimensions(float newScaleFactor);
	void SetDimensions(GameBall::BallSize size);
	void SetBallSize(GameBall::BallSize size);
	void SetBallState(BallState* state, bool deletePrevState);

};

inline const CannonBlock* GameBall::GetCannonBlock() const {
	return this->currState->GetCannonBlock();
}	

inline bool GameBall::IsLoadedInCannonBlock() const {
	return this->currState->GetBallStateType() == BallState::InCannonBallState;
}

#endif