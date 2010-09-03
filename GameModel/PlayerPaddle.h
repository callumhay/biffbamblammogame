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
	static const float DEFAULT_SPEED;
	static const float POISON_SPEED_DIMINISH;
	static const int RAND_DEG_ANG    = 20;

	static const Vector2D DEFAULT_PADDLE_UP_VECTOR;
	static const Vector2D DEFAULT_PADDLE_RIGHT_VECTOR;

	enum PaddleType { NormalPaddle = 0x00000000, LaserBulletPaddle = 0x00000001, PoisonPaddle = 0x00000010, 
										StickyPaddle = 0x00000100, LaserBeamPaddle = 0x00001000, RocketPaddle = 0x00010000, 
										ShieldPaddle = 0x00100000 };
	enum PaddleSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };

	PlayerPaddle(float minBound, float maxBound);
	PlayerPaddle();
	~PlayerPaddle();

	// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
	void ResetPaddle() {
		this->timeSinceLastLaserBlast = 0.0;
		this->laserBeamTimer = 0.0;
		this->currSize = PlayerPaddle::NormalSize;
		this->centerPos = this->GetDefaultCenterPosition();
		this->currType = PlayerPaddle::NormalPaddle;
		this->colour = ColourRGBA(1, 1, 1, 1);
		this->isFiringBeam = false;
		this->SetupAnimations();
		// This will set the dimensions the the default and regenerate the bounds of the paddle
		this->SetDimensions(PlayerPaddle::NormalSize);
	}

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

	Vector2D GetUpVector() const {
		return Vector2D::Rotate(this->rotAngleZAnimation.GetInterpolantValue(), PlayerPaddle::DEFAULT_PADDLE_UP_VECTOR);
	}

	void Tick(double seconds);
	void Animate(double seconds);

	void Move(float dist) {
		this->distTemp += dist;
	}

	void SetMinMaxLevelBound(float min, float max) {
		assert(min < max);
		assert((max - min) > PADDLE_WIDTH_TOTAL);
		this->minBound = min;
		this->maxBound = max;

		// Reset the paddle to the center of the new bounds
		this->ResetPaddle();
	}

	float GetSpeed() const {
		float currSpeed = this->speed;
		if ((this->currType & PlayerPaddle::PoisonPaddle) == PlayerPaddle::PoisonPaddle) {
			currSpeed -= PlayerPaddle::POISON_SPEED_DIMINISH;
		}
		return currSpeed;
	}

	Vector2D GetAvgVelocity() const {
		return Vector2D(this->avgVel, 0);
	}

	// Paddle size set/get functions
	PaddleSize GetPaddleSize() const {
		return this->currSize;
	}
	float GetPaddleScaleFactor() const {
		return this->currScaleFactor;
	}

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


	/**
	 * Increases the paddle size if it can.
	 * Returns: true if there was an increase in size, false otherwise.
	 */
	bool IncreasePaddleSize() {
		if (this->currSize == BiggestSize) { 
			return false; 
		}
		
		this->SetPaddleSize(static_cast<PaddleSize>(this->currSize + 1));
		return true;
	}

	/**
	 * Decreases the paddle size if it can.
	 * Returns: true if there was an decrease in size, false otherwise.
	 */
	bool DecreasePaddleSize() {
		if (this->currSize == SmallestSize) { 
			return false; 
		}

		this->SetPaddleSize(static_cast<PaddleSize>(this->currSize - 1));
		return true;
	}

	// Paddle type modifying / querying functions
	int GetPaddleType() const {
		return this->currType;
	}
	void AddPaddleType(const PaddleType type) {
		this->currType = this->currType | type;
	}
	void RemovePaddleType(const PaddleType type) {
		this->currType = this->currType & ~type;
	}

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

	void HitByProjectile(const Projectile& projectile);
	bool ProjectilePassesThrough(const Projectile& projectile);
	void ModifyProjectileTrajectory(Projectile& projectile);

	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const;
	bool CollisionCheck(const BoundingLines& bounds) const;
	void DebugDraw() const;

private:
	// Default values for the dimensions of the paddle
	static const float PADDLE_WIDTH_ANGLED_SIDE;
	static const float WIDTH_DIFF_PER_SIZE;
	static const float SECONDS_TO_CHANGE_SIZE;

	static const int AVG_OVER_TICKS  = 60;
	
	static const double PADDLE_LASER_BULLET_DELAY;	// Delay between shots of the laser bullet

	float distTemp;			// A temporary store for the change in movement
	float avgVel;				// Keeps the average velocity (over the past AVG_OVER_TICKS ticks) of the paddle at a given time
	int ticksSinceAvg;  // Keeps track of ticks since a sampling of the average velocity occurred
	bool hitWall;				// True when the paddle hits a wall

	int currType;					// An ORed together current type of this paddle (see PaddleType)
	PaddleSize currSize;	// The current size (width) of this paddle

	Point2D centerPos;						// Paddle position (at its center) in the game model
	float currHalfHeight;					// Half the height of the paddle
	float currHalfWidthFlat;			// Half of the flat portion of the paddle
	float currHalfWidthTotal;			// Half of the total width of the paddle
	float currHalfDepthTotal;			// Half of the total depth of the paddle
	float minBound, maxBound;			// The current level's boundries along its width for the paddle
	float speed;									// Speed of the paddle in units per second
	float currScaleFactor;				// The scale difference between the paddle's current size and its default size
	

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
	float GetPercentNearPaddleCenter(const Projectile& projectile, float& distFromCenter);
	Collision::Circle2D CreatePaddleShieldBounds() const;
};

inline void PlayerPaddle::Animate(double seconds) {
	// Tick any paddle-related animations
	this->colourAnimation.Tick(seconds);
}

// Check to see if the given ball collides, return the normal of the collision and the line of the collision as well
// as the time since the collision occurred
inline bool PlayerPaddle::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
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
inline bool PlayerPaddle::CollisionCheck(const BoundingLines& bounds) const {
	// If the paddle has a shield around it do the collision with the shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {
		return bounds.CollisionCheck(this->CreatePaddleShieldBounds());
	}
	else {
		return this->bounds.CollisionCheck(bounds);
	}
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

#endif