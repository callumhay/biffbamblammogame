
#include "PortalBlock.h"
#include "Projectile.h"
#include "GameBall.h"
#include "GameEventManager.h"

PortalBlock::PortalBlock(unsigned int wLoc, unsigned int hLoc, PortalBlock* sibling) : LevelPiece(wLoc, hLoc), sibling(sibling) {
}

PortalBlock::~PortalBlock() {
}


// NOTE ABOUT PORTAL BLOCK COLLISION CHECKING:
// The portal block is a 'no-bounds' block, however it does still 'consider' collisions
// since it needs to know if something hits it so that when it does it can move it to its sibling portal...

bool PortalBlock::CollisionCheck(const GameBall& ball, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	// No collision if the ball has just previously collided with this portal block
	if (ball.IsLastPieceCollidedWith(this)) {
		return false;
	}

	// Create a line for the ball over time
	const Point2D& currCenter = ball.GetBounds().Center();
	Point2D previousCenter = currCenter - dT * ball.GetVelocity();
	Collision::LineSeg2D sweptCenter(previousCenter, currCenter);

	// Check to see if the circle is/was inside the bounding lines over the time interval dT
	return (this->bounds.CollisionCheckIndices(sweptCenter).size() > 0);
}

bool PortalBlock::CollisionCheck(const Collision::AABB2D& aabb) const {
	// Check to see if the center of the AABB is in the bounding lines
	return this->bounds.IsInside(Point2D::GetMidPoint(aabb.GetMin(), aabb.GetMax()));
}

bool PortalBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return this->bounds.CollisionCheck(ray, rayT);	
}

bool PortalBlock::CollisionCheck(const BoundingLines& boundingLines) const {
	return this->bounds.CollisionCheck(boundingLines);
}

void PortalBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
															 const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
															 const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
															 const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
	// Make the bounds a bit smaller than a typical level piece and always make all of them

	// Clear all the currently existing boundry lines first
	this->bounds.Clear();

	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	static const float FRACTION_HALF_PIECE_WIDTH  = 0.75f * LevelPiece::HALF_PIECE_WIDTH;
	static const float FRACTION_HALF_PIECE_HEIGHT = 0.75f * LevelPiece::HALF_PIECE_HEIGHT;

	// Left boundry of the piece
	Collision::LineSeg2D l1(this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT), 
							 this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT));
	Vector2D n1(-1, 0);
	boundingLines.push_back(l1);
	boundingNorms.push_back(n1);

	// Bottom boundry of the piece
	Collision::LineSeg2D l2(this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT),
							 this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT));
	Vector2D n2(0, -1);
	boundingLines.push_back(l2);
	boundingNorms.push_back(n2);

	// Right boundry of the piece
	Collision::LineSeg2D l3(this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, -FRACTION_HALF_PIECE_HEIGHT),
							 this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT));
	Vector2D n3(1, 0);
	boundingLines.push_back(l3);
	boundingNorms.push_back(n3);

	// Top boundry of the piece
	Collision::LineSeg2D l4(this->center + Vector2D(FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT),
							 this->center + Vector2D(-FRACTION_HALF_PIECE_WIDTH, FRACTION_HALF_PIECE_HEIGHT));
	Vector2D n4(0, 1);
	boundingLines.push_back(l4);
	boundingNorms.push_back(n4);

	this->bounds = BoundingLines(boundingLines, boundingNorms);
}

LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, GameBall& ball) {
	// Tell the ball what the last piece it collided with was the sibling so it doesn't
	// keep teleporting back and forth between this and its sibling...
	ball.SetLastPieceCollidedWith(this->sibling);

	// EVENT: The ball has officially entered the portal...
	GameEventManager::Instance()->ActionBallPortalBlockTeleport(ball, *this);

	// Teleport the ball to the sibling portal block:
	ball.SetCenterPosition(this->sibling->GetCenter());

	return this;
}

/**
 * Call when a projectile hits the portal block. This tends to cause the projectile to
 * be teleported to the sibling portal.
 */
LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// Ignore the collision if this was the last piece collided with
	if (projectile->IsLastLevelPieceCollidedWith(this)) {
		return this;
	}

	// Send the projectile to the sibling portal...
	Point2D newPosition = projectile->GetPosition() + projectile->GetHeight() * projectile->GetVelocityDirection();
	std::list<Collision::Ray2D> newProjectileRay = this->GetReflectionRefractionRays(newPosition, projectile->GetVelocityDirection());
	assert(newProjectileRay.size() == 1);
	
	// EVENT: The projectile is officially being teleported...
	GameEventManager::Instance()->ActionProjectilePortalBlockTeleport(*projectile, *this);

	// Change the projectile so that it comes out of the sibling portal
	const Collision::Ray2D& ray = *newProjectileRay.begin();
	projectile->SetPosition(ray.GetOrigin());
	projectile->SetVelocity(ray.GetUnitDirection(), projectile->GetVelocityMagnitude());

	// Make sure the projectile knows that the last piece it collided with was the sibling so
	// that when it comes out of the sibling it doesn't collide again
	projectile->SetLastLevelPieceCollidedWith(this->sibling);

	return this;
}

/**
 * Calculates what happens when a ray goes through the portal and comes out of its sibling portal.
 */
std::list<Collision::Ray2D> PortalBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const {
	// Find position difference between the point and center...
	Vector2D toHitVec = hitPoint - this->GetCenter();
	// The new ray is simply the old one coming out of the sibling portal
	Point2D newPosition = this->sibling->GetCenter() + toHitVec;
	
	std::list<Collision::Ray2D> result;
	result.push_back(Collision::Ray2D(newPosition, impactDir));
	return result;
}

// Whether or not the portal colour generator will reset
bool PortalBlock::portalGeneratorReset = true;

// Resets the portal colour generator so that next time GeneratePortalColour() is called
// it will start over again.
void PortalBlock::ResetPortalColourGenerator() {
	PortalBlock::portalGeneratorReset = true;
}

/**
 * Static function that assigns portal colours to a portal and its sibling
 * this helps distinguish, to the user, what portals match up.... and it looks prettier.
 */
Colour PortalBlock::GeneratePortalColour() {
	static const int MAX_PORTAL_COLOURS = 8;
	
	static std::vector<Colour> nonUsedPortalColours;
	static std::vector<Colour> usedPortalColours;
	if (PortalBlock::portalGeneratorReset ) {
		nonUsedPortalColours.clear();
		nonUsedPortalColours.reserve(MAX_PORTAL_COLOURS);
		usedPortalColours.clear();
		usedPortalColours.reserve(MAX_PORTAL_COLOURS);

		// We multiply all the colours by fractions to make sure they aren't over
		// saturated when rendered
		nonUsedPortalColours.push_back(0.8f * Colour(0.8627f, 0.07843f, 0.2353f));	// 1 Crimson
		nonUsedPortalColours.push_back(0.8f * Colour(0.58f, 0.0f, 0.8274f));				// 2 Dark Violet
		nonUsedPortalColours.push_back(0.8f * Colour(0.4117f, 0.3490f, 0.8039f));		// 3 Blueish-Purple
		nonUsedPortalColours.push_back(0.8f * Colour(0.3882f, 0.72157f, 1.0f));			// 4 Steely-sky Blue
		nonUsedPortalColours.push_back(0.8f * Colour(0.0f, 0.8078f, 0.8196f));			// 5 Dark Turquoise
		nonUsedPortalColours.push_back(0.8f * Colour(0.0f, 0.788f, 0.3412f));				// 6 Emerald Green
		nonUsedPortalColours.push_back(0.7f * Colour(1.0f, 0.7568f, 0.1451f));			// 7 Orangish-yellow
		nonUsedPortalColours.push_back(0.7f * Colour(0.7529f, 1.0f, 0.2431f));			// 8 Yellowish-green

		PortalBlock::portalGeneratorReset = false;
	}

	// In the case where we've used up all the portals then start over again...
	if (nonUsedPortalColours.size() == 0) {
		assert(usedPortalColours.size() == MAX_PORTAL_COLOURS);
		nonUsedPortalColours = usedPortalColours;
		usedPortalColours.clear();
		usedPortalColours.reserve(MAX_PORTAL_COLOURS);
	}

	// Choose a random colour from the set of portal colours not used yet...
	unsigned int randomIndex = Randomizer::GetInstance()->RandomUnsignedInt() % nonUsedPortalColours.size();
	Colour colourToUse = nonUsedPortalColours[randomIndex];
	nonUsedPortalColours.erase(nonUsedPortalColours.begin() + randomIndex);
	usedPortalColours.push_back(colourToUse);

	return colourToUse;
}