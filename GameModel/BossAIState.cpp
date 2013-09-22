/**
 * BossAIState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "BossAIState.h"
#include "Boss.h"
#include "GameModel.h"

const float BossAIState::DEFAULT_TARGET_ERROR = 0.5f * LevelPiece::PIECE_HEIGHT;

BossAIState::BossAIState() : currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f), 
startPosition(0,0), targetPosition(0,0), targetError(DEFAULT_TARGET_ERROR) {
}

BossAIState::~BossAIState() {
}

void BossAIState::UpdateMovement(double dT) {
    Boss* boss = this->GetBoss();

    // Figure out how much to move and update the position of the boss
    if (!this->currVel.IsZero()) {
        boss->alivePartsRoot->Translate(Vector3D(dT * this->currVel));
    }

    // Update the speed based on the acceleration
    this->currVel = this->currVel + dT * this->GetAcceleration();
    boss->alivePartsRoot->SetCollisionVelocity(this->currVel);
}

/// <summary> Move the boss to the currently set target position. </summary>
/// <returns> true if the boss made it to the target position, false if the boss is still moving to the target. </returns>
bool BossAIState::MoveToTargetPosition(float maxMoveSpd) {

    Point2D bossPos = this->GetBoss()->alivePartsRoot->GetTranslationPt2D();
    Vector2D startToTargetVec   = this->targetPosition - this->startPosition;
    Vector2D currPosToTargetVec = this->targetPosition - bossPos;

    if (currPosToTargetVec.SqrMagnitude() < this->targetError ||
        Vector2D::Dot(startToTargetVec, currPosToTargetVec) <= 0.0f) {

            // Stop the boss immediately
            this->desiredVel = Vector2D(0,0);
            this->currVel    = Vector2D(0,0);
            return true;
    }

    // We're not at or near the target position yet, get the velocity to move there
    // (unless we're getting close then we want to slow down and stop)

    // With a bit of kinematics we figure out when to start stopping...
    float distToTargetToStartStopping = -this->currVel.SqrMagnitude() / (2 * -this->GetAccelerationMagnitude());
    assert(distToTargetToStartStopping >= 0.0);
    if (currPosToTargetVec.SqrMagnitude() <= distToTargetToStartStopping * distToTargetToStartStopping) {
        this->desiredVel = Vector2D(0,0);
    }
    else {
        this->desiredVel = maxMoveSpd * Vector2D::Normalize(currPosToTargetVec);
    }

    return false;
}