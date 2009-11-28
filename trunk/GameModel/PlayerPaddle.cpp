/**
 * PlayerPaddle.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PlayerPaddle.h"
#include "GameModel.h"
#include "GameEventManager.h"
#include "GameModelConstants.h"
#include "Projectile.h"
#include "GameBall.h"

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
	hitWall(false), currType(NormalPaddle), currSize(PlayerPaddle::NormalSize), 
	attachedBall(NULL), isPaddleCamActive(false), colour(1,1,1,1) {

	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);

	this->SetDimensions(PlayerPaddle::NormalSize);
}

PlayerPaddle::PlayerPaddle(float minBound, float maxBound) : 
speed(DEFAULT_SPEED), distTemp(0.0f), avgVel(0.0f), ticksSinceAvg(0), 
hitWall(false), isPaddleCamActive(false), colour(1,1,1,1) {

	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);

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
	lineBounds.reserve(3);
	lineNorms.reserve(3);
	
	if (this->isPaddleCamActive) {
		// When the paddle camera is active we compress the collision boundries down to the bottom of the paddle
		// this help make for a better game experience since the collision seems more natural when it's on the camera's view plane

		// 'Top' (flat) boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthFlat, -this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// 'Side' (oblique) boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1(1, 1);
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2(-1, 1);
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);
	}
	else {
		// Top boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthFlat, this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// Side boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1(1, 1);
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2(-1, 1);
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);
	}

	this->bounds = BoundingLines(lineBounds, lineNorms);

	// We need to be careful when the collision boundries get changed the ball position could
	// be compromised; we need to make sure the ball also moves to be on the 
	// outside of the new paddle boundries
	if (this->HasBallAttached()) {
		assert(this->attachedBall != NULL);
		// TODO
	}
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

/**
 * Private helper function that shoots the ball attached to the player paddle.
 */
void PlayerPaddle::FireAttachedBall() {
	assert(this->attachedBall != NULL);

	// Set the ball's new trajectory in order for it to leave the paddle
	Vector2D ballReleaseDir = GameBall::STD_INIT_VEL_DIR;

	// Get the paddle's avg. velocity
	Vector2D avgPaddleVel = this->GetAvgVelocity();

	// Check to see if the paddle is moving, if not just use a random angle
	if (fabs(avgPaddleVel[0]) <= EPSILON) {
		// Add some randomness to the velocity by deviating a straight-up shot by some random angle
		float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::STILL_RAND_RELEASE_DEG;		
		ballReleaseDir = Rotate(randomAngleInDegs, ballReleaseDir);
	}
	else {
		// The paddle appears to be moving, modify the ball's release velocity
		// to reflect some of this movement
		float multiplier = PlayerPaddle::DEFAULT_SPEED / static_cast<float>(GameBall::NormalSpeed);
		Vector2D newBallDir = Vector2D::Normalize(ballReleaseDir + multiplier * avgPaddleVel);
		
		// and, of course, add some randomness...
		float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::MOVING_RAND_RELEASE_DEG;		
		ballReleaseDir = Rotate(randomAngleInDegs, newBallDir);
	}

	ballReleaseDir.Normalize();

	// Set the ball velocity (tragectory it will leave the paddle on) and re-enable its collisions
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), ballReleaseDir);
	this->attachedBall->SetBallCollisionsEnabled();

	// EVENT: Ball Shot
	GameEventManager::Instance()->ActionBallShot(*this->attachedBall);

	// Remove the ball from the paddle
	this->attachedBall = NULL;
}

/**
 * Private helper function used to solve the issue where a ball that is attached
 * to the paddle when the paddle boundries change doesn't get moved to the new boundries.
 * e.g., when in paddle cam mode and then changing back while the sticky paddle is active.
 */
void PlayerPaddle::MoveAttachedBallToNewBounds() {
	if (!this->HasBallAttached()) {
		return;
	}

	// Figure out if the ball is colliding with any of the boundries of the paddle
	// if it is we can easily figure out where we have to move the ball to make 
	// it accommodate those new boundries
	Vector2D normal(0, 0);
	float distance = 0.0f;
	Collision::Circle2D& ballBounds = this->attachedBall->GetBounds();
	bool isCollision = this->CollisionCheck(ballBounds, normal, distance);

	if (isCollision) {
		this->attachedBall->SetCenterPosition(ballBounds.Center() + (ballBounds.Radius() - distance - 5 * EPSILON) * normal);
	}
	else {
		// Evasive measures... just do whatever it takes to get the ball on the outside of the paddle...

		// Move the ball into paddle space and find the closest point to the center
		Point2D ballCenterInPaddleSpace = ballBounds.Center() - Vector2D(this->centerPos[0], this->centerPos[1]);
		Point2D closestPtInPaddleSpace = this->bounds.ClosestPoint(ballCenterInPaddleSpace);
		Vector2D closestPtDir   = Vector2D::Normalize(closestPtInPaddleSpace - ballCenterInPaddleSpace);

		// Now we just attach the ball at the closest point on the paddle...
		Point2D closestPtInWorldSpace = closestPtInPaddleSpace + Vector2D(this->centerPos[0], this->centerPos[1]);
		this->attachedBall->SetCenterPosition(closestPtInWorldSpace + ballBounds.Radius() * closestPtDir);
	}	

}

