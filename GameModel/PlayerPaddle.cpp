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
#include "PaddleLaser.h"

#include "Beam.h"
#include "Projectile.h"

// Default values for the size of the paddle
const float PlayerPaddle::PADDLE_WIDTH_TOTAL = 3.5f;
const float PlayerPaddle::PADDLE_WIDTH_FLAT_TOP = 2.0f;
const float PlayerPaddle::PADDLE_WIDTH_ANGLED_SIDE = (PADDLE_WIDTH_TOTAL - PADDLE_WIDTH_FLAT_TOP) / 2.0f;
const float PlayerPaddle::PADDLE_HEIGHT_TOTAL = 0.80f;
const float PlayerPaddle::PADDLE_DEPTH_TOTAL = 1.0f;
const float PlayerPaddle::PADDLE_HALF_WIDTH = PADDLE_WIDTH_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_HEIGHT = PADDLE_HEIGHT_TOTAL / 2.0f;
const float PlayerPaddle::PADDLE_HALF_DEPTH = PADDLE_DEPTH_TOTAL / 2.0f;

const Vector2D PlayerPaddle::DEFAULT_PADDLE_UP_VECTOR(0, 1);
const Vector2D PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR(1, 0);

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
const double PlayerPaddle::PADDLE_LASER_BULLET_DELAY = 0.25;

PlayerPaddle::PlayerPaddle() : 
	centerPos(0.0f, 0.0f), minBound(0.0f), maxBound(0.0f), speed(DEFAULT_SPEED), distTemp(0.0f), 
	avgVel(0.0f), ticksSinceAvg(0), timeSinceLastLaserBlast(PADDLE_LASER_BULLET_DELAY), 
	hitWall(false), currType(NormalPaddle), currSize(PlayerPaddle::NormalSize), 
	attachedBall(NULL), isPaddleCamActive(false), colour(1,1,1,1), isFiringBeam(false) {

	this->SetupAnimations();
	this->SetDimensions(PlayerPaddle::NormalSize);
}

PlayerPaddle::PlayerPaddle(float minBound, float maxBound) : 
speed(DEFAULT_SPEED), distTemp(0.0f), avgVel(0.0f), ticksSinceAvg(0), 
hitWall(false), isPaddleCamActive(false), colour(1,1,1,1), isFiringBeam(false) {

	this->SetupAnimations();
	this->SetMinMaxLevelBound(minBound, maxBound);
}

PlayerPaddle::~PlayerPaddle() {
}

/**
 * Private helper/initializer function for the paddle animations.
 */
void PlayerPaddle::SetupAnimations() {
	// The colour animation is used to change the colour or fade the paddle during 
	// certain animation sequences
	this->colourAnimation = AnimationLerp<ColourRGBA>(&this->colour);
	this->colourAnimation.SetRepeat(false);
	
	// The beam force downwards animation forces the paddle to move off the level/downwards
	// the amount of units equal to its current interpolated value when the paddle is firing its
	// laser beam power-up
	this->moveDownAnimation.ClearLerp();
	this->moveDownAnimation.SetInterpolantValue(0.0f);
	this->moveDownAnimation.SetRepeat(false);

	// Set the rotation animation - this sets the paddle at a certain rotation about the z-axis
	this->rotAngleZAnimation.ClearLerp();
	this->rotAngleZAnimation.SetInterpolantValue(0.0f);
	this->rotAngleZAnimation.SetRepeat(false);
}

