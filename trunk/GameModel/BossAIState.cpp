/**
 * BossAIState.cpp
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

#include "BossAIState.h"
#include "Boss.h"
#include "GameModel.h"

const float BossAIState::DEFAULT_TARGET_ERROR = 0.1f * LevelPiece::PIECE_HEIGHT;

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