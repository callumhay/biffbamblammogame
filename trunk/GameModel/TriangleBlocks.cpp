/**
 * TriangleBlocks.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "TriangleBlocks.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"

#include "EmptySpaceBlock.h"

// Breakable Triangle Block Class Functions ---------------------------------------------------------

BreakableTriangleBlock::BreakableTriangleBlock(char type, TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) : 
BreakableBlock(type, wLoc, hLoc), orient(orientation) {
}

BreakableTriangleBlock::~BreakableTriangleBlock() {
}

void BreakableTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																					const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																					const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																					const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

Matrix4x4 BreakableTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 BreakableTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

// Solid Triangle Block Class Functions ---------------------------------------------------------------
SolidTriangleBlock::SolidTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) :
SolidBlock(wLoc, hLoc), orient(orientation) {
}

SolidTriangleBlock::~SolidTriangleBlock() {
}

void SolidTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																			const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																			const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																			const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

Matrix4x4 SolidTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 SolidTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

PrismTriangleBlock::PrismTriangleBlock(TriangleBlock::Orientation orientation, unsigned int wLoc, unsigned int hLoc) :
PrismBlock(wLoc, hLoc), orient(orientation) {
}

PrismTriangleBlock::~PrismTriangleBlock() {
}

Matrix4x4 PrismTriangleBlock::GetPieceToLevelTransform() const {
	Matrix4x4 translation =  Matrix4x4::translationMatrix(Vector3D(this->center[0], this->center[1], 0.0f));
	Matrix4x4 orient = TriangleBlock::GetOrientationMatrix(this->orient);
	return translation * orient;
}

Matrix4x4 PrismTriangleBlock::GetPieceToLevelInvTransform() const {
	Matrix4x4 invTranslation =  Matrix4x4::translationMatrix(Vector3D(-this->center[0], -this->center[1], 0.0f));
	Matrix4x4 invOrient = TriangleBlock::GetInvOrientationMatrix(this->orient);
	return invOrient * invTranslation;
}

void PrismTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																			const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
																			const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
																			const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {
	this->bounds = TriangleBlock::CreateTriangleBounds(this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

LevelPiece* PrismTriangleBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	if (projectile->GetType() == Projectile::PaddleLaserBulletProjectile) {
	// Based on where the laser bullet hits, we change its direction
		
		// Need to figure out if this laser bullet already collided with this block... if it has then we just ignore it
		if (!projectile->IsLastLevelPieceCollidedWith(this)) {

			// Obtain all the normals of the lines that the projectile is colliding with...
			std::vector<int> collidingIndices = this->bounds.CollisionCheckIndices(projectile->BuildBoundingLines());
			Vector2D collisionNormal;

			if (collidingIndices.size() == 2) {
				// Special case - due to the speed of the collision, 2 lines were found to be
				// colliding with the projectile, we need to reconcile this...

				// Just figure out which normal is closest to the negative velocity of the projectile and use it
				Vector2D normal0 = this->bounds.GetNormal(collidingIndices[0]);
				Vector2D normal1 = this->bounds.GetNormal(collidingIndices[1]);

				float angleBetween0 = Trig::radiansToDegrees(acos(Vector2D::Dot(-projectile->GetVelocityDirection(), normal0)));
				float angleBetween1 = Trig::radiansToDegrees(acos(Vector2D::Dot(-projectile->GetVelocityDirection(), normal1)));

				if (angleBetween0 < angleBetween1) {
					collisionNormal = this->bounds.GetNormal(collidingIndices[0]);
				}
				else {
					collisionNormal = this->bounds.GetNormal(collidingIndices[1]);
				}

			}
			else if (collidingIndices.size() > 2 || collidingIndices.size() == 0) {
				// This should never happen...
				assert(false);
			}
			else {
				// Just one collision line, use it
				collisionNormal = this->bounds.GetNormal(collidingIndices[0]);
			}

			// Send the laser flying out the hypotenuse side if it hit within a certain angle
			const float ANGLE_OF_INCIDENCE = Trig::radiansToDegrees(acos(Vector2D::Dot(-projectile->GetVelocityDirection(), collisionNormal)));
			const float CUTOFF_ANGLE			 = 15.0f;

			// Check to see if the collision was within the cut-off angle, if the laser
			// almost hits the prism on the normal it will be refracted and/or split
			// if it hits beyond the cut-off angle it will just be reflected
			if (ANGLE_OF_INCIDENCE <= CUTOFF_ANGLE) {

				if (fabs(collisionNormal[1]) < EPSILON || fabs(collisionNormal[0]) < EPSILON) {
					// Dealing with either the x-axis short-side or the y-axis long-side of the triangle prism:
					// Refract the laser out the hypotenuse side of the triangle
					projectile->SetPosition(this->GetCenter());
					Vector2D hypNormal = TriangleBlock::GetSideNormal(TriangleBlock::HypotenuseSide, this->orient);
					projectile->SetVelocity(hypNormal, projectile->GetVelocityMagnitude());
				}
				else {
					// Hypotenuse collision:
					// Split the laser in two and send it out both the long and short side of the triangle
					Vector2D shortSideNormal = TriangleBlock::GetSideNormal(TriangleBlock::ShortSide, this->orient);
					Vector2D longSideNormal  = TriangleBlock::GetSideNormal(TriangleBlock::LongSide, this->orient);
					Point2D splitPosition    = projectile->GetPosition() + projectile->GetHalfHeight() * projectile->GetVelocityDirection();

					// Send the current projectile out the long side, spawn a new one for the short side
					PaddleLaser* newProjectile  = new PaddleLaser(*projectile); 
					newProjectile->SetPosition(splitPosition);
					newProjectile->SetVelocity(shortSideNormal, projectile->GetVelocityMagnitude());
					newProjectile->SetLastLevelPieceCollidedWith(this);
					gameModel->AddProjectile(newProjectile);

					projectile->SetPosition(splitPosition);
					projectile->SetVelocity(longSideNormal, projectile->GetVelocityMagnitude());
				}
			}
			else {
				// Laser hit beyond the cut-off angle: reflect the laser
				Vector2D newVelDir = Reflect(projectile->GetVelocityDirection(), collisionNormal);
				newVelDir.Normalize();
				
				// Move the projectile to where it collided and change its velocity to reflect
				projectile->SetPosition(projectile->GetPosition() + projectile->GetHalfHeight() * projectile->GetVelocityDirection());
				projectile->SetVelocity(newVelDir, projectile->GetVelocityMagnitude());
			}

			projectile->SetLastLevelPieceCollidedWith(this);
		}
	}

	return this;
}

/**
 * Get the resulting reflection/refraction rays for the given hit point
 * on this prism triangle block with the given impact velocity direction.
 */
