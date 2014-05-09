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
    if (this->currVel != this->desiredVel) {
        
        Vector2D prevVel = this->currVel;
        
        float accelMagnitude;
        Vector2D accelVec = this->GetAcceleration(accelMagnitude);
        Vector2D dA = dT * accelVec;
        Vector2D velDiff = this->desiredVel - this->currVel;
        if (velDiff.SqrMagnitude() < dT*dT*accelMagnitude*accelMagnitude) {
            dA = velDiff;
        }

        this->currVel = this->currVel + dA;

        /*
        // Make sure we haven't exceeded the desired velocity
        if (this->desiredVel.IsZero()) {
            // If the velocity just changed directions then we set it to zero
            if (Vector2D::Dot(prevVel, this->currVel) < 0) {
                this->currVel = this->desiredVel;
            }
        }
        else {
            float currSqrSpd    = this->currVel.SqrMagnitude();
            float desiredSqrSpd = this->desiredVel.SqrMagnitude();
            float dotProd = Vector2D::Dot(this->currVel, this->desiredVel);
            if (dotProd > 0 && fabs(dotProd*dotProd - currSqrSpd*desiredSqrSpd) < EPSILON) {
                if (currSqrSpd > desiredSqrSpd) {
                    this->currVel.Normalize();
                    this->currVel *= sqrt(desiredSqrSpd);
                }
            }
        }
        */
    }

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

    // We're not at the target position yet, get the velocity to move there
    // (unless we're getting close then we want to slow down and stop)

    float currSqrSpd = this->currVel.SqrMagnitude();

    // With a bit of kinematics we figure out when to start stopping...
    float distToTargetToStartStopping = -currSqrSpd / (2 * -this->GetAccelerationMagnitude());
    assert(distToTargetToStartStopping >= 0.0);
    if (currPosToTargetVec.SqrMagnitude()-EPSILON <= distToTargetToStartStopping * distToTargetToStartStopping) {
        this->desiredVel = Vector2D(0,0);
    }
    else {
        this->desiredVel = maxMoveSpd * Vector2D::Normalize(currPosToTargetVec);
    }

    return false;
}

bool LargestToSmallestDistPoint2DSorter(const std::pair<float, Point2D>& a, const std::pair<float, Point2D>& b) {
    return (a.first > b.first);
}

/// <summary>
/// Gets the furthest distance positions (the number of these chosen positions is the one given)
/// from the current boss position.
/// NOTE: 'positions' and 'result' may reference the same vector in memory.
/// </summary>
/// <param name="currBossPos"> The boss' current position. </param>
/// <param name="positions"> The overall possible positions. </param>
/// <param name="numPositionsToChoose"> Number of positions to choose from. </param>
/// <param name="result"> [in,out] The resulting, chosen furthest positions (size == numPositionsToChoose). </param>
void BossAIState::GetFurthestDistFromBossPositions(const Point2D& currBossPos, const std::vector<Point2D>& positions, 
                                                   int numPositionsToChoose, std::vector<Point2D>& result) {

    assert(static_cast<int>(positions.size()) >= numPositionsToChoose);

    std::vector<std::pair<float, Point2D> > distPosPairs;
    distPosPairs.reserve(positions.size());
    for (int i = 0; i < static_cast<int>(positions.size()); i++) {
        distPosPairs.push_back(std::make_pair(Point2D::SqDistance(currBossPos, positions[i]), positions[i]));
    }

    // Sort by the distances (largest to smallest)
    std::sort(distPosPairs.begin(), distPosPairs.end(), LargestToSmallestDistPoint2DSorter);

    // Pick the last three positions among the pairs...
    assert(static_cast<int>(distPosPairs.size()) >= numPositionsToChoose);
    result.resize(numPositionsToChoose);
    for (int i = 0; i < numPositionsToChoose; i++) {
        result[i] = distPosPairs[i].second;
    }

    assert(static_cast<int>(result.size()) == numPositionsToChoose);
}