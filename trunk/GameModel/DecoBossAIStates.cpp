/**
 * DecoBossAIStates.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#include "DecoBossAIStates.h"
#include "DecoBoss.h"
#include "PlayerPaddle.h"

using namespace decobossai;


DecoBossAIState::DecoBossAIState(DecoBoss* boss) : boss(boss) {

}

DecoBossAIState::~DecoBossAIState() {
}

Boss* DecoBossAIState::GetBoss() const {
    return this->boss;
}

Collision::AABB2D DecoBossAIState::GenerateDyingAABB() const {
    return this->boss->GetCore()->GenerateWorldAABB();
}


// DecoBossStage1 Functions **************************************************************

const float DecoBossStage1::DEFAULT_ACCELERATION = 0.25f * PlayerPaddle::DEFAULT_ACCELERATION;

DecoBossStage1::DecoBossStage1(DecoBoss* boss) : DecoBossAIState(boss) {

}

DecoBossStage1::~DecoBossStage1() {
}

void DecoBossStage1::SetState(DecoBossAIState::AIState newState) {

}