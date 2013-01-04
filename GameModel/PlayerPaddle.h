/**
 * PlayerPaddle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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
class PaddleMineProjectile;
class MineTurretProjectile;
class MineProjectile;

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

	static const double PADDLE_LASER_BULLET_DELAY;	// Delay between shots of the laser bullet
    static const double PADDLE_MINE_LAUNCH_DELAY;   // Delay between launches of mines

	static const Vector2D DEFAULT_PADDLE_UP_VECTOR;
	static const Vector2D DEFAULT_PADDLE_RIGHT_VECTOR;

	static const int DEFAULT_SHIELD_DMG_PER_SECOND;

	enum PaddleType { NormalPaddle = 0x00000000, LaserBulletPaddle = 0x00000001, PoisonPaddle = 0x00000002, 
                      StickyPaddle = 0x00000004, LaserBeamPaddle = 0x00000008, RocketPaddle = 0x00000010, 
                      ShieldPaddle = 0x00000020, InvisiPaddle = 0x00000040, MagnetPaddle = 0x00000080,
                      MineLauncherPaddle = 0x00000100 };

	enum PaddleSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };

    static const float DEFAULT_PADDLE_SCALE;
    static void SetNormalScale(float scale) { assert(scale > 0.0f); PlayerPaddle::NormalSizeScale = scale; };

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

	void Tick(double seconds, bool pausePaddleMovement, GameModel& gameModel);
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
			this->MoveAttachedObjectsToNewBounds(dT);
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
    void ShootBall();
    void ReleaseEverythingAttached();
	
    bool AttachBall(GameBall* ball);
    void AttachProjectile(Projectile* projectile);
    void DetachProjectile(Projectile* projectile);

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

    bool HasProjectileAttached() const {
        return !this->attachedProjectiles.empty();
    }

    //void HitByBoss(const BossBodyPart& bossPart);

	void HitByProjectile(GameModel* gameModel, const Projectile& projectile);
	bool ProjectilePassesThrough(const Projectile& projectile);
    bool ProjectileIsDestroyedOnCollision(const Projectile& projectile);
	void ModifyProjectileTrajectory(Projectile& projectile);

	void UpdateBoundsByPieceCollision(const LevelPiece& p, bool doAttachedBallCollision);

	// TODO: Add the parameter: "bool includeAttachedBallCheck" to all paddle collision checks...
	bool CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision);
	bool CollisionCheck(const BoundingLines& bounds, bool includeAttachedBallCheck) const;
	bool CollisionCheckWithProjectile(const Projectile& projectile, const BoundingLines& bounds) const;
	
    bool UpdateForOpposingForceBallCollision(const GameBall& ball, double dT);

	Collision::AABB2D GetPaddleAABB(bool includeAttachedBall) const;
    //std::list<PointAward> GetPointsForHittingBall(const GameBall& ball) const;

    void AugmentDirectionOnPaddleMagnet(double seconds, float degreesChangePerSec,
        const Point2D& currCenter, Vector2D& vectorToAugment) const;

    const BoundingLines& GetBounds() const {
        return this->bounds;
    }

	void DebugDraw() const;

    // Enable options for the paddle - used during the tutorial
    static void SetEnablePaddleReleaseTimer(bool enabled) {
        PlayerPaddle::paddleBallReleaseTimerEnabled = enabled;
    }
    static bool GetIsPaddleReleaseTimerEnabled() {
        return PlayerPaddle::paddleBallReleaseTimerEnabled;
    }

    static void SetEnablePaddleRelease(bool enabled) {
        PlayerPaddle::paddleBallReleaseEnabled = enabled;
    }
    static bool GetIsPaddleReleaseEnabled() {
        return PlayerPaddle::paddleBallReleaseEnabled;
    }

    float GetMineProjectileStartingHeightRelativeToPaddle() const;

private:
	// Default values for the dimensions of the paddle
	static const float PADDLE_WIDTH_ANGLED_SIDE;
	static const float WIDTH_DIFF_PER_SIZE;
	static const float SECONDS_TO_CHANGE_SIZE;

	static const int AVG_OVER_TICKS  = 60;
	
    static bool paddleBallReleaseTimerEnabled;
    static bool paddleBallReleaseEnabled;

    static float NormalSizeScale;

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
	float maxSpeed;	  	  // The maximum absolute value speed of the paddle in units per second (note that the minimum Speed is always 0)
	float currSpeed;      // The current absolute value speed of the paddle in units per second
	float lastDirection;  // Used to store the last direction the user told the paddle to move in (-1 for left, 1 for right, 0 for no movement)
	bool moveButtonDown;  // Whether the move button is being held down currently
	float impulse;        // When there's an immediate impulse applied to the paddle we need to use this and add it directly to the position
                          // of the paddle (and then immediately reset it to zero)

	// Colour and animation
	ColourRGBA colour;															// The colour multiply of the paddle, including its visibility/alpha
	AnimationLerp<ColourRGBA> colourAnimation;			// Animation associated with the colour
	AnimationMultiLerp<float> moveDownAnimation;		// Animation for when the paddle is being pushed down by the laser beam (away from the level)
	AnimationMultiLerp<float> rotAngleZAnimation;		// Animation for rotating the paddle on the plane that the game is played, default angle is zero (pointing up)

	BoundingLines bounds;						// Collision bounds of the paddle, kept in paddle space (paddle center is 0,0)
	
    double timeSinceLastMineLaunch; // Time since the last launch of a mine projectile
	double timeSinceLastLaserBlast;	// Time since the last laser projectile/bullet was fired
	double laserBeamTimer;					// Time left on the laser beam power-up

	GameBall* attachedBall;	// When a ball is resting on the paddle it will occupy this variable

    std::list<Projectile*> attachedProjectiles;

	bool isPaddleCamActive;	// Whether or not the camera is inside the paddle
	bool isFiringBeam;			// Whether this paddle is firing the laser beam

	void SetupAnimations();

	void RegenerateBounds();

	void SetDimensions(float newScaleFactor);
	void SetDimensions(PlayerPaddle::PaddleSize size);
	void SetPaddleSize(PlayerPaddle::PaddleSize size);
	void FireAttachedBall();
    void FireAttachedProjectile();
	void MoveAttachedObjectsToNewBounds(double dT);

	void CollateralBlockProjectileCollision(const Projectile& projectile);
	void LaserBulletProjectileCollision(const Projectile& projectile);
	void RocketProjectileCollision(GameModel* gameModel, const RocketProjectile& projectile);
    void MineProjectileCollision(GameModel* gameModel, const MineProjectile& projectile);
	void FireGlobProjectileCollision(const Projectile& projectile);
	float GetPercentNearPaddleCenter(const Point2D& projectileCenter, float& distFromCenter);
	void SetPaddleHitByProjectileAnimation(const Point2D& projectileCenter, double totalHitEffectTime, 
                                           float minMoveDown, float closeToCenterCoeff, float maxRotationInDegs);

	Collision::Circle2D CreatePaddleShieldBounds() const;
	void GenerateRocketDimensions(Point2D& spawnPos, float& width, float& height) const;

    static float CalculateTargetScaleFactor(PlayerPaddle::PaddleSize size) {
        int diffFromNormalSize = static_cast<int>(size) - static_cast<int>(PlayerPaddle::NormalSize);
        return PlayerPaddle::NormalSizeScale * (PADDLE_WIDTH_TOTAL + diffFromNormalSize * PlayerPaddle::WIDTH_DIFF_PER_SIZE) / PADDLE_WIDTH_TOTAL;
    }

    DISALLOW_COPY_AND_ASSIGN(PlayerPaddle);
};

/**
 * Set the dimensions of the paddle based on an enumerated paddle size given.
 * This will change the scale factor and bounds of the paddle.
 */
