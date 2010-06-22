#include "CannonBlock.h"
#include "GameBall.h"
#include "Projectile.h"
#include "GameEventManager.h"
#include "GameLevel.h"
#include "GameModel.h"
#include "EmptySpaceBlock.h"
#include "GameTransformMgr.h"

const float CannonBlock::CANNON_BARREL_LENGTH				= 1.20f;
const float CannonBlock::HALF_CANNON_BARREL_LENGTH	= CannonBlock::CANNON_BARREL_LENGTH	/ 2.0f;
const float CannonBlock::CANNON_BARREL_HEIGHT				= 0.8f;
const float CannonBlock::HALF_CANNON_BARREL_HEIGHT	= CannonBlock::CANNON_BARREL_HEIGHT / 2.0f;

// Rotation will happen for some random period of time in between these values
const double CannonBlock::MIN_ROTATION_TIME_IN_SECS	= 1.0f;
const double CannonBlock::MAX_ROTATION_TIME_IN_SECS = 2.5f;

// Rotation will occur at some random speed in between these values
const float CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC	= 150.0f;
const float CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC = 450.0f;

CannonBlock::CannonBlock(unsigned int wLoc, unsigned int hLoc) : LevelPiece(wLoc, hLoc), 
loadedBall(NULL), currRotationFromXInDegs(0.0f), currRotationSpeed(0.0f), elapsedRotationTime(0.0), 
totalRotationTime(0.0) {
}

CannonBlock::~CannonBlock() {
}

// Determine whether the given projectile will pass through this block...
bool CannonBlock::ProjectilePassesThrough(Projectile* projectile) {
	return false;
}

LevelPiece* CannonBlock::Destroy(GameModel* gameModel) {
	// You can't destroy the cannon block...
	return this;
}

// We need to override this in order to make sure it actually checks for a collision
bool CannonBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
		return this->bounds.Collide(dT, ball.GetBounds(), ball.GetVelocity(), n, collisionLine, timeSinceCollision);
}

bool CannonBlock::CollisionCheck(const Collision::AABB2D& aabb) const {
	// See if there's a collision between this and the given AABB
	return Collision::IsCollision(this->bounds.GenerateAABBFromLines(), aabb);	
}

bool CannonBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);
}

bool CannonBlock::CollisionCheck(const BoundingLines& boundingLines) const {
	return this->bounds.CollisionCheck(boundingLines);
}

/**
 * Updating the bounds on the cannon block is overridden - we only want the ball to go 
 * into the cannon block if it actually hits the cannon.
 */
void CannonBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
															 const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
															 const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
															 const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// We only update the bounds on the cannon block once...
	if (this->bounds.GetNumLines() > 0) {
		return;
	}

	// Make the bounds a bit smaller than a typical level piece and always make all of them
	// despite the neighboring blocks...

	// Clear all the currently existing boundry lines first
	this->bounds.Clear();

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D> boundingNorms;

	// Left boundry of the piece
	Collision::LineSeg2D l1(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT), 
							 this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n1(-1, 0);
	boundingLines.push_back(l1);
	boundingNorms.push_back(n1);

	// Bottom boundry of the piece
	Collision::LineSeg2D l2(this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n2(0, -1);
	boundingLines.push_back(l2);
	boundingNorms.push_back(n2);

	// Right boundry of the piece
	Collision::LineSeg2D l3(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, -CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n3(1, 0);
	boundingLines.push_back(l3);
	boundingNorms.push_back(n3);

	// Top boundry of the piece
	Collision::LineSeg2D l4(this->center + Vector2D(CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT),
							 this->center + Vector2D(-CannonBlock::HALF_CANNON_BARREL_LENGTH, CannonBlock::HALF_CANNON_BARREL_HEIGHT));
	Vector2D n4(0, 1);
	boundingLines.push_back(l4);
	boundingNorms.push_back(n4);

	this->bounds = BoundingLines(boundingLines, boundingNorms);
}

// The cannon block will capture the colliding ball and spin around a bit before shooting it in some random direction
LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	// If the cannon already has a ball inside it then just leave and have the ball bounce off of it
	// Also if the ball already just collided with this piece, ignore the collision
	if (this->loadedBall != NULL || ball.IsLastPieceCollidedWith(this) || ball.IsLoadedInCannonBlock()) {
		return this;
	}

	// Reset the elapsed rotation time
	this->elapsedRotationTime = 0.0;

	// Pick a random rotation speed
	this->currRotationSpeed = CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC	+ 
		Randomizer::GetInstance()->RandomNumZeroToOne() * (CannonBlock::MAX_ROTATION_SPD_IN_DEGS_PER_SEC - CannonBlock::MIN_ROTATION_SPD_IN_DEGS_PER_SEC);
	// Rotation direction is random as well...
	this->currRotationSpeed *= Randomizer::GetInstance()->RandomNegativeOrPositive();

	// Pick a random rotation time
	this->totalRotationTime = CannonBlock::MIN_ROTATION_TIME_IN_SECS +
		Randomizer::GetInstance()->RandomNumZeroToOne() * (CannonBlock::MAX_ROTATION_TIME_IN_SECS - CannonBlock::MIN_ROTATION_TIME_IN_SECS);
	assert(this->totalRotationTime > 0.0);

	// Place the ball in the cannon and make sure we don't continue colliding...
	ball.LoadIntoCannonBlock(this);
	ball.SetLastPieceCollidedWith(this);
	this->loadedBall = &ball;

	return this;
}

/**
 * Call this when a collision has actually occured with a projectile and this block.
 * Returns: The resulting level piece that this has become.
 */
LevelPiece* CannonBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {
		case Projectile::PaddleLaserBulletProjectile:
			// Do nothing, typical laser bullets just extinguish on contact
			break;

		case Projectile::CollateralBlockProjectile:
			// Do nothing, collateral block will be destroyed..
			break;

		case Projectile::PaddleRocketBulletProjectile:
			resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, this);
			break;

		default:
			assert(false);
			break;
	}

	return resultingPiece;
}

/**
 * Tell the cannon block to continually rotate (ONLY IF IT HAS A BALL INSIDE) and eventually
 * fire based on some random time.
 * Returns: true if the ball has fired, false if still rotating.
 */
bool CannonBlock::RotateAndEventuallyFire(double dT) {
	assert(this->totalRotationTime > 0.0);

	if (this->elapsedRotationTime >= this->totalRotationTime) {
		this->elapsedRotationTime = this->totalRotationTime;
		this->loadedBall = NULL;
		return true;
	}

	// Increment the rotation...
	float rotationIncrement = static_cast<float>(this->currRotationSpeed * dT);
	this->currRotationFromXInDegs += rotationIncrement;
	if (this->currRotationFromXInDegs >= 360.0f) {
		this->currRotationFromXInDegs -= 360.0f;
	}
	else if (this->currRotationFromXInDegs <= -360.0f) {
		this->currRotationFromXInDegs += 360.0f;
	}

	// Also rotate the the bounding lines appropriately...
	this->bounds.RotateLinesAndNormals(rotationIncrement, this->center);
	
	this->elapsedRotationTime += dT;

	return false;
}