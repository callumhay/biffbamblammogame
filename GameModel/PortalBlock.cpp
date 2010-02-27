
#include "PortalBlock.h"
#include "Projectile.h"

PortalBlock::PortalBlock(unsigned int wLoc, unsigned int hLoc, const PortalBlock* sibling) : LevelPiece(wLoc, hLoc), sibling(sibling) {
}

PortalBlock::~PortalBlock() {
}

LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, const GameBall& ball) {
	//ball.TeleportViaPortal(this->sibling);
	return this;
}

LevelPiece* PortalBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	// Send the projectile to the sibling portal...
	std::list<Collision::Ray2D> newProjectileRay = this->GetReflectionRefractionRays(projectile->GetPosition(), projectile->GetVelocityDirection());
	assert(newProjectileRay.size() == 1);
	
	// Change the projectile so that it comes out of the sibling portal
	const Collision::Ray2D& ray = *newProjectileRay.begin();
	projectile->SetPosition(ray.GetOrigin());
	projectile->SetVelocity(ray.GetUnitDirection(), projectile->GetVelocityMagnitude());

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
		nonUsedPortalColours.push_back(0.85f * Colour(0.3882f, 0.72157f, 1.0f));			// 4 Steely-sky Blue
		nonUsedPortalColours.push_back(0.8f * Colour(0.0f, 0.8078f, 0.8196f));			// 5 Dark Turquoise
		nonUsedPortalColours.push_back(0.85f * Colour(0.0f, 0.788f, 0.3412f));				// 6 Emerald Green
		nonUsedPortalColours.push_back(0.8f * Colour(1.0f, 0.7568f, 0.1451f));			// 7 Orangish-yellow
		nonUsedPortalColours.push_back(0.8f * Colour(0.7529f, 1.0f, 0.2431f));			// 8 Yellowish-green

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