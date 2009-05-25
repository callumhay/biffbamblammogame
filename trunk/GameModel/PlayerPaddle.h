#ifndef __PLAYERPADDLE_H__
#define __PLAYERPADDLE_H__

#include "../BlammoEngine/BlammoEngine.h"
#include "BoundingLines.h"

class GameModel;

// Represents the player controlled paddle shaped as follows:
//                -------------
//               /_____________\

class PlayerPaddle {
public:
	enum PaddleType { NormalPaddle = 0x00000000, LaserPaddle = 0x00000001 };
	enum PaddleSize { SmallestSize = 0, SmallerSize = 1, NormalSize = 2, BiggerSize = 3, BiggestSize = 4 };

private:
	// Default values for the dimensions of the paddle
	static const float PADDLE_WIDTH_TOTAL;
	static const float PADDLE_WIDTH_FLAT_TOP;
	static const float PADDLE_WIDTH_ANGLED_SIDE;
	static const float PADDLE_HEIGHT_TOTAL;
	static const float PADDLE_HALF_WIDTH;
	static const float PADDLE_HALF_HEIGHT;
	static const float WIDTH_DIFF_PER_SIZE;
	static const float SECONDS_TO_CHANGE_SIZE;

	static const int AVG_OVER_TICKS  = 60;

	static const float PADDLE_LASER_DELAY;	// Delay between shots of the laser

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
	float minBound, maxBound;			// The current level's boundries along its width for the paddle
	float speed;									// Speed of the paddle in units per second
	float currScaleFactor;			// The scale difference between the paddle's current size and its default size
	
	BoundingLines bounds;					// Collision bounds of the paddle, kept in paddle space (paddle center is 0,0)
	
	float timeSinceLastLaserBlast;	// Time since the last laser projectile was fired

	void SetDimensions(float newScaleFactor);
	void SetDimensions(PlayerPaddle::PaddleSize size);
	void SetPaddleSize(PlayerPaddle::PaddleSize size);

public:
	static const float DEFAULT_SPEED;
	static const int RAND_DEG_ANG    = 20;

	PlayerPaddle(float minBound, float maxBound);
	PlayerPaddle();
	~PlayerPaddle();

	// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
	void ResetPaddle() {
		this->currSize = PlayerPaddle::NormalSize;
		this->SetDimensions(PlayerPaddle::NormalSize);
		this->centerPos = Point2D((maxBound + minBound)/2.0f, this->currHalfHeight);
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

	void Tick(double seconds);
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
		return this->speed;
	}

	Vector2D GetAvgVelocity() const {
		return Vector2D(this->avgVel, 0);
	}

	// Paddle size modifying / querying functions
	PaddleSize GetPaddleSize() const {
		return this->currSize;
	}
	float GetPaddleScaleFactor() const {
		return this->currScaleFactor;
	}

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

	void Shoot(GameModel* gameModel);

	bool CollisionCheck(const Collision::Circle2D& c, Vector2D& n, float& d);
	void DebugDraw() const;

};
#endif