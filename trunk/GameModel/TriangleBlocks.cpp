/**
 * TriangleBlocks.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TriangleBlocks.h"
#include "GameModel.h"
#include "GameLevel.h"
#include "GameEventManager.h"
#include "PaddleLaserProjectile.h"
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

    // If the triangle block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateBreakableBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
	}
	else {
        this->SetBounds(TriangleBlock::CreateTriangleBounds(false, this->orient, this->center, 
            leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor), 
            leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
	}
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
    this->colour = ColourRGBA(0.55f, 0.55f, 0.55f, 1.0f);
}

SolidTriangleBlock::~SolidTriangleBlock() {
}

bool SolidTriangleBlock::CollisionCheck(const Collision::Ray2D& ray, float& rayT) const {
	return LevelPiece::CollisionCheck(ray, rayT);
}

void SolidTriangleBlock::UpdateBounds(const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                      const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor,
                                      const LevelPiece* topRightNeighbor, const LevelPiece* topLeftNeighbor,
                                      const LevelPiece* bottomRightNeighbor, const LevelPiece* bottomLeftNeighbor) {

	// If the triangle block is in ice then its bounds are a basic rectangle...
	if (this->HasStatus(LevelPiece::IceCubeStatus)) {
		LevelPiece::UpdateSolidRectBlockBounds(this, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor,
            topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
	}
	else {
		this->SetBounds(TriangleBlock::CreateTriangleBounds(
            false, this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor), 
			leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
		 	topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
	}
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

	UNUSED_PARAMETER(topRightNeighbor);
	UNUSED_PARAMETER(topLeftNeighbor);
	UNUSED_PARAMETER(bottomRightNeighbor);
	UNUSED_PARAMETER(bottomLeftNeighbor);

	this->SetBounds(TriangleBlock::CreateTriangleBounds(false, this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor), 
									leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
	 								topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
	this->reflectRefractBounds = TriangleBlock::CreateTriangleBounds(true, this->orient, this->center, leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor);
}

LevelPiece* PrismTriangleBlock::CollisionOccurred(GameModel* gameModel, Projectile* projectile) {
	LevelPiece* resultingPiece = this;

	switch (projectile->GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
		case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile: {
				// Based on where the laser bullet hits, we change its direction
				
				// Need to figure out if this laser bullet already collided with this block... if it has then we just ignore it
				if (!projectile->IsLastThingCollidedWith(this)) {

					// Obtain all the normals of the lines that the projectile is colliding with...
					std::vector<int> collidingIndices = 
                        this->reflectRefractBounds.CollisionCheckIndices(Collision::Ray2D(projectile->GetPosition(), projectile->GetVelocityDirection()));
					Vector2D collisionNormal;
                    bool doReflectionRefractionCollisions = true;

					if (collidingIndices.size() == 2) {
						// Special case - due to the speed of the collision, 2 lines were found to be
						// colliding with the projectile, we need to reconcile this...

						// Just figure out which normal is closest to the negative velocity of the projectile and use it
						Vector2D normal0 = this->reflectRefractBounds.GetNormal(collidingIndices[0]);
						Vector2D normal1 = this->reflectRefractBounds.GetNormal(collidingIndices[1]);

						float angleBetween0 = Trig::radiansToDegrees(
                            acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-projectile->GetVelocityDirection(), normal0)))));
						float angleBetween1 = Trig::radiansToDegrees(
                            acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-projectile->GetVelocityDirection(), normal1)))));

						if (angleBetween0 < angleBetween1) {
							collisionNormal = this->reflectRefractBounds.GetNormal(collidingIndices[0]);
						}
						else {
							collisionNormal = this->reflectRefractBounds.GetNormal(collidingIndices[1]);
						}
					}
					else if (collidingIndices.size() == 1) {
						// Just one collision line, use it
						collisionNormal = this->reflectRefractBounds.GetNormal(collidingIndices[0]);
					}
                    else {
                        // More than two colliding lines or no colliding lines...
                        // This shouldn't happen but it has happened before... probably a numerical error thing,
                        // to deal with this we just let the laser keep going through the prism
                        doReflectionRefractionCollisions = false;
                    }

                    if (doReflectionRefractionCollisions) {
					    // Send the laser flying out the hypotenuse side if it hit within a certain angle
					    const float angleOfIncidence = Trig::radiansToDegrees(acos(std::min<float>(1.0f, 
                            std::max<float>(-1.0f, Vector2D::Dot(-projectile->GetVelocityDirection(), collisionNormal)))));
					    static const float MIN_CUTOFF_ANGLE = 15.0f;
                        static const float MAX_CUTOFF_ANGLE = 90.0f;

                        // Laser hit at a really wide angle... strange and should not really happen, just let the laser carry on
                        if (angleOfIncidence <= MAX_CUTOFF_ANGLE) {

					        // Check to see if the collision was within the cut-off angle, if the laser
					        // almost hits the prism on the normal it will be refracted and/or split
					        // if it hits beyond the cut-off angle it will just be reflected
					        if (angleOfIncidence <= MIN_CUTOFF_ANGLE) {

						        if (fabs(collisionNormal[1]) < EPSILON || fabs(collisionNormal[0]) < EPSILON) {
							        // Dealing with either the x-axis short-side or the y-axis long-side of the triangle prism:
							        // Refract the laser out the hypotenuse side of the triangle
							        projectile->SetPosition(this->GetCenter());
							        Vector2D hypNormal = TriangleBlock::GetSideNormal(true, TriangleBlock::HypotenuseSide, this->orient);
							        projectile->SetVelocity(hypNormal, projectile->GetVelocityMagnitude());
						        }
						        else {
							        // Hypotenuse collision:
							        // Split the laser in two and send it out both the long and short side of the triangle
							        Vector2D shortSideNormal = TriangleBlock::GetSideNormal(true, TriangleBlock::ShortSide, this->orient);
							        Vector2D longSideNormal  = TriangleBlock::GetSideNormal(true, TriangleBlock::LongSide, this->orient);

							        // Send the current projectile out the long side, spawn a new one for the short side
                                    float scaleFactor = Projectile::GetProjectileSplitScaleFactor(2);
                                    Projectile* newProjectile  = Projectile::CreateProjectileFromCopy(projectile, true); 
                                    
                                    newProjectile->SetWidth(scaleFactor * newProjectile->GetWidth());
                                    newProjectile->SetHeight(scaleFactor * newProjectile->GetHeight());
                                    newProjectile->SetPosition(Point2D(projectile->GetPosition()[0], this->GetCenter()[1])
                                                               + projectile->GetHalfHeight() * shortSideNormal);
							        newProjectile->SetVelocity(shortSideNormal, projectile->GetVelocityMagnitude());
							        newProjectile->SetLastThingCollidedWith(this);
							        gameModel->AddProjectile(newProjectile);


                                    projectile->SetWidth(scaleFactor * projectile->GetWidth());
                                    projectile->SetHeight(scaleFactor * projectile->GetHeight());
							        projectile->SetPosition(Point2D(this->GetCenter()[0], projectile->GetPosition()[1])
                                                            + projectile->GetHalfHeight() * shortSideNormal);
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
                        }

					    projectile->SetLastThingCollidedWith(this);
				    }
                }
			}
			break;

        case Projectile::BossLightningBoltBulletProjectile:
            break;

		case Projectile::CollateralBlockProjectile:
            resultingPiece = this->Destroy(gameModel, LevelPiece::CollateralDestruction);
			break;

		case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
            assert(dynamic_cast<RocketProjectile*>(projectile) != NULL);
		    resultingPiece = gameModel->GetCurrentLevel()->RocketExplosion(gameModel, static_cast<RocketProjectile*>(projectile), this);
			break;

        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            // A mine will just come to rest on the block.
            break;

		case Projectile::FireGlobProjectile:
        case Projectile::PaddleFlameBlastProjectile:
        case Projectile::PaddleIceBlastProjectile:
			// Extinguish, projectile has no effect
			break;

		default:
			assert(false);
			break;

	}

	return this;
}

/**
 * Get the resulting reflection/refraction rays for the given hit point
 * on this prism triangle block with the given impact velocity direction.
 */