std::list<Collision::Ray2D> PrismTriangleBlock::GetReflectionRefractionRays(const Point2D& hitPoint, const Vector2D& impactDir) const {
	std::list<Collision::Ray2D> resultRays;
	Collision::Ray2D defaultRay(hitPoint, impactDir);	// This is what happens to the original ray
	Vector2D negImpactDir = -impactDir;

	// Obtain all the normals of the lines that the hit point is closest to
	std::vector<int> collidingIndices = this->bounds.ClosestCollisionIndices(hitPoint, 0.01f);
	Vector2D collisionNormal;

	if (collidingIndices.size() >= 2) {
		// Special case - 2 or more lines were found to be colliding with the projectile, we need to reconcile this...
		// Just figure out which normal is closest to the negative velocity of the projectile and use it
		float smallestAngle = FLT_MAX;
		for (std::vector<int>::iterator iter = collidingIndices.begin(); iter != collidingIndices.end(); ++iter) {
			Vector2D currentNormal = this->bounds.GetNormal(*iter);
			float currentAngle     = Trig::radiansToDegrees(acos(Vector2D::Dot(negImpactDir, currentNormal)));
			if (currentAngle < smallestAngle) {
				smallestAngle   = currentAngle;
				collisionNormal = currentNormal; 
			}
		}
	}
	else if (collidingIndices.size() == 0) {
		// This should never happen...
		assert(false);
		collisionNormal = negImpactDir;
	}
	else {
		// Just one collision line, use it
		collisionNormal = this->bounds.GetNormal(collidingIndices[0]);
	}

	// Send the laser flying out the hypotenuse side if it hit within a certain angle
	const float ANGLE_OF_INCIDENCE = Trig::radiansToDegrees(acos(Vector2D::Dot(negImpactDir, collisionNormal)));
	const float CUTOFF_ANGLE			 = 15.0f;

	// Check to see if the collision was within the cut-off angle, if the laser
	// almost hits the prism on the normal it will be refracted and/or split
	// if it hits beyond the cut-off angle it will just be reflected
	if (ANGLE_OF_INCIDENCE <= CUTOFF_ANGLE) {

		if (fabs(collisionNormal[1]) < EPSILON || fabs(collisionNormal[0]) < EPSILON) {
			// Dealing with either the x-axis short-side or the y-axis long-side of the triangle prism:
			// Refract the laser out the hypotenuse side of the triangle
			Vector2D hypNormal = TriangleBlock::GetSideNormal(TriangleBlock::HypotenuseSide, this->orient);
			//defaultRay.SetOrigin(this->GetCenter());
			defaultRay.SetUnitDirection(hypNormal);
		}
		else {
			// Hypotenuse collision:
			// Split the laser in two and send it out both the long and short side of the triangle
			Vector2D shortSideNormal = TriangleBlock::GetSideNormal(TriangleBlock::ShortSide, this->orient);
			Vector2D longSideNormal  = TriangleBlock::GetSideNormal(TriangleBlock::LongSide, this->orient);

			defaultRay.SetUnitDirection(longSideNormal);
			resultRays.push_back(Collision::Ray2D(hitPoint, shortSideNormal));
		}
	}
	else {
		// Laser hit beyond the cut-off angle: reflect the laser
		Vector2D newVelDir = Reflect(impactDir, collisionNormal);
		newVelDir.Normalize();
		defaultRay.SetUnitDirection(newVelDir);
	}
	resultRays.push_front(defaultRay);
	return resultRays;
}