inline void PlayerPaddle::SetDimensions(PlayerPaddle::PaddleSize size) {
	this->SetDimensions(PlayerPaddle::CalculateTargetScaleFactor(size));
}

inline void PlayerPaddle::Animate(double seconds) {
	// Tick any paddle-related animations
	this->colourAnimation.Tick(seconds);
}

// Check to see if the given ball collides, return the normal of the collision and the line of the collision as well
// as the time since the collision occurred
inline bool PlayerPaddle::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, 
                                         Collision::LineSeg2D& collisionLine, 
                                         double& timeSinceCollision) {

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
            timeSinceCollision = Vector2D::Magnitude(approxCircleCollisionPt - ball.GetBounds().Center() + ball.GetBounds().Radius()*n) / ball.GetSpeed();

			return true;
		}
		else {
			return false;
		}
	}
	else {

        if (this->bounds.GetNumLines() == 3) {
            return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
        }
        else {

            // Don't allow the bottom paddle collision boundry line to factor into the collision if the ball is traveling downward
            // at the paddle or the ball is above the paddle
            assert(this->bounds.GetNumLines() == 4);

            if ((ball.GetCenterPosition2D()[1] < (this->GetCenterPosition()[1] - this->GetHalfHeight())) &&
                Vector2D::Dot(ball.GetDirection(), this->GetUpVector()) > 0) {
                // Ball is travelling upwards at the paddle from below it...
                return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
            }
            else {
                // Ball is travelling downwards/parallel at the paddle
                Collision::LineSeg2D bottomLine = this->bounds.GetLine(3);
                Vector2D bottomNormal           = this->bounds.GetNormal(3);
                this->bounds.PopLastLineAndNormal();
                bool wasCollision = this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
                this->bounds.PushLineAndNormal(bottomLine, bottomNormal);
                return wasCollision;
            }
        }
	}
}

