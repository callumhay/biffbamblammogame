/**
 * PlayerPaddle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PLAYERPADDLE_H__
#define __PLAYERPADDLE_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Animation.h"

#include "BoundingLines.h"
#include "GameBall.h"
#include "Projectile.h"
#include "PaddleRocketProjectile.h"

class GameModel;
class GameBall;

// Represents the player controlled paddle shaped as follows:
//                -------------
//               /_____________\

class PlayerPaddle {
public:
	static const float PADDLE_HEIGHT_TOTAL;
	static const float PADDLE_WIDTH_TOTAL;
	static const float PADDLE_DEPTH_TOTAL;
	static const float PADDLE_HALF_WIDTH;
	static const float PADDLE_HALF_HEIGHT;
	static const float PADDLE_HALF_DEPTH;
	static const float PADDLE_WIDTH_FLAT_TOP;
	static const float DEFAULT_MAX_SPEED;
	static const float DEFAULT_ACCELERATION;
	static const float DEFAULT_DECCELERATION;
	static const float POISON_SPEED_DIMINISH;
	static const int DEFLECTION_DEGREE_ANGLE;

	static const Vector2D DEFAULT_PADDLE_UP_VECTOR;
	static const Vector2D DEFAULT_PADDLE_RIGHT_VECTOR;

	static const int DEFAULT_SHIELD_DMG_PER_SECOND;

	enum PaddleType { NormalPaddle = 0x00000000, LaserBulletPaddle = 0x00000001, PoisonPaddle = 0x00000002, 
										StickyPaddle = 0x00000004, LaserBeamPaddle = 0x00000008, RocketPaddle = 0x00000010, 
										ShieldPaddle = 0x00000020 };
	enum PaddleSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };

	PlayerPaddle(float minBound, float maxBound);
	PlayerPaddle();
	~PlayerPaddle();

	void UpdatePaddleBounds(float min, float max) {
		assert(min < max);
		assert((max - min) > PADDLE_WIDTH_TOTAL);
		this->minBound = min;
		this->maxBound = max;
	}

	void ResetPaddle();

	// Obtain the center position of this paddle.
	const Point2D& GetCenterPosition() const {
		return this->centerPos;
	}
	Point2D GetDefaultCenterPosition() const {
		return Point2D((this->maxBound + this->minBound)/2.0f, this->currHalfHeight);
	}
	void SetCenterPosition(const Point2D& center) {
		this->centerPos = center;
	}

	float GetZRotation() const {
		return this->rotAngleZAnimation.GetInterpolantValue();
	}

	float GetHalfHeight() const {
		return this->currHalfHeight;
	}
	float GetHalfWidthTotal() const {
		return this->currHalfWidthTotal;
	}
	float GetHalfFlatTopWidth() const {
		return this->currHalfWidthFlat;
	}
	float GetHalfDepthTotal() const {
		return this->currHalfDepthTotal;
	}

	int GetShieldDamagePerSecond() const {
		return PlayerPaddle::DEFAULT_SHIELD_DMG_PER_SECOND;
	}

	Vector2D GetUpVector() const {
		return Vector2D::Rotate(this->rotAngleZAnimation.GetInterpolantValue(), PlayerPaddle::DEFAULT_PADDLE_UP_VECTOR);
	}

	void Tick(double seconds, bool pausePaddleMovement);
	void Animate(double seconds);

	enum PaddleMovement { LeftPaddleMovement = -1, NoPaddleMovement = 0, RightPaddleMovement = 1 };
	void ControlPaddleMovement(const PaddleMovement& movement) {
		this->moveButtonDown = (movement != NoPaddleMovement);
		if (this->moveButtonDown) {
			this->lastDirection = static_cast<float>(movement);
		}
	}
	float GetLastMovingDirection() const {
		return this->lastDirection;
	}

	void SetNewMinMaxLevelBound(float min, float max) {
		this->UpdatePaddleBounds(min, max);
		// Reset the paddle to the center of the new bounds
		this->ResetPaddle();
	}

	float GetSpeed() const {
		return this->lastDirection * this->currSpeed;
	}

	Vector2D GetVelocity() const {
		return Vector2D(this->GetSpeed(), 0);
	}

	//float GetAverageSpeed() const {
		//return this->avgSpeed;
	//}
	//Vector2D GetAverageVelocity() const {
	//	return Vector2D(this->avgSpeed, 0);
	//}

	// Paddle size set/get functions
	const PaddleSize& GetPaddleSize() const {
		return this->currSize;
	}
	float GetPaddleScaleFactor() const {
		return this->currScaleFactor;
	}

	void ApplyImpulseForce(float xDirectionalForce);

	// Paddle colour set/get functions
	const ColourRGBA& GetColour() const {
		return this->colour;
	}
	void SetColour(const Colour& c) {
		this->colour = ColourRGBA(c, this->colour.A());
	}
	void SetVisiblity(float alpha) {
		this->colour[3] = alpha;
	}
	void AnimateFade(bool fadeOut, double duration);

	bool IncreasePaddleSize();
	bool DecreasePaddleSize();

	// Paddle type modifying / querying functions
	int32_t GetPaddleType() const { return this->currType; }

	void AddPaddleType(const PaddleType& type);
	void RemovePaddleType(const PaddleType& type);

	// Paddle camera set/get functions
	void SetPaddleCamera(bool isPaddleCamOn, double dT) {
		this->isPaddleCamActive = isPaddleCamOn;
		
		// When the paddle camera is on, we change the collision boundries to
		// be more natural to the vision of the paddle cam
		this->SetDimensions(this->currSize);
		if (!isPaddleCamOn) {
			this->MoveAttachedBallToNewBounds(dT);
		}
	}
	bool GetIsPaddleCameraOn() const {
		return this->isPaddleCamActive;
	}

	void SetIsLaserBeamFiring(bool isFiring);
	bool GetIsLaserBeamFiring() const {
		return this->isFiringBeam;
	}

	// Attach/detach ball functions
	void Shoot(GameModel* gameModel);
	bool AttachBall(GameBall* ball);
	
	/** 
	 * Whether or not this paddle has a ball currently attached to it.
	 * Returns: true if there's a ball resting on the paddle, false otherwise.
	 */
	bool HasBallAttached() const { 
		return this->attachedBall != NULL; 
	}
	GameBall* GetAttachedBall() const {
		return this->attachedBall;
	}

	void HitByProjectile(GameModel* gameModel, const Projectile& projectile);
	bool ProjectilePassesThrough(const Projectile& projectile);
	void ModifyProjectileTrajectory(Projectile& projectile);

	void UpdateBoundsByPieceCollision(const LevelPiece& p, bool doAttachedBallCollision);

	// TODO: Add the parameter: "bool includeAttachedBallCheck" to all paddle collision checks...
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const BoundingLines& bounds, bool includeAttachedBallCheck) const;
	bool CollisionCheckWithProjectile(const Projectile::ProjectileType& projectileType, const BoundingLines& bounds) const;
	
	Collision::AABB2D GetPaddleAABB(bool includeAttachedBall) const;
	
	void DebugDraw() const;

