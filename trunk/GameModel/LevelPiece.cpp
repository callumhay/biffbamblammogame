/**
 * LevelPiece.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "LevelPiece.h"
#include "BreakableBlock.h"
#include "GameEventManager.h"
#include "GameBall.h"
#include "GameModel.h"
#include "GameItem.h"
#include "GameItemFactory.h"
#include "GameModelConstants.h"
#include "Beam.h"
#include "PaddleLaserProjectile.h"
#include "PaddleMineProjectile.h"
#include "FireGlobProjectile.h"

const LevelPiece::TriggerID LevelPiece::NO_TRIGGER_ID = -1;

const float LevelPiece::PIECE_WIDTH = 2.5f;
const float LevelPiece::PIECE_HEIGHT = 1.0f;
const float LevelPiece::PIECE_DEPTH  = 1.30f;
const float LevelPiece::HALF_PIECE_WIDTH  = PIECE_WIDTH / 2.0f;
const float LevelPiece::HALF_PIECE_HEIGHT = PIECE_HEIGHT / 2.0f;
const float LevelPiece::HALF_PIECE_DEPTH  = PIECE_DEPTH / 2.0f;

LevelPiece::LevelPiece(unsigned int wLoc, unsigned int hLoc) : 
colour(1,1,1,1), pieceStatus(LevelPiece::NormalStatus), leftNeighbor(NULL),
bottomNeighbor(NULL), rightNeighbor(NULL), topNeighbor(NULL), topRightNeighbor(NULL),
topLeftNeighbor(NULL), bottomRightNeighbor(NULL), bottomLeftNeighbor(NULL), 
triggerID(LevelPiece::NO_TRIGGER_ID) {
	this->SetWidthAndHeightIndex(wLoc, hLoc);
}

LevelPiece::~LevelPiece() {
	// We need to be sure to obliterate any status effects that might be lingering on the level piece -
	// this emits an important event to any model listeners to remove those effects as well!
	this->RemoveAllStatus(NULL);

    // Make sure we remove all attached projectiles as well
    // NOTE: DO NOT USE ITERATORS HERE SINCE THE MINE PROJECTILE DETACHES ITSELF IN THE SetAsFalling call
    while (!this->attachedProjectiles.empty()) {
                 
        MineProjectile* p = *this->attachedProjectiles.begin();
        p->SetLastThingCollidedWith(NULL);
        p->SetAsFalling();
        this->DetachProjectile(p); // This is redundant and will be ignored, just here for robustness
    }
    this->attachedProjectiles.clear();
}

// Static function to determine if the piece type given is valid
bool LevelPiece::IsValidLevelPieceType(int pieceType) {
    switch (pieceType) {
        case LevelPiece::Breakable:
        case LevelPiece::Solid:
        case LevelPiece::Empty:
        case LevelPiece::Bomb:
        case LevelPiece::SolidTriangle:
        case LevelPiece::BreakableTriangle:
        case LevelPiece::Ink:
        case LevelPiece::Prism:
        case LevelPiece::Portal:
        case LevelPiece::PrismTriangle:
        case LevelPiece::Cannon:
        case LevelPiece::Collateral:
        case LevelPiece::Tesla:
        case LevelPiece::ItemDrop:
        case LevelPiece::Switch:
        case LevelPiece::OneWay:
        case LevelPiece::NoEntry:
        case LevelPiece::LaserTurret:
        case LevelPiece::RocketTurret:
        case LevelPiece::MineTurret:
        case LevelPiece::AlwaysDrop:
        case LevelPiece::Regen:
            return true;
        default:
            assert(false);
            break;
    }
    return false;
}

void LevelPiece::SetWidthAndHeightIndex(unsigned int wLoc, unsigned int hLoc) {
	this->wIndex = wLoc;
	this->hIndex = hLoc;
	this->center = Point2D(wLoc * PIECE_WIDTH + HALF_PIECE_WIDTH, hLoc * PIECE_HEIGHT + HALF_PIECE_HEIGHT);
}

/** 
 * The default update bounds method used by subclasses - this will update the boundries
 * based on a generalized idea that if there is a boundry piece type on a certain side
 * of this piece then there is no need for another boundry on that side. In all other cases
 * the boundry will be placed there.
 */