// Regenerate the bounding lines of the player paddle so that they adhere to the size, position and rotation
// of the paddle as it currently lies.
void PlayerPaddle::RegenerateBounds() {
	std::vector<Collision::LineSeg2D> lineBounds;
	std::vector<Vector2D> lineNorms;

	if (this->isPaddleCamActive) {
		// When the paddle camera is active we compress the collision boundries down to the bottom of the paddle
		// this help make for a better game experience since the collision seems more natural when it's on the camera's view plane

		lineBounds.reserve(3);
		lineNorms.reserve(3);

		// 'Top' (flat) boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthFlat, -this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// 'Side' (oblique) boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, -this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1 = Vector2D(1, 1) / SQRT_2;
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2 = Vector2D(-1, 1) / SQRT_2;
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);
	}
	else {
		lineBounds.reserve(4);
		lineNorms.reserve(4);

		// Top boundry
		Collision::LineSeg2D l1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthFlat, this->currHalfHeight));
		Vector2D n1(0, 1);
		lineBounds.push_back(l1);
		lineNorms.push_back(n1);

		// Side boundries
		Collision::LineSeg2D sideLine1(Point2D(this->currHalfWidthFlat, this->currHalfHeight), Point2D(this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal1 = Vector2D(1, 1) / SQRT_2;
		lineBounds.push_back(sideLine1);
		lineNorms.push_back(sideNormal1);
		
		Collision::LineSeg2D sideLine2(Point2D(-this->currHalfWidthFlat, this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D sideNormal2 = Vector2D(-1, 1) / SQRT_2;
		lineBounds.push_back(sideLine2);
		lineNorms.push_back(sideNormal2);

		// Bottom boundry
		Collision::LineSeg2D bottomLine(Point2D(this->currHalfWidthTotal, -this->currHalfHeight), Point2D(-this->currHalfWidthTotal, -this->currHalfHeight));
		Vector2D bottomNormal(0, -1);
		lineBounds.push_back(bottomLine);
		lineNorms.push_back(bottomNormal);
	}

	this->bounds = BoundingLines(lineBounds, lineNorms);
	// Rotate the bounds (in the case where the paddle rotation has changed)
	this->bounds.RotateLinesAndNormals(this->rotAngleZAnimation.GetInterpolantValue(), Point2D(0,0));
	// Translate the bounds into game space
	this->bounds.TranslateBounds(Vector2D(this->GetCenterPosition()[0], this->GetCenterPosition()[1]));
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
	this->currHalfDepthTotal  = this->currScaleFactor * PADDLE_HALF_DEPTH;
	
	this->RegenerateBounds();
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
		ballReleaseDir = Vector2D::Rotate(randomAngleInDegs, ballReleaseDir);
	}
	else {
		// The paddle appears to be moving, modify the ball's release velocity
		// to reflect some of this movement
		float multiplier = PlayerPaddle::DEFAULT_SPEED / static_cast<float>(GameBall::NormalSpeed);
		Vector2D newBallDir = Vector2D::Normalize(ballReleaseDir + multiplier * avgPaddleVel);
		
		// and, of course, add some randomness...
		float randomAngleInDegs = static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne()) * GameBall::MOVING_RAND_RELEASE_DEG;		
		ballReleaseDir = Vector2D::Rotate(randomAngleInDegs, newBallDir);
	}

	ballReleaseDir.Normalize();

	// Set the ball velocity (tragectory it will leave the paddle on) and re-enable its collisions
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), ballReleaseDir);
	this->attachedBall->SetBallBallCollisionsEnabled();
	this->attachedBall->SetBallPaddleCollisionsEnabled();

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
void PlayerPaddle::MoveAttachedBallToNewBounds(double dT) {
	if (!this->HasBallAttached()) {
		return;
	}

	// We need to be careful when the collision boundries get changed the ball position could
	// be compromised; we need to make sure the ball also moves to be on the 
	// outside of the new paddle boundries
	assert(this->attachedBall != NULL);

	// Make sure the attached ball is sitting on the bounding lines and not inside the paddle...
	// Move the ball along the up vector until its no longer hitting...
	Point2D closestPtOnBounds = Collision::ClosestPoint(this->attachedBall->GetCenterPosition2D(), this->bounds.GetLine(0));
	this->attachedBall->SetCenterPosition(closestPtOnBounds + this->attachedBall->GetBounds().Radius() * this->GetUpVector());
}

