#ifndef __PLAYERPADDLE_H__
#define __PLAYERPADDLE_H__

#include <vector>

#include "../Utils/Point.h"
#include "BoundingLines.h"

// Represents the player controlled paddle shaped as follows:
//              -----------------
//              |_______________|

class PlayerPaddle {

private:
	// Default values for the dimensions of the paddle
	static const float PADDLE_WIDTH_TOTAL;
	static const float PADDLE_WIDTH_FLAT_TOP;
	static const float PADDLE_HEIGHT_TOTAL;
	static const float PADDLE_HALF_WIDTH;
	static const float PADDLE_HALF_HEIGHT;

	static const int AVG_OVER_TICKS  = 60;

	float distTemp;			// A temporary store for the change in movement
	float avgVel;				// Keeps the average velocity (over the past AVG_OVER_TICKS ticks) of the paddle at a given time
	int ticksSinceAvg;  // Keeps track of ticks since a sampling of the average velocity occurred
	bool hitWall;				// True when the paddle hits a wall

	Point2D centerPos;						// Paddle position (at its center) in the game model
	float currHalfHeight;					// Half the height of the paddle
	float currHalfWidthFlat;			// Half of the flat portion of the paddle
	float currHalfWidthTotal;			// Half of the total width of the paddle
	float minBound, maxBound;			// The current level's boundries along its width for the paddle
	float speed;									// Speed of the paddle in units per second
	
	BoundingLines bounds;					// Collision bounds of the paddle, kept in paddle space (paddle center is 0,0)
	
	
	void SetDefaultDimensions();

	// Reset the dimensions and position of this paddle (e.g., after death, start of a level).
	void ResetPaddle() {
		this->SetDefaultDimensions();
		this->centerPos = Point2D((maxBound + minBound)/2.0f, this->currHalfHeight);
	}

public:
	static const float DEFAULT_SPEED;
	static const int RAND_DEG_ANG    = 20;

	PlayerPaddle(float minBound, float maxBound);
	PlayerPaddle();
	~PlayerPaddle();

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

	bool CollisionCheck(const Circle2D& c, Vector2D& n, float& d);
	void DebugDraw() const;

};
#endif