void LevelPiece::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                              const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                              const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                              const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	UNUSED_PARAMETER(bottomLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);

	// Set the bounding lines for a rectangular block - these are also used when any block is frozen in an ice cube
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

    bool shouldGenBounds = false;

	// Left boundry of the piece
    shouldGenBounds = (leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        leftNeighbor->GetType() != LevelPiece::Solid);
    if (shouldGenBounds) {
		Collision::LineSeg2D l1(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n1(-1, 0);
		boundingLines.push_back(l1);
		boundingNorms.push_back(n1);
    }


	// Bottom boundry of the piece
    shouldGenBounds = (bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        bottomNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
		Collision::LineSeg2D l2(this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n2(0, -1);
		boundingLines.push_back(l2);
		boundingNorms.push_back(n2);
	}

	// Right boundry of the piece
    shouldGenBounds = (rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        rightNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
		Collision::LineSeg2D l3(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n3(1, 0);
		boundingLines.push_back(l3);
		boundingNorms.push_back(n3);
	}

	// Top boundry of the piece
    shouldGenBounds = (topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        topNeighbor->HasStatus(LevelPiece::OnFireStatus) || topNeighbor->GetType() != LevelPiece::Solid);
	if (shouldGenBounds) {
		Collision::LineSeg2D l4(this->center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
								 this->center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
		Vector2D n4(0, 1);
		boundingLines.push_back(l4);
		boundingNorms.push_back(n4);
	}

	this->SetBounds(BoundingLines(boundingLines, boundingNorms), leftNeighbor, bottomNeighbor,
        rightNeighbor, topNeighbor, topRightNeighbor, topLeftNeighbor, 
        bottomRightNeighbor, bottomLeftNeighbor);
}


bool LevelPiece::ProjectileIsDestroyedOnCollision(const Projectile* projectile) const {
    switch (projectile->GetType()) {

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // Mines are typically not destroyed by collisions with blocks.
            return false;

        default:
            break;
    }

    return !this->ProjectilePassesThrough(projectile);
}

void LevelPiece::AddStatus(GameLevel* level, const PieceStatus& status) {
	// If the piece already has the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) == status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus | status);
	
	// We update the bounds since the status can change the nature of the boundaries generated for this block and
    // its surrounding blocks
    level->UpdateBoundsOnPieceAndSurroundingPieces(this);

	// EVENT: A status has been added to this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusAdded(*this, status);
}

void LevelPiece::RemoveStatus(GameLevel* level, const PieceStatus& status) {
	// If the piece doesn't have the status then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if ((this->pieceStatus & status) != status) {
		return;
	}

	this->pieceStatus = (this->pieceStatus & ~status);
	
	// We update the bounds since the status can change the nature of the boundaries generated for this block and
    // its surrounding blocks
    level->UpdateBoundsOnPieceAndSurroundingPieces(this);

	// EVENT: A status has been removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceStatusRemoved(*this, status);
}

void LevelPiece::RemoveStatuses(GameLevel* level, int32_t statusMask) {
	bool statusFound = false;
	// Go through each status and remove it...
	if ((statusMask & LevelPiece::OnFireStatus) == LevelPiece::OnFireStatus) {
		this->RemoveStatus(level, LevelPiece::OnFireStatus);
		statusFound = true;
	}
	if ((statusMask & LevelPiece::IceCubeStatus) == LevelPiece::IceCubeStatus) {
		this->RemoveStatus(level, LevelPiece::IceCubeStatus);
		statusFound = true;
	}
	//... TODO for each other status

	assert(statusFound);
}

void LevelPiece::RemoveAllStatus(GameLevel* level) {

	// If the piece has no status effects then just exit, this saves us the trouble
	// of emitting an event that could potentially be a more expensive operation
	if (this->pieceStatus == LevelPiece::NormalStatus) {
		return;
	}

	bool hadIceCubeStatus = this->HasStatus(LevelPiece::IceCubeStatus);
	this->pieceStatus = LevelPiece::NormalStatus;
	if (hadIceCubeStatus) {
		// We update the bounds since ice cubes change the bounds of a block - this must be done
		// AFTER the change to the status
        if (level != NULL) {
		    level->UpdateBoundsOnPieceAndSurroundingPieces(this);
        }
	}

	// EVENT: All status effects removed from this piece...
	GameEventManager::Instance()->ActionLevelPieceAllStatusRemoved(*this);
}

// Helper function used to light this piece on fire (if it is allowed to catch on fire).
void LevelPiece::LightPieceOnFire(GameModel* gameModel, bool canCatchOnFire) {
	// If this piece is currently in an ice cube then just melt the ice cube but don't
	// make the block catch on fire...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::IceCubeStatus);
        UNUSED_VARIABLE(success);
		assert(success);

        // EVENT: Frozen block cancelled-out by fire
        GameEventManager::Instance()->ActionBlockIceCancelledWithFire(*this);
	}
	else if (canCatchOnFire) {
        this->AddStatus(gameModel->GetCurrentLevel(), LevelPiece::OnFireStatus);
		gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::OnFireStatus);
	}
}