void PlayerPaddle::Tick(double seconds) {
	Point2D startingCenterPos = this->centerPos;
	Point2D defaultCenterPos = this->GetDefaultCenterPosition();

	// If the beam is firing its laser then we want to animate it being pushed down
	this->centerPos[1] = defaultCenterPos[1] - this->moveDownAnimation.GetInterpolantValue();
	this->moveDownAnimation.Tick(seconds);

	// If the paddle is rotating (might have been hit or knocked around) we want to animate that rotation
	float startingZRotation = this->rotAngleZAnimation.GetInterpolantValue();
	this->rotAngleZAnimation.Tick(seconds);

	// Check to see if we need to increment seconds since the previous laser shot
	// This makes sure the user can't consecutively fire lasers like crazy
	if (this->timeSinceLastLaserBlast < PADDLE_LASER_BULLET_DELAY) {
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
	bool ballIsAttached = this->attachedBall != NULL;
	if (ballIsAttached) {	
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
	if (ballIsAttached) {
		// We need to figure out what the distance is from the paddle center-top to the ball center
		Vector2D paddleTopCenterToBallCenter = this->attachedBall->GetBounds().Center() - (oldPaddleCenter + Vector2D(0, this->GetHalfHeight()));
		// Maintain the vector obtained above in the new position of the paddle
		this->attachedBall->SetCenterPosition(this->centerPos + Vector2D(0, this->GetHalfHeight()) + paddleTopCenterToBallCenter);
	}

	// Check to see if the center position changed or its rotation changed, if so then regenerate the bounds
	if (startingCenterPos != this->centerPos || startingZRotation != this->rotAngleZAnimation.GetInterpolantValue()) {
		this->RegenerateBounds();
	}
}



void PlayerPaddle::SetIsLaserBeamFiring(bool isFiring) {
	if (isFiring) {

		// Start up the animation that pushes the paddle down (as if reacting to the sheer
		// force of the beam pressing down on it)
		std::vector<double> times;
		times.reserve(2);
		times.push_back(0.0f);
		times.push_back(0.05f);

		std::vector<float> values;
		values.reserve(2);
		values.push_back(this->moveDownAnimation.GetInterpolantValue());
		values.push_back(PlayerPaddle::PADDLE_HEIGHT_TOTAL);

		this->moveDownAnimation.SetLerp(times, values);	// Blended lerp??
	}
	else {
		this->moveDownAnimation.AppendLerp(0.1, 0.0f);
	}
	this->isFiringBeam = isFiring;
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
	
	// Check for laser beam paddle (this has top priority)
	if ((this->GetPaddleType() & PlayerPaddle::LaserBeamPaddle) == PlayerPaddle::LaserBeamPaddle && !this->isFiringBeam) {
		// We add the beam to the game model, the rest will be taken care of by the beam and model
		gameModel->AddBeam(Beam::PaddleLaserBeam);
	}
	// Check for laser bullet paddle (shoots little laser bullets from the paddle)
	else if ((this->GetPaddleType() & PlayerPaddle::LaserBulletPaddle) == PlayerPaddle::LaserBulletPaddle) {
		// Make sure we are allowed to fire a new laser bullet
		if (this->timeSinceLastLaserBlast >= PADDLE_LASER_BULLET_DELAY) {

			// Calculate the width and height of the laser bullet based on the size of the paddle...
			float projectileWidth  = this->GetPaddleScaleFactor() * PaddleLaser::PADDLELASER_WIDTH_DEFAULT;
			float projectileHeight = this->GetPaddleScaleFactor() * PaddleLaser::PADDLELASER_HEIGHT_DEFAULT;

			// Create the right type of projectile in the right place
			Projectile* newProjectile = Projectile::CreateProjectile(Projectile::PaddleLaserBulletProjectile, 
				this->GetCenterPosition() + Vector2D(0, this->currHalfHeight + 0.5f * projectileHeight));

			// Modify the fired bullet based on the current paddle's properties...
			newProjectile->SetWidth(projectileWidth);
			newProjectile->SetHeight(projectileHeight);

			// Fire ze laser bullet! - tell the model about it
			gameModel->AddProjectile(newProjectile);

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
	// Reset the ball so that it has no previous record of the level piece it last hit
	ball->SetLastPieceCollidedWith(NULL);

	// Make sure the position of the ball is sitting on-top of the paddle
	Vector2D normal;
	Collision::LineSeg2D collisionLine;
	double timeSinceCollision;
	bool onPaddle = this->CollisionCheck(*this->attachedBall, 0.0, normal, collisionLine, timeSinceCollision);
	if (onPaddle) {
		// Position the ball so that it is against the collision line
		const Collision::Circle2D& ballBounds = this->attachedBall->GetBounds();
		//this->attachedBall->SetCenterPosition(ballBounds.Center() + (ballBounds.Radius() - distance - 5 * EPSILON) * - this->attachedBall->GetDirection());
		this->attachedBall->SetCenterPosition(ballBounds.Center() + (EPSILON + timeSinceCollision) * -this->attachedBall->GetVelocity());
	}

	// Remove the ball's direction but not its speed
	this->attachedBall->SetVelocity(this->attachedBall->GetSpeed(), Vector2D(0, 0));

	// Disable collisions for the ball (reenable them when the ball is detached)
	this->attachedBall->SetBallBallCollisionsDisabled();
	this->attachedBall->SetBallPaddleCollisionsDisabled();

	return true;
}

// Called when the paddle is hit by a projectile
void PlayerPaddle::HitByProjectile(const Projectile& projectile) {
	// Different projectiles have different effects on the paddle...
	switch (projectile.GetType()) {
		case Projectile::CollateralBlockProjectile:
			// Causes the paddle to be violently thrown off course for a short period of time...
			this->CollateralBlockProjectileCollision(projectile);
			break;
		case Projectile::PaddleLaserBulletProjectile:
			this->LaserBulletProjectileCollision(projectile);
			break;
		default:
			assert(false);
			break;
	}

	// EVENT: Paddle was just hit by a projectile
	GameEventManager::Instance()->ActionPaddleHitByProjectile(*this, projectile);
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
	this->bounds.DebugDraw();
	glPopMatrix();
}

/**
 * Do the paddle collision with the collateral block projectile - this will knock the
 * the paddle off course and rotate it a bit.
 */
void PlayerPaddle::CollateralBlockProjectileCollision(const Projectile& projectile) {
	float distFromCenter = 0.0f;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile, distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

	static const float MAX_HIT_ROTATION = 60.0f;
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
	//else {
		// The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
	//}

	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	static const double HIT_EFFECT_TIME = 2.5;
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = 3.0f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * 2.25f * PlayerPaddle::PADDLE_HEIGHT_TOTAL;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	std::vector<float> rotationValues;
	rotationValues.reserve(3);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
}

// A laser bullet just collided with the paddle...
void PlayerPaddle::LaserBulletProjectileCollision(const Projectile& projectile) {
	float distFromCenter = 0.0;
	// Find percent distance from edge to center of the paddle
	float percentNearCenter = this->GetPercentNearPaddleCenter(projectile, distFromCenter);
	float percentNearEdge = 1.0 - percentNearCenter;

	static const float MAX_HIT_ROTATION = 15.0f;
	
	float rotationAmount = percentNearEdge * MAX_HIT_ROTATION;
	if (distFromCenter > 0.0) {
		// The projectile hit the 'right' side of the paddle (typically along the positive x-axis)
		rotationAmount *= -1.0f;
	}
	//else {
		// The projectile hit the 'left' side of the paddle (typically along the negative x-axis)
	//}
	
	// Set up the paddle to move down (and eventually back up) and rotate out of position then eventually back into its position
	static const double HIT_EFFECT_TIME = 1.5;
	std::vector<double> times;
	times.reserve(3);
	times.push_back(0.0f);
	times.push_back(0.05f);
	times.push_back(HIT_EFFECT_TIME);

	static const float MIN_MOVE_DOWN_AMT = PlayerPaddle::PADDLE_HEIGHT_TOTAL;
	float totalMaxMoveDown = MIN_MOVE_DOWN_AMT + percentNearCenter * PlayerPaddle::PADDLE_HEIGHT_TOTAL;

	std::vector<float> moveDownValues;
	moveDownValues.reserve(3);
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue());
	moveDownValues.push_back(this->moveDownAnimation.GetInterpolantValue() + totalMaxMoveDown);
	moveDownValues.push_back(0.0f);

	this->moveDownAnimation.SetLerp(times, moveDownValues);

	std::vector<float> rotationValues;
	rotationValues.reserve(3);
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue());
	rotationValues.push_back(this->rotAngleZAnimation.GetInterpolantValue() + rotationAmount);
	rotationValues.push_back(0.0f);

	this->rotAngleZAnimation.SetLerp(times, rotationValues);
}

// Get an idea of how close the given projectile is to the center of the paddle as a percentage
// where 0 is a far away from the center as possible and 1 is at the center. Also returns the value
// of distFromCenter which is assigned a negative/positive value based on the distance from the center
// of the paddle on the left/right side respectively.
float PlayerPaddle::GetPercentNearPaddleCenter(const Projectile& projectile, float& distFromCenter) {
	// Figure out what side of the paddle the projectile is hitting...
	// Use a line test...
	Vector2D vecToProjectile = projectile.GetPosition() - this->GetCenterPosition();
	Vector2D positivePaddleAxis = Vector2D::Rotate(this->rotAngleZAnimation.GetInterpolantValue(), PlayerPaddle::DEFAULT_PADDLE_RIGHT_VECTOR);

	// Use a dot product to determine whether the projectile is on the positive side or not
	distFromCenter = Vector2D::Dot(vecToProjectile, positivePaddleAxis);

	// Find percent distance from edge to center of the paddle
	float percentNearCenter = std::max<float>(0.0f, this->currHalfWidthTotal - fabs(distFromCenter)) / this->currHalfWidthTotal;
	assert(percentNearCenter >= 0.0f && percentNearCenter <= 1.0);
	return percentNearCenter;
}