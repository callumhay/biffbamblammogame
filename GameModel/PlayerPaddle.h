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

	enum PaddleType { NormalPaddle = 0x00000000, LaserBulletPaddle = 0x00000001, PoisonPaddle = 0x00000010, 
										StickyPaddle = 0x00000100, LaserBeamPaddle = 0x00001000 };
	enum PaddleSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };

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
	
	Vector2D upVector;						// The unit vector pointing upwards for the paddle

	ColourRGBA colour;													// The colour multiply of the paddle, including its visibility/alpha
	AnimationLerp<ColourRGBA> colourAnimation;	// Animations associated with the colour
	
	BoundingLines bounds;						// Collision bounds of the paddle, kept in paddle space (paddle center is 0,0)
	
	double timeSinceLastLaserBlast;	// Time since the last laser projectile/bullet was fired
	double laserBeamTimer;					// Time left on the laser beam power-up

	GameBall* attachedBall;	// When a ball is resting on the paddle it will occupy this variable

	bool isPaddleCamActive;	// Whether or not the camera is inside the paddle

	void SetDimensions(float newScaleFactor);
	void SetDimensions(PlayerPaddle::PaddleSize size);
	void SetPaddleSize(PlayerPaddle::PaddleSize size);
	void FireAttachedBall();
	void MoveAttachedBallToNewBounds();

public:
	static const float DEFAULT_SPEED;
	static const float POISON_SPEED_DIMINISH;
	static const int RAND_DEG_ANG    = 20;

	PlayerPaddle(float minBound, float maxBound);
	PlayerPaddle();
	~PlayerPaddle();

	// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
	void ResetPaddle() {
		this->timeSinceLastLaserBlast = 0.0;
		this->laserBeamTimer = 0.0;
		this->currSize = PlayerPaddle::NormalSize;
		this->SetDimensions(PlayerPaddle::NormalSize);
		this->centerPos = Point2D((maxBound + minBound)/2.0f, this->currHalfHeight);
		this->currType = PlayerPaddle::NormalPaddle;
	}

	// Obtain the center position of this paddle.
	Point2D GetCenterPosition() const {
		return this->centerPos;
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
		return this->upVector;
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
	ColourRGBA GetColour() const {
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
	void SetPaddleCamera(bool isPaddleCamOn) {
		this->isPaddleCamActive = isPaddleCamOn;
		
		// When the paddle camera is on, we change the collision boundries to
		// be more natural to the vision of the paddle cam
		this->SetDimensions(this->currSize);
		if (!isPaddleCamOn) {
			this->MoveAttachedBallToNewBounds();
		}
	}
	bool GetIsPaddleCameraOn() const {
		return this->isPaddleCamActive;
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

	bool CollisionCheck(const Collision::Circle2D& c, const Vector2D& velocity, Vector2D& n, float& d);
	void DebugDraw() const;

};
#endif