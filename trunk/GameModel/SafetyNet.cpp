/**
 * SafetyNet.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "SafetyNet.h"
#include "GameLevel.h"
#include "PaddleMineProjectile.h"

const float SafetyNet::SAFETY_NET_HEIGHT      = 1.0f;
const float SafetyNet::SAFETY_NET_HALF_HEIGHT = SafetyNet::SAFETY_NET_HEIGHT / 2.0f;

SafetyNet::SafetyNet(const GameLevel& currLevel) {

    const LevelPiece* maxBoundPiece = currLevel.GetMaxPaddleBoundPiece();
    const LevelPiece* minBoundPiece = currLevel.GetMinPaddleBoundPiece();

	// Create the safety net bounding line for this level
	std::vector<Collision::LineSeg2D> lines;
	lines.reserve(1);
    Collision::LineSeg2D safetyNetLine(
        Point2D(minBoundPiece->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT),
        Point2D(maxBoundPiece->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH, -LevelPiece::HALF_PIECE_HEIGHT));
	lines.push_back(safetyNetLine);

	std::vector<Vector2D> normals;
	normals.reserve(1);
	normals.push_back(Vector2D(0, 1));
	
	this->bounds = BoundingLines(lines, normals);
}

SafetyNet::~SafetyNet() {
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