void PlayerPaddle::Tick(double seconds) {
	// Check to see if we need to increment seconds since the previous laser shot
	// This makes sure the user can't consecutively fire lasers like some sort of mad person
	if (this->timeSinceLastLaserBlast < PADDLE_LASER_DELAY) {
		this->timeSinceLastLaserBlast += seconds;
	}

	float distanceTravelled = this->distTemp * seconds;
	float newCenterX = this->centerPos[0] + (this->distTemp * seconds);
	float minNewXPos = newCenterX - this->currHalfWidthTotal;
	float maxNewXPos = newCenterX + this->currHalfWidthTotal;
	Point2D oldPaddleCenter = this->centerPos;
	
	float halfWidthTotalWithBallMin = this->currHalfWidthTotal;
	float halfWidthTotalWithBallMax = this->currHalfWidthTotal;

	// If a ball is attached, it could affect the min and max x boundries of the paddle so 
	// adjust the boundries based on the position of the ball on the paddle
	if (this->attachedBall != NULL) {	
		float ballX = this->attachedBall->GetBounds().Center()[0] + distanceTravelled;
		float ballMinX = ballX - this->attachedBall->GetBounds().Radius();
		float ballMaxX = ballX + this->attachedBall->GetBounds().Radius();
		
		// If it's the case that the ball increases the size of the paddle on the -x dir
		// then accomodate this, same goes for +x direction
		if (ballMinX < minNewXPos) {
			halfWidthTotalWithBallMin += (minNewXPos - ballMinX);
			minNewXPos = ballMinX;
		}
		else if (ballMaxX > maxNewXPos) {
			halfWidthTotalWithBallMax += (ballMaxX - maxNewXPos);
			maxNewXPos = ballMaxX;
		}
	}

	if (minNewXPos - EPSILON <= this->minBound) {
		// The paddle bumped into the left wall
		this->centerPos[0] = this->minBound + halfWidthTotalWithBallMin;
		
		if (this->hitWall) {
			this->distTemp = 0.0f;
		}
		else {
			// EVENT: paddle hit left wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(-halfWidthTotalWithBallMin, 0));
		}

		this->hitWall = true;
	}
	else if (maxNewXPos + EPSILON > this->maxBound) {
		// The paddle bumped into the right wall
		this->centerPos[0] = this->maxBound - halfWidthTotalWithBallMax;
		
		if (this->hitWall) {
			this->distTemp = 0.0f;
		}
		else {
			// EVENT: paddle hit right wall for first time
			GameEventManager::Instance()->ActionPaddleHitWall(*this, this->centerPos + Vector2D(halfWidthTotalWithBallMax, 0));
		}
		this->hitWall = true;
	}
	else {
		this->centerPos[0] = newCenterX;
		this->hitWall = false;
	}

	// TODO: Fix this to work nicer
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

	// If there is a ball attached then we need to move it around with the paddle
	if (this->attachedBall != NULL) {
		// We need to figure out what the distance is from the paddle center-top to the ball center
		Vector2D paddleTopCenterToBallCenter = this->attachedBall->GetBounds().Center() - (oldPaddleCenter + Vector2D(0, this->GetHalfHeight()));
		// Maintain the vector obtained above in the new position of the paddle
		this->attachedBall->SetCenterPosition(this->centerPos + Vector2D(0, this->GetHalfHeight()) + paddleTopCenterToBallCenter);
	}
}

void PlayerPaddle::Animate(double seconds) {
	// Tick any paddle-related animations
	this->colourAnimation.Tick(seconds);
}

/**
 * This will fire any weapons or abilities that paddle currently has - if none
 * exist then this function does nothing.
 */
void PlayerPaddle::Shoot(GameModel* gameModel) {
	// Check for the various paddle types and react appropriately for each
	
	// If there's a ball attached to the paddle then we release it and exit
	if (this->HasBallAttached()) {
		this->FireAttachedBall();
		return;
	}
	
	// Check for laser paddle
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

/**
 * Attaches the given ball to the paddle if there is no ball already attached
 * to the paddle.
 * Returns: true if the ball was successfully attached, false otherwise.
 */
bool PlayerPaddle::AttachBall(GameBall* ball) {
	assert(ball != NULL);

	// If the paddle already has a ball attached then we do nothing
	if (this->HasBallAttached()) {
		return false;
	}

	// Attach the ball
	this->attachedBall = ball;

	// Make sure the position of the ball is sitting on-top of the paddle
	Vector2D normal;
	float distance;
	bool onPaddle = this->CollisionCheck(this->attachedBall->GetBounds(), normal, distance);
	if (onPaddle) {
		// Position the ball so that it is against the collision line, exactly
		Collision::Circle2D& ballBounds = this->attachedBall->GetBounds();
		this->attachedBall->SetCenterPosition(ballBounds.Center() + (ballBounds.Radius() - distance - 5 * EPSILON) * - this->attachedBall->GetDirection());
	}

	// Remove the ball's direction but not its speed
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), Vector2D(0, 0));

	// Disable collisions for the ball (reenable them when the ball is detached)
	this->attachedBall->SetBallCollisionsDisabled();

	return true;
}

bool PlayerPaddle::CollisionCheck(const Collision::Circle2D& c, Vector2D& n, float& d) {
	// Move the circle into paddle space
	Collision::Circle2D temp = Collision::Circle2D(c.Center() - Vector2D(this->centerPos[0], this->centerPos[1]), c.Radius());
	return this->bounds.Collide(temp, n, d);
}

/**
 * Adds an animation to the paddle that fades it in or out based on the
 * given parameter over the given amount of time.
 */
void PlayerPaddle::AnimateFade(bool fadeOut, double duration) {
	ColourRGBA finalColour = this->colour;
	if (fadeOut) {
		finalColour[3] = 0.0f;
	}
	else {
		finalColour[3] = 1.0f;
	}

	this->colourAnimation.SetLerp(duration, finalColour);
}

void PlayerPaddle::DebugDraw() const {
	glPushMatrix();
	glTranslatef(this->centerPos[0], this->centerPos[1], 0.0f);
	this->bounds.DebugDraw();
	glPopMatrix();
}