// Helper function used to freeze this piece in an ice block (if it is allowed to be frozen).
void LevelPiece::FreezePieceInIce(GameModel* gameModel, bool canBeFrozen) {
	// If this piece is currently on fire then the ice will cancel with the fire and
	// the piece will no longer be on fire
	if (this->HasStatus(LevelPiece::OnFireStatus)) {
		bool success = gameModel->RemoveStatusForLevelPiece(this, LevelPiece::OnFireStatus);
        UNUSED_VARIABLE(success);
		assert(success);

        // EVENT: On-fire block cancelled-out by ice
        GameEventManager::Instance()->ActionBlockFireCancelledWithIce(*this);
	}
	else if (canBeFrozen) {
		// The ball is icy, encapsulate this piece in an ice cube, this will make the ball take
		// have to hit this piece once more in order to destroy it...
        this->AddStatus(gameModel->GetCurrentLevel(), LevelPiece::IceCubeStatus);
		gameModel->AddStatusUpdateLevelPiece(this, LevelPiece::IceCubeStatus);
	}
}

/**
 * Call this in order to properly set the projectile (and possibly spawn any extra projectiles) when
 * this piece is frozen in ice.
 */
void LevelPiece::DoIceCubeReflectRefractLaserBullets(Projectile* projectile, GameModel* gameModel) const {

	assert(this->HasStatus(LevelPiece::IceCubeStatus));
	assert(projectile != NULL);
	assert(gameModel != NULL);

	// Need to figure out if this laser bullet already collided with this block... 
	// If it has then we just ignore it
	if (projectile->IsLastThingCollidedWith(this)) {
		return;
	}
	
	std::list<Collision::Ray2D> rays;
	this->GetIceCubeReflectionRefractionRays(projectile->GetPosition(), projectile->GetVelocityDirection(), rays);
	assert(rays.size() >= 1);
					
	std::list<Collision::Ray2D>::iterator rayIter = rays.begin();

    // Make refracted rays smaller based on how many there are
    float scaleFactor = Projectile::GetProjectileSplitScaleFactor(rays.size());

    projectile->SetWidth(scaleFactor * projectile->GetWidth());
    projectile->SetHeight(scaleFactor * projectile->GetHeight());

	// The first ray is how the current projectile gets transmitted through this block...
	projectile->SetPosition(rayIter->GetOrigin());
	projectile->SetVelocity(rayIter->GetUnitDirection(), projectile->GetVelocityMagnitude());
	projectile->SetLastThingCollidedWith(this);

	// All the other rays were created via refraction or some such thing, so spawn new particles for them
	++rayIter;
	for (; rayIter != rays.end(); ++rayIter) {
        Projectile* newProjectile = Projectile::CreateProjectileFromCopy(projectile);
		
        newProjectile->SetWidth(scaleFactor * newProjectile->GetWidth());
        newProjectile->SetHeight(scaleFactor * newProjectile->GetHeight());
        newProjectile->SetPosition(rayIter->GetOrigin());
		newProjectile->SetVelocity(rayIter->GetUnitDirection(), projectile->GetVelocityMagnitude());
		newProjectile->SetLastThingCollidedWith(this); // If we don't do this then it will cause recursive doom

		gameModel->AddProjectile(newProjectile);
	}
}

/**
 * Gets the expected reflection/refraction rays for a levelpiece that's been frozen in an ice cube status.
 * The resulting rays are based on the position of the projectile at impact relative to this piece.
 */