void PrismTriangleBlock::GetReflectionRefractionRays(const Point2D& hitPoint,
                                                     const Vector2D& impactDir,
                                                     std::list<Collision::Ray2D>& rays) const {

	Collision::Ray2D defaultRay(hitPoint, impactDir);	// This is what happens to the original ray
	Vector2D negImpactDir = -impactDir;

	// Obtain all the normals of the lines that the hit point is closest to
	std::vector<int> collidingIndices = this->reflectRefractBounds.ClosestCollisionIndices(hitPoint, 0.01f);
	Vector2D collisionNormal;

	if (collidingIndices.size() >= 2) {
		// Special case - 2 or more lines were found to be colliding with the projectile, we need to reconcile this...
		// Just figure out which normal is closest to the negative velocity of the projectile and use it
		float smallestAngle = FLT_MAX;
		for (std::vector<int>::iterator iter = collidingIndices.begin(); iter != collidingIndices.end(); ++iter) {
			Vector2D currentNormal = this->reflectRefractBounds.GetNormal(*iter);
			float currentAngle     = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(negImpactDir, currentNormal)))));
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
		collisionNormal = this->reflectRefractBounds.GetNormal(collidingIndices[0]);
	}

	// Send the laser flying out the hypotenuse side if it hit within a certain angle
	const float angleOfIncidence = Trig::radiansToDegrees(acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(negImpactDir, collisionNormal)))));
	static const float MIN_CUTOFF_ANGLE = 15.0f;
    static const float MAX_CUTOFF_ANGLE = 90.0f;

    // Laser hit at a really wide angle... strange and should not really happen, just let the laser carry on
    if (angleOfIncidence <= MAX_CUTOFF_ANGLE) {
	    // Check to see if the collision was within the cut-off angle, if the laser
	    // almost hits the prism on the normal it will be refracted and/or split
	    // if it hits beyond the cut-off angle it will just be reflected
	    if (angleOfIncidence <= MIN_CUTOFF_ANGLE) {

		    if (fabs(collisionNormal[1]) < EPSILON || fabs(collisionNormal[0]) < EPSILON) {
			    // Dealing with either the x-axis short-side or the y-axis long-side of the triangle prism:
			    // Refract the laser out the hypotenuse side of the triangle
			    Vector2D hypNormal = TriangleBlock::GetSideNormal(true, TriangleBlock::HypotenuseSide, this->orient);
			    //defaultRay.SetOrigin(this->GetCenter());
			    defaultRay.SetUnitDirection(hypNormal);
		    }
		    else {
			    // Hypotenuse collision:
			    // Split the laser in two and send it out both the long and short side of the triangle
			    Vector2D shortSideNormal = TriangleBlock::GetSideNormal(true, TriangleBlock::ShortSide, this->orient);
			    Vector2D longSideNormal  = TriangleBlock::GetSideNormal(true, TriangleBlock::LongSide, this->orient);

			    defaultRay.SetUnitDirection(longSideNormal);
			    rays.push_back(Collision::Ray2D(hitPoint, shortSideNormal));
		    }
	    }
	    else {
		    // Laser hit beyond the cut-off angle: reflect the laser
		    Vector2D newVelDir = Reflect(impactDir, collisionNormal);
		    newVelDir.Normalize();
		    defaultRay.SetUnitDirection(newVelDir);
	    }
    }

	rays.push_front(defaultRay);
}