private:
	// Default values for the dimensions of the paddle
	static const float PADDLE_WIDTH_ANGLED_SIDE;
	static const float WIDTH_DIFF_PER_SIZE;
	static const float SECONDS_TO_CHANGE_SIZE;

	static const int AVG_OVER_TICKS  = 60;
	
	static const double PADDLE_LASER_BULLET_DELAY;	// Delay between shots of the laser bullet

	bool hitWall;					// True when the paddle hits a wall

	int32_t currType;	    // An ORed together current type of this paddle (see PaddleType)
	PaddleSize currSize;	// The current size (width) of this paddle

	Point2D centerPos;						// Paddle position (at its center) in the game model
	float currHalfHeight;					// Half the height of the paddle
	float currHalfWidthFlat;			// Half of the flat portion of the paddle
	float currHalfWidthTotal;			// Half of the total width of the paddle
	float currHalfDepthTotal;			// Half of the total depth of the paddle
	float minBound, maxBound;			// The current level's boundries along its width for the paddle
	float currScaleFactor;				// The scale difference between the paddle's current size and its default size
	
	// Movement 
	float acceleration;	  // The paddle's acceleration in units / second^2 (always positive)
	float decceleration;  // The paddle's deceleration in units / second^2 (always negative)
	float maxSpeed;	  		// The maximum absolute value speed of the paddle in units per second (note that the minimum Speed is always 0)
	float currSpeed;			// The current absolute value speed of the paddle in units per second
	float lastDirection;	// Used to store the last direction the user told the paddle to move in (-1 for left, 1 for right, 0 for no movement)
	bool moveButtonDown;	// Whether the move button is being held down currently
	float impulse;				// When there's an immediate impulse applied to the paddle we need to use this and add it directly to the position
												// of the paddle (and then immediately reset it to zero)

	// Colour and animation
	ColourRGBA colour;															// The colour multiply of the paddle, including its visibility/alpha
	AnimationLerp<ColourRGBA> colourAnimation;			// Animation associated with the colour
	AnimationMultiLerp<float> moveDownAnimation;		// Animation for when the paddle is being pushed down by the laser beam (away from the level)
	AnimationMultiLerp<float> rotAngleZAnimation;		// Animation for rotating the paddle on the plane that the game is played, default angle is zero (pointing up)

	BoundingLines bounds;						// Collision bounds of the paddle, kept in paddle space (paddle center is 0,0)
	
	double timeSinceLastLaserBlast;	// Time since the last laser projectile/bullet was fired
	double laserBeamTimer;					// Time left on the laser beam power-up

	GameBall* attachedBall;	// When a ball is resting on the paddle it will occupy this variable

	bool isPaddleCamActive;	// Whether or not the camera is inside the paddle
	bool isFiringBeam;			// Whether this paddle is firing the laser beam

	void SetupAnimations();

	void RegenerateBounds();

	void SetDimensions(float newScaleFactor);
	void SetDimensions(PlayerPaddle::PaddleSize size);
	void SetPaddleSize(PlayerPaddle::PaddleSize size);
	void FireAttachedBall();
	void MoveAttachedBallToNewBounds(double dT);

	void CollateralBlockProjectileCollision(const Projectile& projectile);
	void LaserBulletProjectileCollision(const Projectile& projectile);
	void RocketProjectileCollision(GameModel* gameModel, const Projectile& projectile);
	void FireGlobProjectileCollision(const Projectile& projectile);
	float GetPercentNearPaddleCenter(const Point2D& projectileCenter, float& distFromCenter);
	void SetPaddleHitByProjectileAnimation(const Point2D& projectileCenter, double totalHitEffectTime, 
																				 float minMoveDown, float closeToCenterCoeff, float maxRotationInDegs);

	Collision::Circle2D CreatePaddleShieldBounds() const;


	void GenerateRocketDimensions(Point2D& spawnPos, float& width, float& height) const;
};