// Check for a collision with the given projectile
inline bool PlayerPaddle::CollisionCheckWithProjectile(const Projectile& projectile,
                                                       const BoundingLines& bounds) const {
    switch (projectile.GetType()) {

        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // These projectiles can only collide with the paddle if it's NOT going upwards into the level
            if (Vector2D::Dot(projectile.GetVelocityDirection(), this->GetUpVector()) <= 0) {
                return this->CollisionCheck(bounds, true);
            }
            break;

        default:
            return this->CollisionCheck(bounds, true);
    }
    return false;
}

// The paddle destroys all projectiles that collide with it, currently
inline bool PlayerPaddle::ProjectilePassesThrough(const Projectile& projectile) {
	// Projectiles can pass through when reflected by the paddle shield
	if ((this->GetPaddleType() & PlayerPaddle::ShieldPaddle) == PlayerPaddle::ShieldPaddle) {

        switch (projectile.GetType()) {

            case Projectile::BallLaserBulletProjectile:
            case Projectile::PaddleLaserBulletProjectile:
            case Projectile::PaddleRocketBulletProjectile:
            case Projectile::RocketTurretBulletProjectile:
            case Projectile::LaserTurretBulletProjectile:
            case Projectile::PaddleMineBulletProjectile:
            case Projectile::MineTurretBulletProjectile:
                return true;
            default:
                break;
        }

	}

	return false;
}

// Build a temporary circle representing the bounds of any shield that might exist around
// the paddle.
inline Collision::Circle2D PlayerPaddle::CreatePaddleShieldBounds() const {
	return Collision::Circle2D(this->GetCenterPosition(), this->GetHalfWidthTotal());
}

// Applies an immediate impulse force along the x-axis (movement axis of the paddle)
inline void PlayerPaddle::ApplyImpulseForce(float xDirectionalForce) {
	assert(xDirectionalForce != 0.0f);
	this->lastDirection = NumberFuncs::SignOf(xDirectionalForce);
	this->impulse = xDirectionalForce;
}

inline void PlayerPaddle::GenerateRocketDimensions(Point2D& spawnPos, float& width, float& height) const {
    height = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;
    width  = this->currScaleFactor * PaddleRocketProjectile::PADDLEROCKET_WIDTH_DEFAULT;
    spawnPos = this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + 0.5f * height);
}

#endif