// Triangle Block Namespace Functions ----------------------------------------------------------------

/**
 * Static class for sharing the creation of triangle boundaries used by the triangle block classes.
 * Returns: bounding lines for the given triangle.
 */
BoundingLines TriangleBlock::CreateTriangleBounds(bool generateReflectRefractNormals, Orientation triOrient, const Point2D& center, 
                                                  const LevelPiece* leftNeighbor, const LevelPiece* bottomNeighbor,
                                                  const LevelPiece* rightNeighbor, const LevelPiece* topNeighbor) {
    // Set the bounding lines for a rectangular block
    std::vector<Collision::LineSeg2D> boundingLines;
    std::vector<Vector2D>  boundingNorms;
    std::vector<bool> onInside;

    Vector2D longSideNorm, shortSideNorm, hypSideNorm;
    Collision::LineSeg2D shortSide, longSide, hypSide;

    bool topNeighborNotSolid = topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        (topNeighbor->GetType() != LevelPiece::Solid && topNeighbor->GetType() != LevelPiece::Breakable &&
         topNeighbor->GetType() != LevelPiece::AlwaysDrop && topNeighbor->GetType() != LevelPiece::Regen);
    bool bottomNeighborNotSolid	= bottomNeighbor == NULL  || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus | LevelPiece::OnFireStatus) ||
        (bottomNeighbor->GetType() != LevelPiece::Solid && bottomNeighbor->GetType() != LevelPiece::Breakable &&
         bottomNeighbor->GetType() != LevelPiece::AlwaysDrop && bottomNeighbor->GetType() != LevelPiece::Regen);
    bool leftNeighborNotSolid = leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) || 
        (leftNeighbor->GetType() != LevelPiece::Solid && leftNeighbor->GetType() != LevelPiece::Breakable &&
         leftNeighbor->GetType() != LevelPiece::AlwaysDrop && leftNeighbor->GetType() != LevelPiece::Regen);
    bool rightNeighborNotSolid = rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
        (rightNeighbor->GetType() != LevelPiece::Solid && rightNeighbor->GetType() != LevelPiece::Breakable &&
         rightNeighbor->GetType() != LevelPiece::AlwaysDrop && rightNeighbor->GetType() != LevelPiece::Regen);

    // Triangle neighbor cases...
    if (topNeighbor != NULL && TriangleBlock::IsTriangleType(*topNeighbor)) {
        TriangleBlock::Orientation orientation = dynamic_cast<const TriangleBlockInterface*>(topNeighbor)->GetOrientation();
        topNeighborNotSolid &= (orientation != TriangleBlock::LowerLeft && orientation != TriangleBlock::LowerRight);
    }
    if (bottomNeighbor != NULL && TriangleBlock::IsTriangleType(*bottomNeighbor)) {
        TriangleBlock::Orientation orientation = dynamic_cast<const TriangleBlockInterface*>(bottomNeighbor)->GetOrientation();
        bottomNeighborNotSolid &= (orientation != TriangleBlock::UpperLeft && orientation != TriangleBlock::UpperRight);
    }
    if (leftNeighbor != NULL && TriangleBlock::IsTriangleType(*leftNeighbor)) {
        TriangleBlock::Orientation orientation = dynamic_cast<const TriangleBlockInterface*>(leftNeighbor)->GetOrientation();
        leftNeighborNotSolid &= (orientation != TriangleBlock::UpperRight && orientation != TriangleBlock::LowerRight);
    }
    if (rightNeighbor != NULL && TriangleBlock::IsTriangleType(*rightNeighbor)) {
        TriangleBlock::Orientation orientation = dynamic_cast<const TriangleBlockInterface*>(rightNeighbor)->GetOrientation();
        rightNeighborNotSolid &= (orientation != TriangleBlock::UpperLeft && orientation != TriangleBlock::LowerLeft);
    }

    switch (triOrient) {
        case TriangleBlock::UpperLeft:
            // Long side is the top, short side is left and hypotenuse goes from lower left to upper right
            longSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::LongSide, TriangleBlock::UpperLeft);
            shortSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::ShortSide, TriangleBlock::UpperLeft);

            longSide  = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));
            shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));

            if (bottomNeighborNotSolid || rightNeighborNotSolid) {
                hypSideNorm = GetSideNormal(generateReflectRefractNormals, TriangleBlock::HypotenuseSide, TriangleBlock::UpperLeft);
                hypSide     = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                              center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));

                boundingLines.push_back(hypSide);
                boundingNorms.push_back(hypSideNorm);
                onInside.push_back(!bottomNeighborNotSolid || !rightNeighborNotSolid);
            }
            break;

        case TriangleBlock::UpperRight:
            // Long side is the top, short side is the right and hypotenuse goes from lower right to upper left
            longSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::LongSide, TriangleBlock::UpperRight);
            shortSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::ShortSide, TriangleBlock::UpperRight);

            longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT),
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));		
            shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));			

            if (bottomNeighborNotSolid || leftNeighborNotSolid) {
                hypSideNorm	 = GetSideNormal(generateReflectRefractNormals, TriangleBlock::HypotenuseSide, TriangleBlock::UpperRight);
                hypSide      = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));

                boundingLines.push_back(hypSide);
                boundingNorms.push_back(hypSideNorm);	
                onInside.push_back(!bottomNeighborNotSolid || !leftNeighborNotSolid);
            }
            break;

        case TriangleBlock::LowerLeft:
            // Long side is at the bottom, short side is on the left and hypotenuse goes from lower-right to upper-left
            longSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::LongSide, TriangleBlock::LowerLeft);
            shortSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::ShortSide, TriangleBlock::LowerLeft);

            longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
            shortSide = Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));

            if (topNeighborNotSolid || rightNeighborNotSolid) {
                hypSideNorm		= GetSideNormal(generateReflectRefractNormals, TriangleBlock::HypotenuseSide, TriangleBlock::LowerLeft);
                hypSide				= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));

                boundingLines.push_back(hypSide);
                boundingNorms.push_back(hypSideNorm);
                onInside.push_back(!topNeighborNotSolid || !rightNeighborNotSolid);
            }
            break;

        case TriangleBlock::LowerRight:
            // Long side is at the bottom, short side is on the right and hypotenuse goes from lower-left to upper-right
            longSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::LongSide, TriangleBlock::LowerRight);
            shortSideNorm	= GetSideNormal(generateReflectRefractNormals, TriangleBlock::ShortSide, TriangleBlock::LowerRight);

            longSide	= Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));		
            shortSide = Collision::LineSeg2D(center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT), 
                center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));

            if (topNeighborNotSolid || leftNeighborNotSolid) {
                hypSideNorm		= GetSideNormal(generateReflectRefractNormals, TriangleBlock::HypotenuseSide, TriangleBlock::LowerRight);
                hypSide				= Collision::LineSeg2D(center + Vector2D(-LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
                    center + Vector2D(LevelPiece::HALF_PIECE_WIDTH, LevelPiece::HALF_PIECE_HEIGHT));

                boundingLines.push_back(hypSide);
                boundingNorms.push_back(hypSideNorm);
                onInside.push_back(!topNeighborNotSolid || !leftNeighborNotSolid);
            }
            break;

        default:
            assert(false);
            break;
    }

	// We only create boundaries for breakables in cases where neighbors exist AND they are empty 
	// (i.e., the ball can actually get to them).
    if (topNeighborNotSolid && (triOrient == TriangleBlock::UpperRight || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
        onInside.push_back(topNeighbor == NULL || topNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            topNeighbor->GetType() == LevelPiece::OneWay);
	}
	else if (bottomNeighborNotSolid && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::LowerLeft)) {
		boundingLines.push_back(longSide);
		boundingNorms.push_back(longSideNorm);
        onInside.push_back(bottomNeighbor == NULL || bottomNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            bottomNeighbor->GetType() == LevelPiece::OneWay);
	}

    if (leftNeighborNotSolid && (triOrient == TriangleBlock::LowerLeft || triOrient == TriangleBlock::UpperLeft)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);	
        onInside.push_back(leftNeighbor == NULL || leftNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            leftNeighbor->GetType() == LevelPiece::OneWay);
	}
    else if (rightNeighborNotSolid && (triOrient == TriangleBlock::LowerRight || triOrient == TriangleBlock::UpperRight)) {
		boundingLines.push_back(shortSide);
		boundingNorms.push_back(shortSideNorm);	
        onInside.push_back(rightNeighbor == NULL || rightNeighbor->HasStatus(LevelPiece::IceCubeStatus) ||
            rightNeighbor->GetType() == LevelPiece::OneWay);
	}

	return BoundingLines(boundingLines, boundingNorms, onInside);
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
Vector2D TriangleBlock::GetSideNormal(bool generateReflectRefractNormals, SideType side, Orientation orient) {
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
			{
				if (generateReflectRefractNormals) {
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
				}
				else {
					float xMult = (LevelPiece::PIECE_HEIGHT * LevelPiece::PIECE_HEIGHT) / LevelPiece::PIECE_WIDTH;
					switch (orient) {
						case TriangleBlock::UpperLeft: 
							{
								Vector2D normal = xMult * Vector2D(1, 0) + LevelPiece::PIECE_HEIGHT * Vector2D(0, -1);
								normal.Normalize();
								return normal;
							}
						case TriangleBlock::UpperRight: 
							{
								Vector2D normal = xMult * Vector2D(-1, 0) + LevelPiece::PIECE_HEIGHT * Vector2D(0, -1);
								normal.Normalize();
								return normal;
							}
						case TriangleBlock::LowerLeft:
							{
								Vector2D normal = xMult * Vector2D(1, 0) + LevelPiece::PIECE_HEIGHT * Vector2D(0, 1);
								normal.Normalize();
								return normal;
							}
						case TriangleBlock::LowerRight:
							{
								Vector2D normal = xMult * Vector2D(-1, 0) + LevelPiece::PIECE_HEIGHT * Vector2D(0, 1);
								normal.Normalize();
								return normal;
							}
						default:
							assert(false);
							break;
					}
				}

				break;
			}

		default:
			assert(false);
			break;
	}

	return Vector2D(0.0f, 0.0f);
}

bool TriangleBlock::GetOrientation(const LevelPiece* piece, Orientation& orient) {
    switch (piece->GetType()) {
        case LevelPiece::PrismTriangle: {
            const PrismTriangleBlock* temp = static_cast<const PrismTriangleBlock*>(piece);
            orient = temp->GetOrientation();
            break;
        }
            
        case LevelPiece::SolidTriangle: {
            const SolidTriangleBlock* temp = static_cast<const SolidTriangleBlock*>(piece);
            orient = temp->GetOrientation();
            break;
        }

        case LevelPiece::BreakableTriangle: {
            const BreakableTriangleBlock* temp = static_cast<const BreakableTriangleBlock*>(piece);
            orient = temp->GetOrientation();
            break;
        }

        default:
            return false;
    }

    return true;
}