// Triangle Block Namespace Functions ----------------------------------------------------------------

/**
 * Static class for sharing the creation of triangle boundries used by the triangle block classes.
 * Returns: bounding lines for the given triangle.
 */
BoundingLines TriangleBlock::CreateTriangleBounds(Orientation triOrient, const Point2D& center, 
																									const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
																									const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor) {
	// Set the bounding lines for a rectangular block
	std::vector<Collision::LineSeg2D> boundingLines;
	std::vector<Vector2D>  boundingNorms;

	Vector2D longSideNorm, shortSideNorm, hypSideNorm;
	Collision::LineSeg2D shortSide, longSide, hypSide;

	bool topNeighborEmpty			= topNeighbor != NULL && topNeighbor->IsNoBoundsPieceType();
	bool bottomNeighborEmpty	= bottomNeighbor != NULL && bottomNeighbor->IsNoBoundsPieceType();
	bool leftNeighborEmpty		= leftNeighbor != NULL && leftNeighbor->IsNoBoundsPieceType();
	bool rightNeighborEmpty		= rightNeighbor != NULL && rightNeighbor->IsNoBoundsPieceType();

	switch (triOrient) {
		case TriangleBlock::UpperLeft:
			// Long side is the top, short side is left and hypotenuse goes from lower left to upper right
			longSideNorm	= Vector2D(0, 1);
			shortSideNorm	= Vector2D(-1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
			shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (bottomNeighborEmpty || rightNeighborEmpty) {
				hypSideNorm		= Vector2D(1, -1) / SQRT_2;
				hypSide		= Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);
			}
			break;

		case TriangleBlock::UpperRight:
			// Long side is the top, short side is the right and hypotenuse goes from lower right to upper left
			longSideNorm	= Vector2D(0, 1);
			shortSideNorm	= Vector2D(1, 0);

			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
										center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));			
			
			if (bottomNeighborEmpty || leftNeighborEmpty) {
				hypSideNorm		= Vector2D(-1, -1) / SQRT_2;
				hypSide		= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);				
			}
			break;

		case TriangleBlock::LowerLeft:
			// Long side is at the bottom, short side is on the left and hypotenuse goes from lower-right to upper-left
			longSideNorm	= Vector2D(0, -1);
			shortSideNorm	= Vector2D(-1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (topNeighborEmpty || rightNeighborEmpty) {
				hypSideNorm		= Vector2D(1, 1) / SQRT_2;
				hypSide		= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);
			}
			break;
		case TriangleBlock::LowerRight:
			// Long side is at the bottom, short side is on the right and hypotenuse goes from lower-left to upper-right
			longSideNorm	= Vector2D(0, -1);
			shortSideNorm	= Vector2D(1, 0);
			
			longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
										center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
			shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
									 center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
			
			if (topNeighborEmpty || leftNeighborEmpty) {
				hypSideNorm		= Vector2D(-1, 1) / SQRT_2;
				hypSide		= Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
											center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
				boundingLines.push_back(hypSide);
				boundingNorms.push_back(hypSideNorm);			
			}
			break;

		default:
			assert(false);
			break;
	}

	// We only create boundries for breakables in cases where neighbours exist AND they are empty 
	// (i.e., the ball can actually get to them).
	if (topNeighborEmpty && (triOrient == TriangleBlock::UpperRight || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
	}
	else if (bottomNeighborEmpty && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::LowerLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
	}

	if (leftNeighborEmpty && (triOrient == TriangleBlock::LowerLeft || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);		
	}
	else if (rightNeighborEmpty && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::UpperRight)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);		
	}

	return BoundingLines(boundingLines, boundingNorms);
}

