/**
 * BossAIState.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "BossAIState.h"
#include "Boss.h"
#include "GameModel.h"

BossAIState::BossAIState() : currVel(0.0f, 0.0f), desiredVel(0.0f, 0.0f) {
}

BossAIState::~BossAIState() {
}

void BossAIState::UpdateMovement(double dT, GameModel* gameModel) {
    Boss* boss = this->GetBoss();

    // Figure out how much to move and update the position of the boss
    if (!this->currVel.IsZero()) {
        
        Vector2D dMovement = dT * this->currVel;
        boss->alivePartsRoot->Translate(Vector3D(dMovement));

        // Update the position of the boss based on whether it is now colliding with the boundaries/walls of the level
        const GameLevel* level = gameModel->GetCurrentLevel();
        Vector2D correctionVec;
        if (level->CollideBossWithLevel(boss->alivePartsRoot->GenerateWorldAABB(), correctionVec)) {
            
            Vector3D correctionVec3D(correctionVec);
            boss->alivePartsRoot->Translate(correctionVec3D);
        }
    }

    // Update the speed based on the acceleration
    this->currVel = this->currVel + dT * this->GetAcceleration();
    boss->alivePartsRoot->SetCollisionVelocity(this->currVel);
}