void LevelPiece::GetIceCubeReflectionRefractionRays(const Point2D& currCenter, 
                                                    const Vector2D& currDir, 
                                                    std::list<Collision::Ray2D>& rays) const {

	Collision::Ray2D defaultRay(currCenter, currDir);	// This is what happens to the original ray
	
	// Determine how the ray will move through the ice cube based on where it hit...
	const Vector2D delta										= currCenter - this->GetCenter();
	static const float HEIGHT_FUZZINESS     = LevelPiece::PIECE_HEIGHT / 15.0f;
	static const float WIDTH_FUZZINESS      = LevelPiece::PIECE_WIDTH  / 15.0f;
	static const float HALF_PIECE_WIDTH     = LevelPiece::PIECE_WIDTH  / 2.0f;
	static const float HALF_PIECE_HEIGHT    = LevelPiece::PIECE_HEIGHT / 2.0f;

	static const float LEFT_FUZZ_LINE_X   =  -HALF_PIECE_WIDTH  + WIDTH_FUZZINESS;
	static const float RIGHT_FUZZ_LINE_X  =   HALF_PIECE_WIDTH  - WIDTH_FUZZINESS;
	static const float TOP_FUZZ_LINE_Y    =   HALF_PIECE_HEIGHT - HEIGHT_FUZZINESS;
	static const float BOTTOM_FUZZ_LINE_Y =  -HALF_PIECE_HEIGHT + HEIGHT_FUZZINESS;

	// Figure out what face of the ice cube it hit...
	if (delta[0] <= LEFT_FUZZ_LINE_X) {

		if (delta[1] < BOTTOM_FUZZ_LINE_Y) {
			// Collision was in the bottom left corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, 1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (delta[1] > TOP_FUZZ_LINE_Y) {
			// Collision was in the top left corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(-1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, -1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Collision was on the left side
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(-1.0f, 0.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght right 
				defaultRay.SetUnitDirection(Vector2D(1.0f, 0.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}
	else if (delta[0] >= RIGHT_FUZZ_LINE_X) {
		
		if (delta[1] < BOTTOM_FUZZ_LINE_Y) {
			// Collision was in the bottom right corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(1.0f, -1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, 1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(-1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else if (delta[1] > TOP_FUZZ_LINE_Y) {
			// Collision was in the top right corner
			static const Vector2D BOTTOM_LEFT_NORMAL = Vector2D(1.0f, 1.0f) / SQRT_2;
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, BOTTOM_LEFT_NORMAL)))));
			if (angleBetweenNormalAndLaser <= 20) {
				// The ray will split...
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(0.0f, -1.0f)));
				rays.push_back(Collision::Ray2D(this->GetCenter(), Vector2D(-1.0f, 0.0f)));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Collision was on the right side
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(-1.0f, 0.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght right 
				defaultRay.SetUnitDirection(Vector2D(-1.0f, 0.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}
	else {
		// Collision was on either the bottom or top sides
		if (delta[1] >= 0) {
			// Top
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(0.0f, 1.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght down 
				defaultRay.SetUnitDirection(Vector2D(0.0f, -1.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
		else {
			// Bottom
			float angleBetweenNormalAndLaser = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-currDir, Vector2D(0.0f, -1.0f))))));
			if (angleBetweenNormalAndLaser <= 60) {
				// The ray will just go striaght up 
				defaultRay.SetUnitDirection(Vector2D(0.0f, 1.0f));
				defaultRay.SetOrigin(this->GetCenter());
			}
		}
	}

	rays.push_front(defaultRay);
}

void LevelPiece::DoPossibleFireGlobDrop(GameModel* gameModel) const {
    int fireGlobDropRandomNum = Randomizer::GetInstance()->RandomUnsignedInt() % GameModelConstants::GetInstance()->FIRE_GLOB_CHANCE_MOD;
	if (fireGlobDropRandomNum != 0) {
        return;
    }

	float globSize  = 0.4f * LevelPiece::HALF_PIECE_WIDTH + 0.9f * Randomizer::GetInstance()->RandomNumZeroToOne() * LevelPiece::HALF_PIECE_WIDTH;
	float edgeDist  = ((LevelPiece::PIECE_WIDTH - globSize) / 2.0f) - 0.01f;
	assert(edgeDist >= 0.0f);

	// Calculate a place on the block to drop the glob from...
	Point2D dropPos = this->GetCenter() - Vector2D(Randomizer::GetInstance()->RandomNumNegOneToOne() * edgeDist, globSize / 2.0f);

	// Drop a glob of fire downwards from the block...
	Projectile* fireGlobProjectile = new FireGlobProjectile(dropPos, globSize);
	fireGlobProjectile->SetLastThingCollidedWith(this);
	gameModel->AddProjectile(fireGlobProjectile);
}