Matrix4x4 TriangleBlock::GetOrientationMatrix(TriangleBlock::Orientation orient) {
	Matrix4x4 orientMat;
	switch (orient) {
		case TriangleBlock::UpperLeft:
			// Reflection in y-axis
			orientMat = Matrix4x4::reflectionMatrix('y');
			break;
		case TriangleBlock::UpperRight:
			// Identity matrix.
			break;
		case TriangleBlock::LowerLeft:
			// 180 degree rotation
			orientMat = Matrix4x4::rotationMatrix('z', 180, true);
			break;
		case TriangleBlock::LowerRight:
			orientMat = Matrix4x4::reflectionMatrix('x');
			break;
		default:
			assert(false);
			break;
	}

	return orientMat;
}

Matrix4x4 TriangleBlock::GetInvOrientationMatrix(TriangleBlock::Orientation orient) {
	Matrix4x4 invOrientMat;
	switch (orient) {
		case TriangleBlock::UpperLeft:
			// reflection in y-axis
			invOrientMat = Matrix4x4::reflectionMatrix('y');
			break;
		case TriangleBlock::UpperRight:
			// Identity matrix.
			break;
		case TriangleBlock::LowerLeft:
			// 180 degree rotation
			invOrientMat = Matrix4x4::rotationMatrix('z', -180, true);
			break;
		case TriangleBlock::LowerRight:
			// reflection in x-axis
			invOrientMat = Matrix4x4::reflectionMatrix('x');
			break;
		default:
			assert(false);
			break;
	}

	return invOrientMat;
}

/**
 * Obtain the normal for the given side for the given orientation of triangle block.
 * Returns: The normal corresponding to the givens.
 */
Vector2D TriangleBlock::GetSideNormal(SideType side, Orientation orient) {
	switch (side) {

		case TriangleBlock::ShortSide:
			switch (orient) {
				case TriangleBlock::UpperLeft:
				case TriangleBlock::LowerLeft:
					return Vector2D(-1.0f, 0.0f); 
				default:
					return Vector2D(1.0f, 0.0f);
			}
			break;

		case TriangleBlock::LongSide:
			switch (orient) {
				case TriangleBlock::UpperLeft:
				case TriangleBlock::UpperRight:
					return Vector2D(0.0f, 1.0f);
				default:
					return Vector2D(0.0f, -1.0f); 
			}
			break;

		case TriangleBlock::HypotenuseSide:
			switch (orient) {
				case TriangleBlock::UpperLeft:
					return Vector2D(1, -1) / SQRT_2;
				case TriangleBlock::UpperRight:
					return Vector2D(-1, -1) / SQRT_2;
				case TriangleBlock::LowerLeft:
					return Vector2D(1, 1) / SQRT_2;
				case TriangleBlock::LowerRight:
					return Vector2D(-1, 1) / SQRT_2;
				default:
					assert(false);
					break;
			}
			break;

		default:
			assert(false);
			break;
	}

	return Vector2D(0.0f, 0.0f);
}