inline void PlayerPaddle::Animate(double seconds) {
	// Tick any paddle-related animations
	this->colourAnimation.Tick(seconds);
}

// Check to see if the given ball collides, return the normal of the collision and the line of the collision as well
// as the time since the collision occurred
inline bool PlayerPaddle::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	if (ball.IsLastThingCollidedWith(this)) {
		return false;
	}

	// If the paddle has a shield around it do the collision with the shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {

		Collision::Circle2D shield = this->CreatePaddleShieldBounds();
		Vector2D lengthVec = ball.GetBounds().Center() - shield.Center();
		float sqLength = Vector2D::Dot(lengthVec, lengthVec);
		float radiiSum = ball.GetBounds().Radius() + shield.Radius();
		float sqRadii = radiiSum * radiiSum;

		if (sqLength < sqRadii) {
			n = lengthVec;
			n.Normalize();
			
			Point2D  approxCircleCollisionPt = shield.Center() + shield.Radius() * n;
			Vector2D perpendicularVec(-n[1], n[0]);

			// Make the collision line the tangent at the surface of the shield
			collisionLine = Collision::LineSeg2D(approxCircleCollisionPt - perpendicularVec, approxCircleCollisionPt + perpendicularVec);
			// A rough approximate of the the time since collision is the distance between the 
			// ball's center and the shield barrier divided by the ball's velocity
			timeSinceCollision = Vector2D::Magnitude(approxCircleCollisionPt - ball.GetBounds().Center()) / ball.GetSpeed();

			return true;
		}
		else {
			return false;
		}
	}
	else {
		return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
	}
}

/**
 * Check to see if the given set of bounding lines collides with this paddle.
 */
