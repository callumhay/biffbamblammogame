#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameModelConstants.h"
#include "Projectile.h"

// Default values for the size of the paddle
const float PlayerPaddle::PADDLE_WIDTH_TOTAL = 3.5f;
const float PlayerPaddle::PADDLE_WIDTH_FLAT_TOP = 2.0f;
const float PlayerPaddle::PADDLE_WIDTH_ANGLED_SIDE = (PADDLE_WIDTH_TOTAL - PADDLE_WIDTH_FLAT_TOP) / 2.0f;
const float PlayerPaddle::PADDLE_HEIGHT_TOTAL = 0.80f;
const float PlayerPaddle::PADDLE_HALF_WIDTH = PADDLE_WIDTH_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_HEIGHT = PADDLE_HEIGHT_TOTAL / 2.0f;

// The difference in width per size change of the paddle
const float PlayerPaddle::WIDTH_DIFF_PER_SIZE = 0.8f;

// Number of seconds it takes for the paddle to change between sizes
// (bigger is slower, smaller is faster)
const float PlayerPaddle::SECONDS_TO_CHANGE_SIZE = 0.5f;

// Default speed of the paddle
const float PlayerPaddle::DEFAULT_SPEED = 24.0f;
// Speed amount taken off the paddle when poisoned
const float PlayerPaddle::POISON_SPEED_DIMINISH = PlayerPaddle::DEFAULT_SPEED / 2.75f;
// Delay between shots of the laser
const float PlayerPaddle::PADDLE_LASER_DELAY = 0.25f;

PlayerPaddle::PlayerPaddle() : 
	centerPos(0.0f, 0.0f), minBound(0.0f), maxBound(0.0f), speed(DEFAULT_SPEED), distTemp(0.0f), 
	avgVel(0.0f), ticksSinceAvg(0), timeSinceLastLaserBlast(PADDLE_LASER_DELAY), 
	hitWall(false), currType(NormalPaddle), currSize(PlayerPaddle::NormalSize) {
	this->SetDimensions(PlayerPaddle::NormalSize);
}

PlayerPaddle::PlayerPaddle(float minBound, float maxBound) : speed(DEFAULT_SPEED), distTemp(0.0f), avgVel(0.0f), ticksSinceAvg(0), hitWall(false) {
	this->SetMinMaxLevelBound(minBound, maxBound);
}

PlayerPaddle::~PlayerPaddle() {
}

/**
 * Set the dimensions of the paddle based on an enumerated paddle size given.
 * This will change the scale factor and bounds of the paddle.
 */
void PlayerPaddle::SetDimensions(PlayerPaddle::PaddleSize size) {
	int diffFromNormalSize = static_cast<int>(size) - static_cast<int>(PlayerPaddle::NormalSize);
	this->SetDimensions((PADDLE_WIDTH_TOTAL + diffFromNormalSize * PlayerPaddle::WIDTH_DIFF_PER_SIZE) / PADDLE_WIDTH_TOTAL);
}

/**
 * Set the dimensions of the paddle based on a scale factor given.
 * This will change the scale factor and bounds of the paddle.
 */
void PlayerPaddle::SetDimensions(float newScaleFactor) {
	this->currScaleFactor = newScaleFactor;
	assert(this->currScaleFactor > 0.0f);

	this->currHalfHeight			= this->currScaleFactor * PADDLE_HALF_HEIGHT;
	this->currHalfWidthTotal	= this->currScaleFactor * PADDLE_HALF_WIDTH;
	this->currHalfWidthFlat		= this->currScaleFactor * PADDLE_WIDTH_FLAT_TOP * 0.5f;
	
	// Reset the bounds of the paddle (in paddle-space)
	std::vector<Collision::LineSeg2D> lineBounds;
	std::vector<Vector2D>  lineNorms;
	
	// Top boundry
	Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthFlat, this->currHalfHeight));
	Vector2D n1(0, 1);
	lineBounds.push_back(l1);
	lineNorms.push_back(n1);

	// Bottom boundry
	Collision::LineSeg2D bottomLine(Point2D(this->currHalfWidthTotal, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
	Vector2D bottomNormal(0, -1);
	lineBounds.push_back(bottomLine);
	lineNorms.push_back(bottomNormal);

	// Side boundries
	Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
	Vector2D sideNormal1(1, 1);
	lineBounds.push_back(sideLine1);
	lineNorms.push_back(sideNormal1);
	
	Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
	Vector2D sideNormal2(-1, 1);
	lineBounds.push_back(sideLine2);
	lineNorms.push_back(sideNormal2);

	this->bounds = BoundingLines(lineBounds, lineNorms);
}

/**
 * Sets what the 'future' paddle size will be - this function does not immediately
 * change the size of the paddle but will cause the paddle to 'grow'/'shrink' to that
 * size as the tick function is called.
 */
void PlayerPaddle::SetPaddleSize(PlayerPaddle::PaddleSize size) {
	// If the current size is already the size being set then just exit
	if (this->currSize == size) {
		return;
	}
	this->currSize = size;
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
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(-this->currHalfWidthTotal, 0));
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
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(this->currHalfWidthTotal, 0));
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

	// Change the size gradually (lerp based on some constant time) if need be...
	int diffFromNormalSize = static_cast<int>(this->currSize) - static_cast<int>(PlayerPaddle::NormalSize);
	float targetScaleFactor = (PADDLE_WIDTH_TOTAL + diffFromNormalSize * PlayerPaddle::WIDTH_DIFF_PER_SIZE) / PADDLE_WIDTH_TOTAL;
	float scaleFactorDiff = targetScaleFactor - this->currScaleFactor;
	if (scaleFactorDiff != 0.0f) {
		this->SetDimensions(this->currScaleFactor + ((scaleFactorDiff * seconds) / SECONDS_TO_CHANGE_SIZE));
	}
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
	glPushMatrix();
	glTranslatef(this->centerPos[0], this->centerPos[1], 0.0f);
	this->bounds.DebugDraw();
	glPopMatrix();
}