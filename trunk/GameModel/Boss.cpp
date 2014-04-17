/**
 * Boss.cpp
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

#include "Boss.h"
#include "AbstractBossBodyPart.h"
#include "BossBodyPart.h"
#include "BossWeakpoint.h"
#include "BossAIState.h"
#include "ClassicalBoss.h"
#include "GothicRomanticBoss.h"
#include "NouveauBoss.h"
#include "DecoBoss.h"
#include "FuturismBoss.h"
//#include "DadaSurrealistBoss.h"
//#include "PomoBoss.h"

const double Boss::WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME = 1.5;
const double Boss::FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME = TOTAL_DEATH_ANIM_TIME / 1.25;
const double Boss::TOTAL_DEATH_ANIM_TIME = 5.25;

Boss::Boss() :  currAIState(NULL), nextAIState(NULL), root(NULL), 
deadPartsRoot(NULL), alivePartsRoot(NULL), isBossDeadAndLevelCompleted(false)  {
}

Boss::~Boss() {
    // Clean up the body parts
    for (std::vector<AbstractBossBodyPart*>::iterator iter = this->bodyParts.begin();
         iter != this->bodyParts.end(); ++iter) {

        AbstractBossBodyPart* part = *iter;
        delete part;
        part = NULL;
    }
    this->bodyParts.clear();

    this->root = NULL;
    this->alivePartsRoot = NULL;
    this->deadPartsRoot  = NULL;

	// Delete the state
    if (this->currAIState != NULL) {
	    delete this->currAIState;
	    this->currAIState = NULL;
    }
    if (this->nextAIState != NULL) {
        delete this->nextAIState;
        this->nextAIState = NULL;
    }
}

/**
 * Static factory function for building boss types based on a given world style.
 */
Boss* Boss::BuildStyleBoss(GameModel* gameModel, const GameWorld::WorldStyle& style) {
    assert(gameModel != NULL);

    Boss* boss = NULL;
    switch (style) {
        case GameWorld::Classical:
            boss = new ClassicalBoss();
            break;

        case GameWorld::GothicRomantic:
            boss = new GothicRomanticBoss();
            break;

        case GameWorld::Nouveau:
            boss = new NouveauBoss();
            break;

        case GameWorld::Deco:
            boss = new DecoBoss();
            break;

        case GameWorld::Futurism:
            boss = new FuturismBoss();
            break;

        //case GameWorld::SurrealDada:

        //case GameWorld::Postmodernism:

        default:
            assert(false);
            break;
    }
    boss->worldStyle = style;
    boss->gameModel  = gameModel;

    return boss;
}

Collision::AABB2D Boss::GenerateDyingAABB() const {
    return this->currAIState->GenerateDyingAABB();
}

void Boss::Tick(double dT, GameModel* gameModel) {
    // Tick the body parts
    this->root->Tick(dT);

    // Tick the current state of the boss
    if (this->currAIState != NULL) {
        this->currAIState->Tick(dT, gameModel);
    }

    // Update the AI state if necessary
    this->UpdateAIState();
}

/**
 * Queries for whether the boss is completely dead or not.
 * Returns: true if dead, false if not.
 */
bool Boss::GetIsStateMachineFinished() const {
    if (this->currAIState != NULL) {
        return this->currAIState->IsStateMachineFinished();
    }
    return false;
}

void Boss::CollisionOccurred(GameModel* gameModel, GameBall& ball, BossBodyPart* collisionPart) {
    ball.SetLastThingCollidedWith(collisionPart);

    // NOTE: Make sure the part is collided with FIRST - it informs the current state
    collisionPart->CollisionOccurred(gameModel, ball);
    this->currAIState->CollisionOccurred(gameModel, ball, collisionPart);
}

void Boss::CollisionOccurred(GameModel* gameModel, Projectile* projectile, BossBodyPart* collisionPart) {
    if (projectile->IsLastThingCollidedWith(collisionPart)) {
        return;
    }

    collisionPart->CollisionOccurred(gameModel, projectile);
    this->currAIState->CollisionOccurred(gameModel, projectile, collisionPart);
}