inline bool PlayerPaddle::CollisionCheck(const BoundingLines& bounds, bool includeAttachedBallCheck) const {
	bool didCollide = false;
	// If the paddle has a shield around it do the collision with the shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		didCollide = bounds.CollisionCheck(this->CreatePaddleShieldBounds());
	}
	else {
		didCollide = this->bounds.CollisionCheck(bounds);
	}

	if (includeAttachedBallCheck && !didCollide && this->HasBallAttached()) {
		didCollide = bounds.CollisionCheck(this->GetAttachedBall()->GetBounds());
	}

	// If there's a rocket attached we need to check for collisions with it!
	if (!didCollide && (this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		Point2D rocketSpawnPos;
		float rocketHeight, rocketWidth;
		this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		Collision::AABB2D rocketAABB(rocketSpawnPos - widthHeightVec, rocketSpawnPos + widthHeightVec);
		didCollide = bounds.CollisionCheck(rocketAABB);
	}
	
	return didCollide;
}

// Check for a collision with the given projectile
inline bool PlayerPaddle::CollisionCheckWithProjectile(const Projectile::ProjectileType& projectileType, const BoundingLines& bounds) const {
	UNUSED_PARAMETER(projectileType);
	return this->CollisionCheck(bounds, true);
}

// The paddle destroys all projectiles that collide with it, currently
inline bool PlayerPaddle::ProjectilePassesThrough(const Projectile& projectile) {
	// Projectiles can pass through when reflected by the paddle shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		if (projectile.GetType() == Projectile::PaddleLaserBulletProjectile) {
			return true;
		}
	}
	return false;
}

// Build a temporary circle representing the bounds of any shield that might exist around
// the paddle.
inline Collision::Circle2D PlayerPaddle::CreatePaddleShieldBounds() const {
	return Collision::Circle2D(this->GetCenterPosition(), this->GetHalfWidthTotal());
}

// Obtain an AABB encompassing the entire paddle's current collision area
inline Collision::AABB2D PlayerPaddle::GetPaddleAABB(bool includeAttachedBall) const {
	Point2D minPt, maxPt;
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		Vector2D sphereRadiusVec(this->GetHalfWidthTotal(), this->GetHalfWidthTotal());
		sphereRadiusVec = 1.2f * sphereRadiusVec;
		minPt = this->GetCenterPosition() - sphereRadiusVec;
		maxPt = this->GetCenterPosition() + sphereRadiusVec;
	}
	else {
		Vector2D halfWidthHeight(this->GetHalfWidthTotal(), this->GetHalfHeight());
		halfWidthHeight = 1.2f * halfWidthHeight;
		minPt = this->GetCenterPosition() - halfWidthHeight;
		maxPt = this->GetCenterPosition() + halfWidthHeight;
	}

	Collision::AABB2D paddleAABB(minPt, maxPt);
	if (includeAttachedBall && this->HasBallAttached()) {
		const Collision::Circle2D& ballBounds = this->GetAttachedBall()->GetBounds();
		Vector2D ballRadiusVec(ballBounds.Radius(), ballBounds.Radius());
		ballRadiusVec = 1.2f * ballRadiusVec;
		paddleAABB.AddPoint(ballBounds.Center() + ballRadiusVec);
		paddleAABB.AddPoint(ballBounds.Center() - ballRadiusVec);
	}

	if ((this->GetPaddleType() & PlayerPaddle::RocketPaddle) == PlayerPaddle::RocketPaddle) {
		Point2D rocketSpawnPos;
		float rocketHeight, rocketWidth;
		this->GenerateRocketDimensions(rocketSpawnPos, rocketWidth, rocketHeight);
		Vector2D widthHeightVec(rocketWidth/1.5f, rocketHeight/1.5f);

		paddleAABB.AddPoint(rocketSpawnPos - widthHeightVec);
		paddleAABB.AddPoint(rocketSpawnPos + widthHeightVec);
	}

	return paddleAABB;
}

// Applies an immediate impulse force along the x-axis (movement axis of the paddle)
inline void PlayerPaddle::ApplyImpulseForce(float xDirectionalForce) {
	assert(xDirectionalForce != 0.0f);
	this->lastDirection = NumberFuncs::SignOf(xDirectionalForce);
	this->impulse += 0.05f * xDirectionalForce;
}

inline void PlayerPaddle::GenerateRocketDimensions(Point2D& spawnPos, float& width, float& height) const {
		height = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;
		width  = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT;
		spawnPos = this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + 0.5f * height);
}

#endif