/**
 * GameBall.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __GAMEBALL_H__
#define __GAMEBALL_H__

#include "../BlammoEngine/IPositionObject.h"
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
class GameModel;
class GameLevel;
class Projectile;
class BoundingLines;

class GameBall : public IPositionObject {
	// State Machine Design Pattern Friendships
	friend class NormalBallState;
	friend class InCannonBallState;

public:
    static const float DEFAULT_NORMAL_SPEED;
    static void SetNormalSpeed(float speed);
    static float GetZeroSpeed() { return GameBall::ZeroSpeed; }
    static float GetSlowestSpeed() { return GameBall::SlowestSpeed; }
    static float GetNormalSpeed() { return GameBall::NormalSpeed; }
    static float GetSlowestAllowableSpeed() { return 0.25f * GameBall::SlowestSpeed; }

	enum BallSize  { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };
	enum BallType  { NormalBall = 0x00000000, UberBall = 0x00000001,  InvisiBall = 0x00000002, GhostBall = 0x00000004, 
		             GraviBall = 0x00000008,  CrazyBall = 0x00000010, FireBall = 0x00000020, IceBall = 0x00000040,
                     OmniLaserBulletBall = 0x00000080, FastBall = 0x00000100, SlowBall = 0x00000200 };
	
	// Default radius of the ball
	static const float DEFAULT_BALL_RADIUS;

	// Minimum angle the ball can be reflected at
    static const float MIN_BALL_ANGLE_ON_PADDLE_HIT_IN_DEGS;
	static const float MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_DEGS;
	static const float MIN_BALL_ANGLE_ON_BLOCK_HIT_IN_RADS;
    static const float MAX_GRACING_ANGLE_ON_HIT_IN_DEGS;

	// The largest possible random angle of release (for the ball) from a still paddle, in degrees
	static const int STILL_RAND_RELEASE_DEG  = 3;
	static const int MOVING_RAND_RELEASE_DEG = 3;

	GameBall();
	GameBall(const GameBall& gameBall);
	~GameBall();

	void ResetBallAttributes();

	static void SetBallCamera(GameBall* ballCamBall, const GameLevel* currLevel);
	static bool GetIsBallCameraOn() { return (GameBall::currBallCamBall != NULL); }
	static const GameBall* GetBallCameraBall() { return GameBall::currBallCamBall; }
    bool HasBallCameraActive() const { return this == GameBall::currBallCamBall; }

    bool CanShootBallCamOutOfCannon(const CannonBlock& cannon, const GameLevel& currLevel) const;

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
    float GetAlpha() const {
        return this->colour[3];
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
	double GetTimeSinceLastCollision() const {
		return this->timeSinceLastCollision;
	}

    float GetCollisionDamage() const;
    float GetDamageMultiplier() const;

	/**
	 * Whether or not this ball can collide with other balls.
	 */
	bool CanCollideWithOtherBalls() const {
		return this->ballballCollisionsDisabledTimer <= 0.0;
	}

    bool CollisionCheck(double dT, const GameBall& otherBall, Vector2D& n, 
        Collision::LineSeg2D& collisionLine, double& timeUntilCollision, 
        Point2D& otherBallCenterAtCollision, Point2D& thisBallCenterAtCollision) const;

    bool CollisionCheckWithProjectile(double dT, const Projectile& projectile, const BoundingLines& bounds) const;
    bool ProjectileIsDestroyedOnCollision(const Projectile& projectile) const;
    void ModifyProjectileTrajectory(Projectile& projectile) const;

    void ApplyImpulseForce(float impulseAmt, float deceleration);
    //void ApplyNewtonsThirdLawCollisionVelocity(const Vector2D& impulse);

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
	
	void SetBallBlockAndBossCollisionsDisabled() {
		this->blockAndBossCollisionsDisabled = true;
	}
	void SetBallBlockAndBossCollisionsEnabled() {
		this->blockAndBossCollisionsDisabled = false;
	}
	bool CanCollideWithBlocksAndBosses() const {
		return !this->blockAndBossCollisionsDisabled;
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

    // From the IPositionObject interface
    Point3D GetPosition3D() const {
        return this->GetCenterPosition();
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
	void SetSpeed(float speed);

    void TurnOffBoost() {
        this->boostSpdDecreaseCounter = BOOST_TEMP_SPD_INCREASE_AMT;
    }
    void TurnOffImpulse() {
        this->impulseAmount = 0;
        this->impulseDeceleration = 0;
        this->impulseSpdDecreaseCounter = 0;
    }

	int32_t GetBallType() const {
		return this->currType;
	}
	void RemoveAllBallTypes() {
		this->currType = GameBall::NormalBall;
		this->contributingGravityColour = Colour(1.0f, 1.0f, 1.0f);
		this->contributingCrazyColour		= Colour(1.0f, 1.0f, 1.0f);
		this->contributingIceColour			= Colour(1.0f, 1.0f, 1.0f);
	}
	void AddBallType(const BallType& type) {
		this->currType = this->currType | type;
		switch (type) {
			case GameBall::GraviBall:
				this->contributingGravityColour = GameModelConstants::GetInstance()->GRAVITY_BALL_COLOUR;
				break;
			case GameBall::CrazyBall:
				this->contributingCrazyColour = GameModelConstants::GetInstance()->CRAZY_BALL_COLOUR;
				break;
			case GameBall::IceBall:
				this->contributingIceColour = GameModelConstants::GetInstance()->ICE_BALL_COLOUR;
				break;
			default:
				break;
		}
	}
	void RemoveBallType(const BallType& type) {
		this->currType = this->currType & ~type;
		switch (type) {
			case GameBall::GraviBall:
				this->contributingGravityColour = Colour(1.0f, 1.0f, 1.0f);
				break;
			case GameBall::CrazyBall:
				this->contributingCrazyColour = Colour(1.0f, 1.0f, 1.0f);
				break;
			case GameBall::IceBall:
				this->contributingIceColour = Colour(1.0f, 1.0f, 1.0f);
				break;
			default:
				break;
		}
	}
    bool HasBallType(int32_t type) const { return ((this->currType & type) != 0x0); }

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
	void SetVelocity(float magnitude, const Vector2D& dir) {
		this->currDir = dir;
		// If the ball is travelling directly sideways we should nudge it a bit up,
		// just to make sure it doesn't go sideways forever
		if (this->currDir == Vector2D(1, 0) || this->currDir == Vector2D(-1, 0)) {
			this->currDir = Vector2D(this->currDir[0], 0.01);
			this->currDir.Normalize();
		}

		this->SetSpeed(magnitude);
	}

	void SetGravityVelocity(float magnitude, const Vector2D& dir) {
		this->currDir = dir;
		this->gravitySpeed = magnitude;
	}

	// Increases the Speed of the ball
	void IncreaseSpeed() {
		this->SetSpeed(this->currSpeed + INCREMENT_SPD_AMT);
	}

	// Decreases the speed of the ball
	void DecreaseSpeed() {
		this->SetSpeed(std::max<float>(GameBall::SlowestSpeed, this->currSpeed - INCREMENT_SPD_AMT));
	}

	Onomatoplex::Extremeness GetOnomatoplexExtremeness() const;

	void Tick(bool simulateMovement, double seconds, const Vector2D& worldSpaceGravityDir, GameModel* gameModel);
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
	CannonBlock* GetCannonBlock() const;

    bool IsBallAllowedToBoost() const;
    bool IsBallAllowedToBoostIgnoreAlreadyBoosting() const;
    bool ExecuteBallBoost(const Vector2D& dir);
    bool IsBallBoosting() const;

#ifdef _DEBUG
    void DebugDraw() const;
#endif

private:
    static const float INCREMENT_SPD_AMT;

    static float ZeroSpeed;
    static float SlowestSpeed;
    static float SlowSpeed;
    static float NormalSpeed;
    static float FastSpeed;
    static float FastestSpeed;
    static float FastestSpeedWithBoost;

    static GameBall* currBallCamBall;	// The current ball that has the ball camera active on it, if none then NULL
	BallState* currState;
    AnimationLerp<ColourRGBA> colourAnimation;	// Animations associated with the colour

    // The colour multiply of the paddle, including its visibility/alpha
	ColourRGBA colour;
	Colour contributingGravityColour;
	Colour contributingCrazyColour;
	Colour contributingIceColour;
    
    Collision::Circle2D bounds;	// The bounds of the ball, constantly updated to world space
	
	Vector2D currDir;           // The current direction of movement of the ball
	float currSpeed;            // The current speed of the ball
	float gravitySpeed;			// The current gravity speed of the ball
	int32_t currType;           // The current type of this ball

	BallSize currSize;          // The current size of this ball
	float currScaleFactor;      // The scale difference between the ball's current size and its default size

	double ballballCollisionsDisabledTimer;	// If > 0 then collisions among this ball and other balls are disabled
	bool blockAndBossCollisionsDisabled;
	bool paddleCollisionsDisabled;

	double timeSinceLastCollision;

    float zCenterPos;                   // VERY occasionally (some animations), the ball needs a z-coordinate
    float boostSpdDecreaseCounter;      // Counts the boost deceleration so far, when this reaches BOOST_TEMP_SPD_INCREASE_AMT
                                        // the ball will no longer decelerate from a boost

    float impulseAmount;
    float impulseDeceleration;
    float impulseSpdDecreaseCounter;

	const void* lastThingCollidedWith;	// This is just used to check equality with POINTERS DO NOT CAST THIS!!!

	static const float MAX_ROATATION_SPEED;         // Speed of rotation in degrees/sec
	static const float SECONDS_TO_CHANGE_SIZE;	    // Number of seconds for the ball to grow/shrink
	static const float RADIUS_DIFF_PER_SIZE;        // The difference in radius per size change of the ball
	static const float GRAVITY_ACCELERATION;        // Acceleration of gravity pulling the ball downwards

    static const float BOOST_DECCELERATION;         // The decceleration after a ball is boosted
    static const float BOOST_TEMP_SPD_INCREASE_AMT; // The total speed increase given to the ball (temporarally) when it's boosted

	Vector3D rotationInDegs;	// Used for random rotation of the ball, gives the view the option to use it

	void SetDimensions(float newScaleFactor);
	void SetDimensions(GameBall::BallSize size);
	void SetBallSize(GameBall::BallSize size);
	void SetBallState(BallState* state, bool deletePrevState);

    void AugmentDirectionOnPaddleMagnet(double seconds, const GameModel& model, float degreesChangePerSec);

};

/**
 * Get the amount of damage that this ball does when it collides with something.
 */
inline float GameBall::GetCollisionDamage() const {
    return this->GetDamageMultiplier() * GameModelConstants::GetInstance()->DEFAULT_DAMAGE_ON_BALL_HIT;
}

inline CannonBlock* GameBall::GetCannonBlock() const {
	return this->currState->GetCannonBlock();
}	

inline bool GameBall::IsLoadedInCannonBlock() const {
	return this->currState->GetBallStateType() == BallState::InCannonBallState;
}

/**
 * Queries whether the ball is allowed to boost or not based on its current state.
 * Returns: true if allowed to boost, false otherwise.
 */
inline bool GameBall::IsBallAllowedToBoost() const {
    // We don't allow boosting if the ball is already being boosted...
    return (!this->IsBallBoosting() && this->IsBallAllowedToBoostIgnoreAlreadyBoosting());
}

inline bool GameBall::IsBallAllowedToBoostIgnoreAlreadyBoosting() const {
    return (!this->IsLoadedInCannonBlock() && !this->GetIsBallCameraOn() &&
            !this->blockAndBossCollisionsDisabled && !this->paddleCollisionsDisabled &&
            !currDir.IsZero() && this->GetSpeed() != 0.0f);
}

/**
 * Causes a ball boost based on the given direction (it will boost in the same direction as the one given).
 */
inline bool GameBall::ExecuteBallBoost(const Vector2D& dir) {
    if (!this->IsBallAllowedToBoost()) {
        return false;
    }

    // Add a temporary boost in speed and reset the boost decceleration counter
    this->boostSpdDecreaseCounter = 0.0f; // This value will need to accumulate back up to BOOST_TEMP_SPD_INCREASE_AMT
                                          // So that the ball knows to stop decelerating after being boosted
    this->SetVelocity(this->GetSpeed() + BOOST_TEMP_SPD_INCREASE_AMT, Vector2D::Normalize(dir));
    return true;
}

inline bool GameBall::IsBallBoosting() const {
    return (this->boostSpdDecreaseCounter < BOOST_TEMP_SPD_INCREASE_AMT);
}

#endif