void Boss::CollisionOccurred(GameModel* gameModel, PlayerPaddle& paddle, BossBodyPart* collisionPart) {
    collisionPart->CollisionOccurred(gameModel, paddle);
    this->currAIState->CollisionOccurred(gameModel, paddle, collisionPart);
}

void Boss::MineExplosionOccurred(GameModel* gameModel, const MineProjectile* mine) {
    this->currAIState->MineExplosionOccurred(gameModel, mine);
}

void Boss::RocketExplosionOccurred(GameModel* gameModel, const RocketProjectile* rocket) {
    this->currAIState->RocketExplosionOccurred(gameModel, rocket);
}

void Boss::TeslaLightningArcHitOccurred(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2) {
    this->currAIState->TeslaLightningArcHitOccurred(gameModel, block1, block2);
}

bool Boss::CanHurtPaddleWithBody() const {
    if (this->currAIState != NULL) {
        return this->currAIState->CanHurtPaddleWithBody();
    }
    return false;
}

bool Boss::ProjectileIsDestroyedOnCollision(const Projectile* projectile, BossBodyPart* collisionPart) const {
    UNUSED_PARAMETER(collisionPart);

    // Mines can land on the boss and are not destroyed by it
    if (projectile->IsMine()) {
        return false;
    }
    return !this->ProjectilePassesThrough(projectile);
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossHurtAndInvulnerableColourAnim(double invulnerableTimeInSecs) {
    const int NUM_FLASHES = 20;
    
    std::vector<double> timeValues;
    timeValues.reserve(2*NUM_FLASHES + 1);
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(timeValues.size());
    
    double timeInc = invulnerableTimeInSecs / static_cast<double>(2*NUM_FLASHES);
    double timeCount = 0.0;
    
    for (int i = 0; i <= 2*NUM_FLASHES; i++) {
        timeValues.push_back(timeCount);
        timeCount += timeInc;
    }
    for (int i = 0; i < NUM_FLASHES; i++) {
        colourValues.push_back(ColourRGBA(1.0f, 0.25f, 0.25f, 1.0f));
        colourValues.push_back(ColourRGBA(1.0f, 0.9f, 0.9f, 0.5f));
    }
    colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    AnimationMultiLerp<ColourRGBA> hurtColourAnim;
    hurtColourAnim.SetLerp(timeValues, colourValues);
    hurtColourAnim.SetRepeat(false);

    return hurtColourAnim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossElectrifiedColourAnim(double totalAnimTime, 
                                                                    const Colour& colour1, 
                                                                    const Colour& colour2, 
                                                                    const Colour& colour3) {
    const double SECS_PER_FLASH = 0.2;

    int numFlashes = static_cast<int>(totalAnimTime / SECS_PER_FLASH);

    std::vector<double> timeValues;
    timeValues.reserve(2*numFlashes + 1);
    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(timeValues.size());

    double timeInc = totalAnimTime / static_cast<double>(2*numFlashes);
    double timeCount = 0.0;

    for (int i = 0; i <= 2*numFlashes; i++) {
        timeValues.push_back(timeCount);
        timeCount += timeInc;
    }
    const Colour* currColour;
    for (int i = 0; i < numFlashes; i++) {
        colourValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, static_cast<float>(0.5f + 0.25f*Randomizer::GetInstance()->RandomNumZeroToOne())));

        switch (Randomizer::GetInstance()->RandomUnsignedInt() % 3) {
            case 0:
                currColour = &colour1;
                break;
            case 1:
                currColour = &colour2;
                break;
            case 2:
            default:
                currColour = &colour3;
                break;
        }

        colourValues.push_back(ColourRGBA(*currColour, static_cast<float>(0.6f + 0.30f*Randomizer::GetInstance()->RandomNumZeroToOne())));
    }
    colourValues.push_back(ColourRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    AnimationMultiLerp<ColourRGBA> colourAnim;
    colourAnim.SetLerp(timeValues, colourValues);
    colourAnim.SetRepeat(false);

    return colourAnim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossHurtFlashAndFadeAnim(double totalAnimTime) {
    const double FIRST_FADE_OUT_TIME = 0.5;
    const int NUM_FLASHES = static_cast<int>(25 * totalAnimTime / 4.0);
    const double FLASH_TIME_INC = (totalAnimTime - FIRST_FADE_OUT_TIME) / (2*NUM_FLASHES + 1);

    std::vector<double> timeValues;
    timeValues.reserve(2 + 2*NUM_FLASHES + 1);
    timeValues.push_back(0.0);
    timeValues.push_back(FIRST_FADE_OUT_TIME);
    for (int i = 0; i <= 2*NUM_FLASHES; i++) {
        timeValues.push_back(timeValues.back() + FLASH_TIME_INC);
    }

    const float FIRST_ALPHA_FADE_OUT_VALUE  = 0.5f;
    const float SECOND_ALPHA_FADE_OUT_VALUE = 0.25f;
    std::vector<ColourRGBA> alphaValues;
    alphaValues.reserve(timeValues.size());
    alphaValues.push_back(ColourRGBA(1,1,1,1));
    alphaValues.push_back(ColourRGBA(1,1,1,FIRST_ALPHA_FADE_OUT_VALUE));
    for (int i = 0; i < NUM_FLASHES; i++) {
        alphaValues.push_back(ColourRGBA(1.0f, 0.0f, 0.0f, SECOND_ALPHA_FADE_OUT_VALUE));
        alphaValues.push_back(ColourRGBA(1.0f, 0.8f, 0.8f, FIRST_ALPHA_FADE_OUT_VALUE));
    }
    alphaValues.push_back(ColourRGBA(0.0f, 0.0f, 0.0f, 0.0f));

    AnimationMultiLerp<ColourRGBA> anim;
    anim.SetLerp(timeValues, alphaValues);
    anim.SetRepeat(false);

    return anim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossFinalDeathFlashAnim() {
    std::vector<double> timeValues;
    timeValues.reserve(4);
    timeValues.push_back(0.0);
    timeValues.push_back(WAIT_BEFORE_FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME);
    timeValues.push_back(FADE_TO_BLACK_FINAL_DEAD_BODY_PART_TIME);
    timeValues.push_back(TOTAL_DEATH_ANIM_TIME);

    std::vector<ColourRGBA> colourValues;
    colourValues.reserve(timeValues.size());
    colourValues.push_back(ColourRGBA(1,1,1,1));
    colourValues.push_back(ColourRGBA(1,1,1,1));
    colourValues.push_back(ColourRGBA(0,0,0,1));
    colourValues.push_back(ColourRGBA(0,0,0,0));

    AnimationMultiLerp<ColourRGBA> anim;
    anim.SetLerp(timeValues, colourValues);
    anim.SetRepeat(false);

    return anim;
}

AnimationMultiLerp<ColourRGBA> Boss::BuildBossAngryFlashAnim() {
    const double COLOUR_FLASH_TIME = 0.2;
    
    std::vector<double> timeVals;
    timeVals.reserve(5);
    timeVals.push_back(0.0);
    timeVals.push_back(COLOUR_FLASH_TIME);
    timeVals.push_back(2*COLOUR_FLASH_TIME);
    timeVals.push_back(3*COLOUR_FLASH_TIME);
    timeVals.push_back(4*COLOUR_FLASH_TIME);

    std::vector<ColourRGBA> colourVals;
    colourVals.reserve(timeVals.size());
    colourVals.push_back(ColourRGBA(1,1,1,1));
    colourVals.push_back(ColourRGBA(1,0,0,1));
    colourVals.push_back(ColourRGBA(1,1,0,1));
    colourVals.push_back(ColourRGBA(1,0,0,1));
    colourVals.push_back(ColourRGBA(1,1,1,1));

    AnimationMultiLerp<ColourRGBA> angryColourAnim;
    angryColourAnim.SetLerp(timeVals, colourVals);
    angryColourAnim.SetRepeat(true);
    return angryColourAnim;
}

AnimationMultiLerp<Vector3D> Boss::BuildBossAngryShakeAnim(float shakeMagnitude) {
    const int NUM_SHAKES = 15;
    const double SHAKE_INC_TIME = 0.03;
    
    std::vector<double> timeVals;
    timeVals.reserve(1 + NUM_SHAKES + 1);
    timeVals.push_back(0.0);
    for (int i = 0; i <= NUM_SHAKES*2; i++) {
        timeVals.push_back(timeVals.back() + SHAKE_INC_TIME);
    }

    std::vector<Vector3D> moveVals;
    moveVals.reserve(timeVals.size());
    moveVals.push_back(Vector3D(0,0,0));
    for (int i = 0; i < NUM_SHAKES; i++) {
        float randomNum1 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude;
        float randomNum2 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude;
        moveVals.push_back(Vector3D(randomNum1, randomNum2, 0));
        moveVals.push_back(Vector3D(-randomNum1, -randomNum2, 0));
    }
    moveVals.push_back(Vector3D(0.0f, 0.0f, 0.0f));
    
    AnimationMultiLerp<Vector3D> angryMoveAnim;
    angryMoveAnim.SetLerp(timeVals, moveVals);
    angryMoveAnim.SetRepeat(false);
    return angryMoveAnim;
}

AnimationMultiLerp<Vector3D> Boss::BuildBossFinalDeathShakeAnim(float shakeMagnitude) {
    AnimationMultiLerp<Vector3D> anim = Boss::BuildBossAngryShakeAnim(shakeMagnitude);
    anim.SetRepeat(true);
    return anim;
}

AnimationMultiLerp<Vector3D> Boss::BuildBossHurtMoveAnim(const Vector2D& hurtVec, float shakeMagnitude, double invulnerableTimeInSecs) {
    AnimationMultiLerp<Vector3D> result;

    const double FINAL_JITTER_TIME = 0.3;
    const double SHAKE_INC_TIME = 0.075;
    const int NUM_SHAKES = (invulnerableTimeInSecs - FINAL_JITTER_TIME) / (2*SHAKE_INC_TIME + SHAKE_INC_TIME);

    std::vector<double> timeValues;
    timeValues.clear();
    timeValues.reserve(3 + NUM_SHAKES + 1);
    timeValues.push_back(0.0);
    timeValues.push_back(2.0 * FINAL_JITTER_TIME / 3.0);
    timeValues.push_back(FINAL_JITTER_TIME);
    for (int i = 0; i <= NUM_SHAKES*2; i++) {
        timeValues.push_back(timeValues.back() + SHAKE_INC_TIME);
    }
    assert(timeValues.back() <= invulnerableTimeInSecs);

    Vector2D hurtPos2D = shakeMagnitude * hurtVec;

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D(hurtPos2D[0], hurtPos2D[1], 0.0f));
    moveValues.push_back(Vector3D(0,0,0));
    for (int i = 0; i < NUM_SHAKES; i++) {
        float randomVal1 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude / 5.0f;
        float randomVal2 = Randomizer::GetInstance()->RandomNumNegOneToOne() * shakeMagnitude / 5.0f;
        moveValues.push_back(Vector3D(randomVal1, randomVal2, 0));
        moveValues.push_back(Vector3D(-randomVal1, -randomVal2, 0));
    }
    moveValues.push_back(Vector3D(0.0f, 0.0f, 0.0f));

    result.SetLerp(timeValues, moveValues);
    result.SetRepeat(false);
    return result;
}

AnimationMultiLerp<Vector3D> Boss::BuildShakeAnim(double startTime, double totalAnimTime, float shakeFreq, 
                                                  float shakeMagX, float shakeMagY) {
    AnimationMultiLerp<Vector3D> shakeAnim;

    int numShakes = static_cast<int>(totalAnimTime * shakeFreq);
    double lastShakeTime = totalAnimTime * shakeFreq - static_cast<double>(numShakes);
    double timeOfEachShake = totalAnimTime / static_cast<double>(numShakes);
    double halfTimeOfEachShake = timeOfEachShake / 2.0;

    std::vector<double> timeValues;
    timeValues.reserve(2*numShakes+2);
    timeValues.push_back(startTime);
    for (int i = 0; i < numShakes; i++) {
        timeValues.push_back(timeValues.back() + halfTimeOfEachShake);
        timeValues.push_back(timeValues.back() + halfTimeOfEachShake);
    }
    timeValues.push_back(timeValues.back() + lastShakeTime);

    std::vector<Vector3D> movementValues;
    movementValues.reserve(timeValues.size());
    movementValues.push_back(Vector3D(0,0,0));
    for (int i = 0; i < numShakes; i++) {
        float randomNum1 = Randomizer::GetInstance()->RandomNegativeOrPositive() * 
            (0.1f*shakeMagX + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.9f*shakeMagX);
        float randomNum2 = Randomizer::GetInstance()->RandomNegativeOrPositive() * 
            (0.1f*shakeMagY + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.9f*shakeMagY);
        movementValues.push_back(Vector3D(randomNum1, randomNum2, 0));
        movementValues.push_back(Vector3D(-randomNum1, -randomNum2, 0));
    }
    movementValues.push_back(Vector3D(0,0,0));
    
    shakeAnim.SetLerp(timeValues, movementValues);
    shakeAnim.SetRepeat(false);

    return shakeAnim;
}

AnimationMultiLerp<float> Boss::BuildLimbShakeAnim(float limbSize) {
    AnimationMultiLerp<float> armShakeAnim;

    static const int NUM_ANIM_KEYFRAMES = 32;

    std::vector<double> timeValues;
    timeValues.reserve(NUM_ANIM_KEYFRAMES);
    timeValues.push_back(0.0);
    for (int i = 1; i < NUM_ANIM_KEYFRAMES; i++) {
        timeValues.push_back(timeValues.back() + 0.0015 + Randomizer::GetInstance()->RandomNumZeroToOne() * 0.035);
    }

    const float MAX_MOVE = 0.15f * limbSize;
    int neg = Randomizer::GetInstance()->RandomNegativeOrPositive();
    std::vector<float> movementValues;
    movementValues.reserve(NUM_ANIM_KEYFRAMES);
    movementValues.push_back(0.0f);
    for (int i = 1; i < NUM_ANIM_KEYFRAMES; i++) {
        movementValues.push_back(neg * (MAX_MOVE * 0.01f + Randomizer::GetInstance()->RandomNumZeroToOne() * MAX_MOVE * 0.99f));
        neg = -neg;
    }

    armShakeAnim.SetLerp(timeValues, movementValues);
    armShakeAnim.SetRepeat(true);

    return armShakeAnim;
}

AnimationMultiLerp<Vector3D> Boss::BuildLimbFallOffTranslationAnim(double totalAnimTime, float xDist, float yDist) {
    std::vector<double> timeValues;
    timeValues.reserve(2);
    timeValues.push_back(0.0);
    timeValues.push_back(totalAnimTime);

    std::vector<Vector3D> moveValues;
    moveValues.reserve(timeValues.size());
    moveValues.push_back(Vector3D(0,0,0));
    moveValues.push_back(Vector3D(xDist, yDist, 0.0f));

    AnimationMultiLerp<Vector3D> limbFallOffTransAnim;
    limbFallOffTransAnim.SetLerp(timeValues, moveValues);
    limbFallOffTransAnim.SetRepeat(false);

    return limbFallOffTransAnim;
}

AnimationMultiLerp<float> Boss::BuildLimbFallOffZRotationAnim(double totalAnimTime, float rotAmtInDegs) {
    std::vector<double> timeValues;
    timeValues.reserve(3);
    timeValues.push_back(0.0);
    timeValues.push_back(totalAnimTime);

    std::vector<float> rotationValues;
    rotationValues.reserve(timeValues.size());
    rotationValues.push_back(0.0f);
    rotationValues.push_back(rotAmtInDegs);

    AnimationMultiLerp<float> limbFallOffRotAnim;
    limbFallOffRotAnim.SetLerp(timeValues, rotationValues);
    limbFallOffRotAnim.SetRepeat(false);

    return limbFallOffRotAnim;
}

void Boss::ConvertAliveBodyPartToWeakpoint(size_t index, float lifePoints, float ballDmgOnHit) {
    assert(index < this->bodyParts.size());

    AbstractBossBodyPart* bodyPart = this->bodyParts[index];
    assert(bodyPart != NULL);
    if (dynamic_cast<BossBodyPart*>(bodyPart) == NULL ||
        dynamic_cast<BossWeakpoint*>(bodyPart) != NULL ||
        !this->alivePartsRoot->IsOrContainsPart(bodyPart, true)) {

        assert(false);
        return;
    }

    // Create a weakpoint part
    BossWeakpoint* weakpointBodyPart = BossWeakpoint::BuildWeakpoint(static_cast<BossBodyPart*>(bodyPart), lifePoints, ballDmgOnHit);
    assert(weakpointBodyPart != NULL);
    
    AbstractBossBodyPart* parentPart = this->alivePartsRoot->SearchForParent(bodyPart);
    if (parentPart != NULL) {
        assert(dynamic_cast<BossCompositeBodyPart*>(parentPart) != NULL);
        
        BossCompositeBodyPart* compositeParentPart = static_cast<BossCompositeBodyPart*>(parentPart);
        compositeParentPart->RemoveBodyPart(bodyPart);
        compositeParentPart->AddBodyPart(weakpointBodyPart);
    }

    delete bodyPart;
    bodyPart = NULL;

    this->bodyParts[index] = weakpointBodyPart;
}

void Boss::ConvertAliveBodyPartToDeadBodyPart(size_t index) {
    assert(index < this->bodyParts.size());
    this->ConvertAliveBodyPartToDeadBodyPart(this->bodyParts[index]);
}

void Boss::ConvertAliveBodyPartToDeadBodyPart(AbstractBossBodyPart* bodyPart) {
    // Make sure the body part exists as an alive part of this boss,
    // the parent better be the alivePartsRoot too.
    assert(bodyPart != NULL);
    if (!this->alivePartsRoot->IsOrContainsPart(bodyPart, true)) {
        assert(false);
        return;
    }

    AbstractBossBodyPart* parentPart = this->alivePartsRoot->SearchForParent(bodyPart);
    if (parentPart == NULL) {
        assert(false);
        return;
    }

    assert(dynamic_cast<BossCompositeBodyPart*>(parentPart) != NULL);
    BossCompositeBodyPart* compositeParentPart = static_cast<BossCompositeBodyPart*>(parentPart);

    // Move it from the parent in the alivePartsRoot tree, and move it to the deadPartsRoot tree
    compositeParentPart->RemoveBodyPart(bodyPart);
    this->deadPartsRoot->AddBodyPart(bodyPart);

    // Find all weakpoints and turn them off
    bodyPart->SetDestroyed(true);
}

void Boss::SetNextAIState(BossAIState* nextState) {
    if (this->nextAIState != NULL) {
    	delete this->nextAIState;
    }
    this->nextAIState = nextState;
}

void Boss::UpdateAIState() {
	if (this->nextAIState != NULL) {
		this->SetCurrentAIStateImmediately(this->nextAIState);
		this->nextAIState = NULL;
	}
}

void Boss::SetCurrentAIStateImmediately(BossAIState* newState) {
	assert(newState != NULL);
	if (this->currAIState != NULL) {
		delete this->currAIState;
	}
	this->currAIState = newState;
}

#ifdef _DEBUG
void Boss::DebugDraw() const {
    if (this->root != NULL) {
        this->alivePartsRoot->DebugDraw();
    }
}
#endif