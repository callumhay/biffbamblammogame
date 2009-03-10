#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameModelConstants.h"
#include "Projectile.h"

#include "../BlammoEngine/BlammoEngine.h"

const float PlayerPaddle::PADDLE_WIDTH_TOTAL = 3.5f;
const float PlayerPaddle::PADDLE_WIDTH_FLAT_TOP = 3.5f;
const float PlayerPaddle::PADDLE_HEIGHT_TOTAL = 0.80f;
const float PlayerPaddle::PADDLE_HALF_WIDTH = PADDLE_WIDTH_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_HEIGHT = PADDLE_HEIGHT_TOTAL / 2.0f;

const float PlayerPaddle::DEFAULT_SPEED = 24.0f;

const float PlayerPaddle::PADDLE_LASER_DELAY = 0.25f;

PlayerPaddle::PlayerPaddle() : 
	centerPos(0.0f, 0.0f), minBound(0.0f), maxBound(0.0f), speed(DEFAULT_SPEED), distTemp(0.0f), 
	avgVel(0.0f), ticksSinceAvg(0), timeSinceLastLaserBlast(PADDLE_LASER_DELAY), 
	hitWall(false), currType(NormalPaddle) {
	this->SetDefaultDimensions();
}

PlayerPaddle::PlayerPaddle(float minBound, float maxBound) : speed(DEFAULT_SPEED), distTemp(0.0f), avgVel(0.0f), ticksSinceAvg(0), hitWall(false) {
	this->SetMinMaxLevelBound(minBound, maxBound);
}

PlayerPaddle::~PlayerPaddle() {
}

void PlayerPaddle::SetDefaultDimensions() {
	// Reestablish dimensions of paddle
	this->currHalfHeight = PADDLE_HALF_HEIGHT;
	this->currHalfWidthFlat = PADDLE_HALF_WIDTH;
	this->currHalfWidthTotal = PADDLE_HALF_WIDTH;

	// Reset the bounds of the paddle (in paddle-space)
	std::vector<Collision::LineSeg2D> lineBounds;
	std::vector<Vector2D>  lineNorms;
	
	// Top boundry
	Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, this->currHalfHeight),
		           Point2D(-this->currHalfWidthFlat, this->currHalfHeight));
	Vector2D n1(0, 1);
	lineBounds.push_back(l1);
	lineNorms.push_back(n1);

	this->bounds = BoundingLines(lineBounds, lineNorms);
}

void PlayerPaddle::Tick(double seconds) {
	if (this->timeSinceLastLaserBlast < PADDLE_LASER_DELAY) {
		this->timeSinceLastLaserBlast += seconds;
	}

	float newCenterX = this->centerPos[0] + (this->distTemp * seconds);
	float minNewXPos = newCenterX - this->currHalfWidthTotal;
	float maxNewXPos = newCenterX + this->currHalfWidthTotal;

	if (minNewXPos - EPSILON <= this->minBound) {
		// The paddle bumped into the left wall
		this->centerPos[0] = this->minBound + this->currHalfWidthTotal;
		
		if (this->hitWall) {
			this->distTemp = 0.0f;
		}
		else {
			// EVENT: paddle hit left wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(this->centerPos + Vector2D(-this->currHalfWidthTotal, 0));
		}

		this->hitWall = true;
	}
	else if (maxNewXPos + EPSILON > this->maxBound) {
		// The paddle bumped into the right wall
		this->centerPos[0] = this->maxBound - this->currHalfWidthTotal;
		
		if (this->hitWall) {
			this->distTemp = 0.0f;
		}
		else {
			// EVENT: paddle hit right wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(this->centerPos + Vector2D(this->currHalfWidthTotal, 0));
		}
		this->hitWall = true;
	}
	else {
		this->centerPos[0] = newCenterX;
		this->hitWall = false;
	}
	
	if (this->ticksSinceAvg == 0) {
		this->avgVel = this->distTemp;
	}
	else {
		this->avgVel = (this->avgVel + this->distTemp) * 0.5f;
	}
	this->ticksSinceAvg = (this->ticksSinceAvg + 1) % AVG_OVER_TICKS;
	this->distTemp = 0.0f;
}

/**
 * This will fire any weapons or abilities that paddle currently has - if none
 * exist then this function does nothing.
 */
void PlayerPaddle::Shoot(GameModel* gameModel) {
	// Check for laser paddle power-up
	if ((this->GetPaddleType() & PlayerPaddle::LaserPaddle) == PlayerPaddle::LaserPaddle) {
		
		// Make sure we are allowed to fire a new laser blast
		if (this->timeSinceLastLaserBlast >= PADDLE_LASER_DELAY) {
			// Fire ze laser! - tell the model about it
			gameModel->AddProjectile(Projectile::PaddleLaserProjectile, this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + PaddleLaser::PADDLELASER_HALF_HEIGHT));

			// Reset the timer for the next laser blast
			this->timeSinceLastLaserBlast = 0;
		}
	}

	// TODO: other paddle shooting abilities go here...

}

bool PlayerPaddle::CollisionCheck(const Collision::Circle2D& c, Vector2D& n, float& d) {
	// Move the circle into paddle space
	Collision::Circle2D temp = Collision::Circle2D(c.Center() - Vector2D(this->centerPos[0], this->centerPos[1]), c.Radius());
	return this->bounds.Collide(temp, n, d);
}

void PlayerPaddle::DebugDraw() const {
	this->bounds.DebugDraw();
}